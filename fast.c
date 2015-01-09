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

static int fast_var;

int hashget(double *x, int N, double find)
{
	static double *x_ = NULL;
	static double find_ = 0.0;
	static int steps_ = 0.0;
	if (N == 1)
		return 0;
	if ((x_ == x) && (find_ == find))
		return steps_;
	double x0 = x[0];
	double x1 = x[1];
	double delta = find - x0;
	double step = x1 - x0;
	int steps = delta / step;

	if (steps > (N - 2))
		steps = N - 2;
	if (steps < 0)
		steps = 0;

	x_ = x;
	find_ = find;
	steps_ = steps;
	return steps;
}

void set_io_dump(int in)
{
	fast_var = in;
}

int get_io_dump()
{
	return fast_var;
}
