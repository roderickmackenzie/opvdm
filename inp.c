//    Organic Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for organic solar cells. 
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
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
#include <dirent.h>

#include "inp.h"
#include "util.h"
#include "code_ctrl.h"
#include "true_false.h"

void inp_listdir(struct inp_list *out)
{
	char pwd[1000];
	int mylen = 0;
	int i = 0;
	int err = 0;
	char temp[200];
	if (getcwd(pwd, 1000) == NULL) {
		ewe("IO error\n");
	}

	out->names = (char **)malloc(sizeof(char *) * 2000);
	out->len = 0;

	struct zip *z = zip_open("sim.opvdm", 0, &err);

	if (z != NULL) {
		int files = zip_get_num_files(z);
		for (i = 0; i < files; i++) {
			strcpy(temp, zip_get_name(z, i, ZIP_FL_UNCHANGED));
			if (inp_listcmp(out, temp) != 0) {
				mylen = strlen(temp);
				out->names[out->len] =
				    (char *)malloc(sizeof(char) * (mylen + 1));
				strcpy(out->names[out->len], temp);
				out->len++;
			}

		}

		zip_close(z);

	}

}

void inp_list_free(struct inp_list *in)
{
	int i = 0;

	for (i = 0; i < in->len; i++) {
		free(in->names[i]);
	}

	free(in->names);
}

int inp_listcmp(struct inp_list *in, char *name)
{
	int i = 0;

	for (i = 0; i < in->len; i++) {
		if (strcmp(name, in->names[i]) == 0) {
			return 0;
		}
	}

	return -1;
}

int isfile(char *in)
{
	FILE *f = fopen(in, "r");

	if (f == NULL) {
		return -1;
	}

	fclose(f);
	return 0;

}

int zip_is_in_archive(char *full_file_name)
{
	char zip_path[1000];
	char *file_path = get_dir_name_from_path(full_file_name);
	char *file_name = get_file_name_from_path(full_file_name);

	join_path(2, zip_path, file_path, "sim.opvdm");

	int err = 0;
	struct zip *z = zip_open(zip_path, 0, &err);

	if (z != NULL) {
		//Search for the file of given name
		struct zip_stat st;
		zip_stat_init(&st);
		int ret = zip_stat(z, file_name, 0, &st);

		if (ret == 0) {
			return 0;
		} else {
			return -1;
		}
		zip_close(z);
		return 0;
	} else {
		return -1;
	}
}

int inp_isfile(char *full_file_name)
{
	FILE *f = fopen(full_file_name, "rb");
	if (f != NULL) {
		fclose(f);
		return 0;
	} else {
		return zip_is_in_archive(full_file_name);
	}
//#endif

}

int inp_search_pos(struct inp_file *in, char *token)
{
	int pos = 0;
	inp_reset_read(in);
	char *line = NULL;
	do {
		line = inp_get_string(in);
		//printf("'%s' '%s'\n", line,token);
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
		int err = 0;
		fseek(f, 0, SEEK_END);
		*len = ftell(f);
		fseek(f, 0, SEEK_SET);

		*buf = malloc(((*len) + 2) * sizeof(char));
		memset(*buf, 0, ((*len) + 2) * sizeof(char));
		if (fread(*buf, *len, 1, f) == 0) {
			err = -1;
		}
		fclose(f);
		return err;

	} else {
		char zip_path[1000];
		char *file_path = get_dir_name_from_path(full_file_name);
		char *file_name = get_file_name_from_path(full_file_name);

		join_path(2, zip_path, file_path, "sim.opvdm");
		//printf("1>%s 2>%s 3>%s 4>%s\n",full_file_name,file_path,file_name,zip_path);
		int err = 0;
		struct zip *z = zip_open(zip_path, 0, &err);

		if (z != NULL) {
			//Search for the file of given name
			struct zip_stat st;
			zip_stat_init(&st);
			int ret = zip_stat(z, file_name, 0, &st);

			if (ret == 0) {
				//printf ("Read zip file!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

				//Alloc memory for its uncompressed contents
				*len = st.size * sizeof(char);
				*buf = (char *)malloc(*len);

				//Read the compressed file
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

int inp_load_from_path(struct inp_file *in, char *path, char *file)
{
	char full_path[1000];
	join_path(2, full_path, path, file);
	return inp_load(in, full_path);
}

int inp_load(struct inp_file *in, char *file)
{
	int ret = 0;
	in->pos = 0;
	if (strcmp(in->full_name, file) != 0) {
		//printf("Reload %s %s\n",in->full_name,file);
		if (in->data != NULL) {
			inp_free(in);
		}

		strcpy(in->full_name, file);
		if (inp_read_buffer(&(in->data), &(in->fsize), file) != 0) {
			ret = -1;
		}

		in->edited = FALSE;
	}

	return ret;
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

int zip_write_buffer(char *full_file_name, char *buffer, int len)
{
	int in_zip_file = -1;
	int outside_zip_file = -1;

	in_zip_file = zip_is_in_archive(full_file_name);
	outside_zip_file = isfile(full_file_name);

	if ((in_zip_file != 0) || (outside_zip_file == 0)) {
		int out_fd =
		    open(full_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);

		if (out_fd == -1) {
			ewe("File %s can not be opened\n", full_file_name);
		}

		if (ftruncate(out_fd, 0) == -1) {
			ewe("Error with truncate command\n");
		}

		if (write(out_fd, buffer, len * sizeof(char)) == -1) {
			ewe("Error writing data to file.\n");
		}
		close(out_fd);
	} else {
		char zip_path[1000];
		char *file_path = get_dir_name_from_path(full_file_name);
		char *file_name = get_file_name_from_path(full_file_name);

		join_path(2, zip_path, file_path, "sim.opvdm");

		int err = 0;
		struct zip *z = zip_open(zip_path, 0, &err);

		if (z != NULL) {
			struct zip_source *s;
			s = zip_source_buffer(z, buffer, len, 0);
			int index = zip_name_locate(z, file_name, 0);

			if (index == -1) {
				zip_add(z, file_name, s);
			} else {
				zip_replace(z, index, s);
			}

			zip_close(z);
		} else {
			return -1;
		}

	}
	return 0;
}

int inp_save(struct inp_file *in)
{

	if (in->edited == TRUE) {
		zip_write_buffer(in->full_name, in->data, in->fsize);
		in->edited = FALSE;
	}

	return 0;
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
		//printf("'%s' '%s'\n", line,token);
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
		//printf("'%s' '%s'\n", line,token);
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

int inp_search_english(struct inp_file *in, char *token)
{
	inp_reset_read(in);
	char *line = inp_get_string(in);
	while (line != NULL) {
		//printf("'%s' '%s'\n", line,token);
		if (strcmp(line, token) == 0) {
			line = inp_get_string(in);
			return english_to_bin(line);
		}

		line = inp_get_string(in);
	}
	ewe("Token %s not found in file %s", token, in->full_name);
	exit(0);
	return -1;
}
