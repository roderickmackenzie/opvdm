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

struct inp_file {
	char *data;
	long fsize;
	char full_name[1000];
	int pos;
	int edited;
};

void inp_save(struct inp_file *in);
void inp_init(struct inp_file *in);
int inp_aes_load(struct inp_file *in, char *path, char *file, char *key);
void inp_load(struct inp_file *in, char *file);
void inp_free(struct inp_file *in);
void inp_search_double(struct inp_file *in, double *out, char *token);
void inp_search_int(struct inp_file *in, int *out, char *token);
void inp_search_string(struct inp_file *in, char *out, char *token);
char *inp_search(struct inp_file *in, char *token);
void inp_check(struct inp_file *in, double ver);
void inp_read_buffer(char **buf, long *len, char *full_file_name);
void inp_reset_read(struct inp_file *in);
char *inp_get_string(struct inp_file *in);
char *inp_search_part(struct inp_file *in, char *token);
void inp_load_from_path(struct inp_file *in, char *path, char *file);
void inp_replace(struct inp_file *in, char *token, char *text);
