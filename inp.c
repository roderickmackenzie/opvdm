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
#include "inp.h"
#include "util.h"
#include "code_ctrl.h"

static char *data;
static long fsize;
static char full_name[1000];
static int pos = 0;

void reset_read()
{
	pos = 0;
}

char *get_string()
{
	int i;
	static char ret[100];
	int ii = 0;
	if (pos >= fsize) {
		return 0;
	}

	for (i = pos; i < fsize; i++) {
		if ((data[i] == '\n') || (data[i] == 0)) {
			ret[ii] = 0;
			pos++;
			break;
		}

		ret[ii] = data[i];
		ii++;
		pos++;

	}

	return ret;
}

void inp_read_buffer(char **buf, long *len, char *path, char *file)
{
	char full_file_name[1000];
	sprintf(full_file_name, "%s/%s", path, file);
	if (access(full_file_name, F_OK) != -1) {
		FILE *f = fopen(full_file_name, "rb");
		if (f == NULL) {
			ewe("%s not found\n", file);
		}
		fseek(f, 0, SEEK_END);
		*len = ftell(f);
		fseek(f, 0, SEEK_SET);

		*buf = malloc(((*len) + 2) * sizeof(char));
		fread(*buf, *len, 1, f);
		fclose(f);

		(*buf)[(int)*len] = '\n';
		(*buf)[(int)(*len + 1)] = 0;
	} else {
		sprintf(full_file_name, "%s/sim.opvdm", path);
		int err = 0;
		struct zip *z = zip_open(full_file_name, 0, &err);

		if (z != NULL) {

			struct zip_stat st;
			zip_stat_init(&st);
			int ret = zip_stat(z, file, 0, &st);

			if (ret == 0) {

				*len = st.size * sizeof(char);
				*buf = (char *)malloc(*len);

				struct zip_file *f = zip_fopen(z, file, 0);
				zip_fread(f, *buf, st.size);
				zip_fclose(f);

			}
			zip_close(z);
		} else {
			ewe("error in config file %s", full_file_name);
		}

	}
}

void inp_load(char *path, char *file)
{
	sprintf(full_name, "%s/%s", path, file);
	inp_read_buffer(&data, &fsize, path, file);
}

void inp_free()
{
	free(data);
}

void inp_search_double(double *out, char *token)
{
	sscanf(inp_search(token), "%le", out);
}

void inp_search_int(int *out, char *token)
{
	sscanf(inp_search(token), "%d", out);
}

void inp_search_string(char *out, char *token)
{
	strcpy(out, inp_search(token));
}

void inp_check(double ver)
{
	double read_ver = 0.0;
	reset_read();
	char *line = get_string();
	while (line) {
		if (strcmp(line, "#ver") == 0) {
			line = get_string();
			sscanf(line, "%le", &read_ver);
			if (ver != read_ver) {
				ewe("File compatability problem %s\n",
				    full_name);
			}
			line = get_string();

			if (strcmp(line, "#end") != 0) {
				ewe("#end token missing\n");
			}
			return;
		}

		line = get_string();
	}
	ewe("Token not found\n");
	return;
}

char *inp_search(char *token)
{
	reset_read();
	char *line = get_string();
	while (line) {

		if (strcmp(line, token) == 0) {
			line = get_string();
			return line;
		}

		line = get_string();
	}
	ewe("Token %s not found in file %s", token, full_name);
	exit(0);
	return 0;
}
