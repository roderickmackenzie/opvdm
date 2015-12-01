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
#ifndef buffer_h
#define buffer_h

struct buffer {
	char title[100];
	char type[100];
	double x_mul;
	double y_mul;
	char x_label[100];
	char y_label[100];
	char x_units[100];
	char y_units[100];
	char section_one[100];
	char section_two[100];
	int logscale_x;
	int logscale_y;
	int write_to_zip;
	int norm_x_axis;
	int norm_y_axis;
	double time;
	double Vexternal;
	char *buf;
	int len;
	int max_len;
	char zip_file_name[400];

};

void buffer_zip_set_name(struct buffer *in, char *name);
void buffer_init(struct buffer *in);
void buffer_malloc(struct buffer *in);
void buffer_add_xy_data(struct buffer *in, double *x, double *y, int len);
void buffer_add_string(struct buffer *in, char *string);
void buffer_add_info(struct buffer *in);
void buffer_dump(char *file, struct buffer *in);
void buffer_dump_path(char *path, char *file, struct buffer *in);
void buffer_free(struct buffer *in);
void buffer_dump_aes(char *path, char *file, struct buffer *in, char *key_text);
#endif
