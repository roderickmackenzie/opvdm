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
#include <math.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include "true_false.h"
#include "../../inp.h"

struct inp_file hard_limit_inp;

void hard_limit_init()
{
	inp_init(&hard_limit_inp);
	inp_load(&hard_limit_inp, "hard_limit.inp");
}

void hard_limit_free()
{
	inp_free(&hard_limit_inp);
}

void hard_limit(char *token, double *value)
{
	char token0[1000];
	double ret = *value;
	double min = 0.0;
	double max = 0.0;
	char *text = inp_search_part(&hard_limit_inp, token);

	if (text != NULL) {
		sscanf(text, "%s %lf %lf", token0, &max, &min);

		if (strcmp(token0, token) == 0) {
			if (ret > max) {
				ret = max;
			}

			if (ret < min) {
				ret = min;
			}
		}
	}
	*value = ret;
}
