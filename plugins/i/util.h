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
//    the Free Software Foundation; version 2 of the License.
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

void set_ewe_lock_file(char *lockname, char *data);
void print_hex(unsigned char *data);
void remove_dir(char *dir_name);
int ewe(const char *format, ...);
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

void edit_file_int(char *in_name, char *front, int line, int value);
void edit_file(char *in_name, char *front, int line, double value);
void edit_file_by_var(char *in_name, char *token, char *newtext);
void copy_file(char *output, char *input);
int get_file_len(char *file_name);
int cmpstr_min(char *in1, char *in2);
int english_to_bin(char *in);
void write_x_y_to_file(char *name, double *x, double *y, int len);
void write_x_y_z_to_file(char *name, double *x, double *y, double *z, int len);
void join_path(int max, ...);
char *get_dir_name_from_path(char *in);
char *get_file_name_from_path(char *in);
void mass_copy_file(char **output, char *input, int n);
void string_to_hex(char *out, char *in);
int strextract_name(char *out, char *in);
int strextract_int(char *in);
char *strextract_domain(char *in);
int find_config_file(char *ret, char *dir_name, char *search_name,
		     char *start_of_name);
void fx_with_units(char *out, double number);
#endif
