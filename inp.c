//    Organic Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for organic solar cells. 
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//	roderick.mackenzie@nottingham.ac.uk
//	www.roderickmackenzie.eu
//	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <unistd.h>
#include <fcntl.h>

#include "inp.h"
#include "util.h"
#include "code_ctrl.h"
#include "true_false.h"

int inp_search_pos(struct inp_file *in, char *token)
{
	int pos = 0;
	inp_reset_read(in);
	char *line = NULL;
	do {
		line = inp_get_string(in);

		if (strcmp(line, token) == 0) {
			return pos;
		}

		pos++;

	} while (line != NULL);

	return -1;
}

void inp_reset_read(struct inp_file *in)
{
	in->pos = 0;
}

char *inp_get_string(struct inp_file *in)
{
	int i;
	static char ret[100];
	memset(ret, 0, 100);
	int ii = 0;
	if (in->pos >= in->fsize) {
		return NULL;
	}

	for (i = in->pos; i < in->fsize; i++) {
		if ((in->data[i] == '\n') || (in->data[i] == 0)) {
			ret[ii] = 0;
			in->pos++;
			break;
		}

		ret[ii] = in->data[i];
		ii++;
		in->pos++;

	}

	return ret;
}

int inp_read_buffer(char **buf, long *len, char *full_file_name)
{

	FILE *f = fopen(full_file_name, "rb");
	if (f != NULL) {
		fseek(f, 0, SEEK_END);
		*len = ftell(f);
		fseek(f, 0, SEEK_SET);

		*buf = malloc(((*len) + 2) * sizeof(char));
		memset(*buf, 0, ((*len) + 2) * sizeof(char));
		fread(*buf, *len, 1, f);
		fclose(f);
		return 0;

	} else {
		char zip_path[1000];
		char *file_path = get_dir_name_from_path(full_file_name);
		char *file_name = get_file_name_from_path(full_file_name);

		sprintf(zip_path, "%s/sim.opvdm", file_path);
		int err = 0;
		struct zip *z = zip_open(zip_path, 0, &err);

		if (z != NULL) {

			struct zip_stat st;
			zip_stat_init(&st);
			int ret = zip_stat(z, file_name, 0, &st);

			if (ret == 0) {

				*len = st.size * sizeof(char);
				*buf = (char *)malloc(*len);

				struct zip_file *f = zip_fopen(z, file_name, 0);
				zip_fread(f, *buf, st.size);
				zip_fclose(f);

			} else {
				ewe("File %s not found\n", file_name);
			}
			zip_close(z);
			return 0;
		} else {
			return -1;
		}

	}

}

void inp_init(struct inp_file *in)
{
	strcpy(in->full_name, "");
	in->data = NULL;
	in->fsize = 0;
	in->pos = 0;
	in->edited = FALSE;
}

void inp_load_from_path(struct inp_file *in, char *path, char *file)
{
	char temp[1000];
	sprintf(temp, "%s/%s", path, file);
	inp_load(in, temp);
}

void inp_load(struct inp_file *in, char *file)
{
	in->pos = 0;

	if (strcmp(in->full_name, file) != 0) {

		if (in->data != NULL) {
			inp_free(in);
		}

		strcpy(in->full_name, file);
		inp_read_buffer(&(in->data), &(in->fsize), file);

		in->edited = FALSE;
	}

}

void inp_replace(struct inp_file *in, char *token, char *text)
{
	char *temp = malloc(in->fsize + 100);
	memset(temp, 0, in->fsize + 100);
	char *line;
	int len = 0;

	line = strtok(in->data, "\n");
	int found = FALSE;

	while (line) {
		if (strcmp(line, token) != 0) {
			strcat(temp, line);
			strcat(temp, "\n");
		} else {
			strcat(temp, line);
			strcat(temp, "\n");
			strcat(temp, text);
			strcat(temp, "\n");
			line = strtok(NULL, "\n");
			found = TRUE;
		}
		line = strtok(NULL, "\n");
	}

	len = strlen(temp);
	in->fsize = len;

	if (found == TRUE) {
		in->edited = TRUE;
	}

	in->data = realloc(in->data, (len + 1) * sizeof(char));
	memcpy(in->data, temp, (len + 1) * sizeof(char));

	if (in->data[len] != 0) {
		printf("%s %d\n", in->data, len);
		ewe("String not ended\n");
	}
	free(temp);
}

void inp_save(struct inp_file *in)
{

	if (in->edited == TRUE) {
		int out_fd =
		    open(in->full_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);

		if (out_fd == -1) {
			ewe("File %s can not be opened\n", in->full_name);
		}

		ftruncate(out_fd, 0);

		write(out_fd, in->data, in->fsize * sizeof(char));
		close(out_fd);

		in->edited = FALSE;

		/*FILE *f = fopen(in->full_name, "rb");
		   if (f!=NULL)
		   {
		   fseek(f, 0, SEEK_END);
		   int len = ftell(f);
		   fseek(f, 0, SEEK_SET);

		   char *buf = malloc(((len) + 2)*sizeof(char));
		   memset(buf, 0, ((len) + 2)*sizeof(char));
		   fread(buf, len, 1, f);
		   printf("read'%s' %d\n",buf,len);
		   fclose(f);
		   if (strcmp(buf,in->data)!=0)
		   {
		   ewe("Buffers did not match\n");
		   }
		   free(buf);
		   }else
		   {
		   ewe("Can't open file\n");
		   } */

	}

}

void inp_free(struct inp_file *in)
{

	inp_save(in);

	free(in->data);
	inp_init(in);
}

void inp_search_double(struct inp_file *in, double *out, char *token)
{
	sscanf(inp_search(in, token), "%le", out);
}

void inp_search_int(struct inp_file *in, int *out, char *token)
{
	sscanf(inp_search(in, token), "%d", out);
}

void inp_search_string(struct inp_file *in, char *out, char *token)
{
	strcpy(out, inp_search(in, token));
}

void inp_check(struct inp_file *in, double ver)
{
	double read_ver = 0.0;
	inp_reset_read(in);
	char *line = inp_get_string(in);
	while (line != NULL) {

		if (strcmp(line, "#ver") == 0) {
			line = inp_get_string(in);
			sscanf(line, "%le", &read_ver);

			if (ver != read_ver) {
				ewe("File compatability problem %s\n",
				    in->full_name);
			}
			line = inp_get_string(in);

			if ((line == NULL) || (strcmp(line, "#end") != 0)) {
				ewe("#end token missing %s\n", in->full_name);
			}

			return;
		}

		line = inp_get_string(in);
	}
	ewe("Token #ver not found in %s\n", in->full_name);
	return;
}

char *inp_search_part(struct inp_file *in, char *token)
{
	inp_reset_read(in);
	char *line = inp_get_string(in);
	while (line != NULL) {

		if (cmpstr_min(line, token) == 0) {
			return line;
		}

		line = inp_get_string(in);
	}

	return NULL;
}

char *inp_search(struct inp_file *in, char *token)
{
	inp_reset_read(in);
	char *line = inp_get_string(in);
	while (line != NULL) {

		if (strcmp(line, token) == 0) {
			line = inp_get_string(in);
			return line;
		}

		line = inp_get_string(in);
	}
	ewe("Token %s not found in file %s", token, in->full_name);
	exit(0);
	return NULL;
}
