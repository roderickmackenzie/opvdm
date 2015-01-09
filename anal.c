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
#include "sim.h"
#include <math.h>

void process_ce_data(int col, char *input, char *output)
{
	int i;
	struct istruct data;

	double d =
	    read_value("device.inp", 0, 12) + read_value("device.inp", 0, 39);
	double area =
	    read_value("device.inp", 0, 21) * read_value("device.inp", 0, 23);

	double cap = read_value("blom_bulk.inp", 0, 84) * epsilon0 * area / d;
	double capq = 0.0;

	double dt = 0.0;

	int x = 0;

	FILE *out = fopen(output, "w");

	for (x = 0; x < col; x++) {
		printf("loading.... %d\n", x);
		inter_load_a(&data, input, x, col);

		double tot = 0.0;

		for (i = 1; i < data.len - 1; i++) {

			{
				dt = (data.x[i + 1] - data.x[i - 1]) / 2.0;
				tot += data.data[i] * dt;
			}
		}

		capq = (data.x[0] * cap) / q;
		capq /= area;
		capq /= d;

		tot /= area;
		tot /= d;
		tot /= q;

		fprintf(out, "%e %e\n", data.data[0], tot - capq);
		inter_free(&data);
	}

	fclose(out);

}
