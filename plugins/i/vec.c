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
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "vec.h"

double vec_get_dihedral(struct vec *a, struct vec *b, struct vec *c,
			struct vec *d)
{
	double cos_;
	struct vec b1;
	struct vec b2;
	struct vec b3;
	struct vec b23;
	struct vec b12;

	cpy_vec(&b1, b);
	sub_vec(&b1, a);

	cpy_vec(&b2, c);
	sub_vec(&b2, b);

	cpy_vec(&b3, d);
	sub_vec(&b3, c);

	cros_vec(&b23, &b2, &b3);
	cros_vec(&b12, &b1, &b2);

	double mb2 = mod_vec(&b2);
	double aa = dot_vec(&b1, &b23);

	double cross = 0.0;;
	cross = dot_vec(&b12, &b23);

	cos_ = (atan2(mb2 * aa, cross) / (3.1415026)) * 180.0;
	if (cos_ < 0)
		cos_ = 360 - sqrt(pow(cos_, 2.0));

	return cos_;
}

void rot_vec(struct vec *in, struct vec *unit, struct vec *base, double ang)
{
	struct vec temp;
	struct vec rot;

	double c;
	double s;

	cpy_vec(&rot, in);
	sub_vec(&rot, base);
	c = cos(ang);
	s = sin(ang);
	temp.x =
	    rot.x * (unit->x * unit->x * (1.0 - c) + c) +
	    rot.y * (unit->x * unit->y * (1.0 - c) - unit->z * s) +
	    rot.z * (unit->x * unit->z * (1.0 - c) + unit->y * s);
	temp.y =
	    rot.x * (unit->x * unit->y * (1 - c) + unit->z * s) +
	    rot.y * (unit->y * unit->y * (1 - c) + c) +
	    rot.z * (unit->y * unit->z * (1.0 - c) - unit->x * s);
	temp.z =
	    rot.x * (unit->x * unit->z * (1.0 - c) - unit->y * s) +
	    rot.y * (unit->y * unit->z * (1.0 - c) + unit->x * s) +
	    rot.z * (unit->z * unit->z * (1.0 - c) + c);

	add_vec(&temp, base);
	cpy_vec(in, &temp);
}

void set_vec(struct vec *my_vec, double x, double y, double z)
{
	my_vec->x = x;
	my_vec->y = y;
	my_vec->z = z;
}

void add_to_vec(struct vec *my_vec, double x, double y, double z)
{
	my_vec->x += x;
	my_vec->y += y;
	my_vec->z += z;
}

void cpy_vec(struct vec *my_vec1, struct vec *my_vec2)
{
	my_vec1->x = my_vec2->x;
	my_vec1->y = my_vec2->y;
	my_vec1->z = my_vec2->z;
}

int cmp_vec(struct vec *my_vec1, struct vec *my_vec2)
{
	struct vec temp;
	cpy_vec(&temp, my_vec1);
	sub_vec(&temp, my_vec2);

	double test = mod_vec(&temp);
	if (test < 1e-6)
		return 0;
	return 1;
}

void add_vec(struct vec *my_vec1, struct vec *my_vec2)
{
	my_vec1->x += my_vec2->x;
	my_vec1->y += my_vec2->y;
	my_vec1->z += my_vec2->z;
}

void sub_vec(struct vec *my_vec1, struct vec *my_vec2)
{
	my_vec1->x -= my_vec2->x;
	my_vec1->y -= my_vec2->y;
	my_vec1->z -= my_vec2->z;
}

void div_vec(struct vec *my_vec1, double n)
{
	my_vec1->x /= n;
	my_vec1->y /= n;
	my_vec1->z /= n;
}

double ang_vec(struct vec *one, struct vec *two)
{
	double dot;
	double cos_;

	dot = dot_vec(one, two);

	cos_ =
	    (acos(dot / (mod_vec(one) * mod_vec(two))) / (3.1415026)) * 180.0;

	return cos_;
}

void mul_vec(struct vec *my_vec1, double n)
{
	my_vec1->x *= n;
	my_vec1->y *= n;
	my_vec1->z *= n;
}

double mod_vec(struct vec *my_vec)
{
	return sqrt(pow(my_vec->x, 2.0) + pow(my_vec->y, 2.0) +
		    pow(my_vec->z, 2.0));
}

void norm_vec(struct vec *my_vec1)
{
	double mod = mod_vec(my_vec1);
	my_vec1->x /= mod;
	my_vec1->y /= mod;
	my_vec1->z /= mod;
}

void plus_vec(struct vec *my_vec1)
{
	if (my_vec1->x < 0)
		my_vec1->x *= -1.0;
	if (my_vec1->y < 0)
		my_vec1->y *= -1.0;
	if (my_vec1->z < 0)
		my_vec1->z *= -1.0;
}

double dot_vec(struct vec *a, struct vec *b)
{
	return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

struct vec null_vec;

void cros_vec(struct vec *ret, struct vec *a, struct vec *b)
{
	ret->x = (a->y * b->z) - (a->z * b->y);

	ret->y = (a->z * b->x) - (a->x * b->z);

	ret->z = (a->x * b->y) - (a->y * b->x);

}

void print_vec(struct vec *my_vec)
{
	printf("%lf %lf %lf\n", my_vec->x, my_vec->y, my_vec->z);
}

void fprint_vec(FILE * out, struct vec *my_vec)
{
	fprintf(out, "%lf %lf %lf\n", my_vec->x, my_vec->y, my_vec->z);
}
