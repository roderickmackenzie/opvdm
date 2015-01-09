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
#include <math.h>
#include "advmath.h"

inline double dB(double x)
{
	double ret;
	if (fabs(x) > 1e-10) {
		ret = (exp(x) - 1.0 - x * exp(x)) / (exp(x) - 1) / (exp(x) - 1);
	} else {
		ret =
		    -1.0 / 2.0 + x / 6.0 - pow(x, 3.0) / 180.0 + pow(x,
								     5.0) /
		    5040;
	}

	return ret;
}

inline double B(double x)
{
	double ret;
	if (fabs(x) < 1e-16) {

		ret =
		    1 - x / 2.0 + pow(x, 2.0) / 12.0 - pow(x,
							   4.0) / 720.0 + pow(x,
									      6.0)
		    / 30240.0;
	} else {
		ret = x / (exp(x) - 1.0);
	}
	return ret;
}
