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
#ifndef h_util
#define h_util
#define fg_reset	0
#define fg_red		31
#define fg_green	32
#define fg_yellow	33
#define fg_blue		34
#define fg_purple	35
void remove_dir(char *path, char *dir_name);
int ewe(const char *format, ...);
void waveprint(char *in, double wavelength);
double read_value(char *file, int skip, int line);
int strcmp_end(char *str, char *end);
int extract_str_number(char *in, char *cut);
void randomprint(char *in);
void waveprint(char *in, double wavelength);
void textcolor(int color);
int scanarg(char *in[], int count, char *find);
int get_arg_plusone_pos(char *in[], int count, char *find);
char *get_arg_plusone(char *in[], int count, char *find);
FILE *fopena(char *path, char *name, const char *mode);
FILE *fopenaa(char *path, char *add0, char *name, const char *mode);
FILE *fopenaaa(char *path, char *add0, char *add1, char *name,
	       const char *mode);

void edit_file_int(char *in_name, char *front, int line, int value);
void edit_file(char *in_name, char *front, int line, double value);
void edit_file_by_var(char *in_name, char *token, char *newtext);
void copy_file(char *out_name, char *in_name, char *front, int line,
	       double value);
int get_file_len(char *file_name);
int cmpstr_min(char *in1, char *in2);
int english_to_bin(char *in);
void write_x_y_to_file(char *name, double *x, double *y, int len);
void write_x_y_z_to_file(char *name, double *x, double *y, double *z, int len);
#endif
