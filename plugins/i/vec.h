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
#ifndef vech
#define vech

struct vec {
	double x;
	double y;
	double z;
};

void set_vec(struct vec *my_vec, double x, double y, double z);
void add_to_vec(struct vec *my_vec, double x, double y, double z);
void cpy_vec(struct vec *my_vec1, struct vec *my_vec2);
void add_vec(struct vec *my_vec1, struct vec *my_vec2);
int cmp_vec(struct vec *my_vec1, struct vec *my_vec2);
void plus_vec(struct vec *my_vec1);
void sub_vec(struct vec *my_vec1, struct vec *my_vec2);
void div_vec(struct vec *my_vec1, double n);
double mod_vec(struct vec *my_vec);
void norm_vec(struct vec *my_vec1);
double dot_vec(struct vec *a, struct vec *b);
void cros_vec(struct vec *ret, struct vec *a, struct vec *b);
void print_vec(struct vec *my_vec);
void fprint_vec(FILE * out, struct vec *my_vec);
void rot_vec(struct vec *in, struct vec *unit, struct vec *base, double ang);
double ang_vec(struct vec *one, struct vec *two);
void mul_vec(struct vec *my_vec1, double n);
double vec_get_dihedral(struct vec *a, struct vec *b, struct vec *c,
			struct vec *d);
#endif
