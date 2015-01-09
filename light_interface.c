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
#include <errno.h>
#include <util.h>
#include "light_interface.h"
#include "true_false.h"
#include "device.h"
#include "dump_ctrl.h"

static double last_Psun = -1000.0;
static double last_laser_eff = -1000.0;
static double last_wavelength_laser = -1000.0;

void light_init()
{
	last_Psun = -1000.0;
	last_laser_eff = -1000.0;
	last_wavelength_laser = -1000.0;
}

void light_transfer_gen_rate_to_device(struct device *cell, struct light *in)
{
	int i = 0;
	double Gn = 0.0;
	double Gp = 0.0;

	if (in->align_mesh == FALSE) {
		for (i = 0; i < cell->ymeshpoints; i++) {

			if (in->function == 0) {
				Gn = inter_get_raw(in->x, in->Gn, in->points,
						   in->device_start +
						   cell->ymesh[i]) *
				    cell->Dphotoneff;
				Gp = inter_get_raw(in->x, in->Gp, in->points,
						   in->device_start +
						   cell->ymesh[i]) *
				    cell->Dphotoneff;
			} else if (in->function == 1) {
				if ((in->laser_eff > 0.0) || (in->Psun > 0.0)) {
					Gn = cell->simplephotondensity;
					Gp = cell->simplephotondensity;

				}
			} else if (in->function == 2) {
				if ((in->laser_eff > 0.0) || (in->Psun > 0.0)) {
					Gn = exp(-cell->simple_alpha *
						 cell->ymesh[i]) *
					    cell->simplephotondensity;
					Gp = exp(-cell->simple_alpha *
						 cell->ymesh[i]) *
					    cell->simplephotondensity;
				}
			}

			cell->Gn[i] = Gn * in->electron_eff;
			cell->Gp[i] = Gp * in->hole_eff;
			cell->Habs[i] = 0.0;

		}
	} else {
		for (i = 0; i < cell->ymeshpoints; i++) {
			cell->Gn[i] =
			    in->Gn[in->device_start_i + i] * cell->Dphotoneff;
			cell->Gp[i] =
			    in->Gp[in->device_start_i + i] * cell->Dphotoneff;
		}

	}

}

double light_convert_density(struct device *in, double start, double width)
{
	double ratio = 0.0;
	return ratio;
}

void light_solve_and_update(struct device *cell, struct light *in,
			    double Psun_in, double laser_eff_in)
{
	in->Psun = Psun_in;
	in->laser_eff = laser_eff_in;

	if (in->function == 0) {
		if ((last_laser_eff != in->laser_eff) || (last_Psun != in->Psun)
		    || (last_wavelength_laser != in->laser_wavelength)) {

			light_solve_optical_problem(in);
			last_laser_eff = in->laser_eff;
			last_Psun = in->Psun;
			last_wavelength_laser = in->laser_wavelength;
		}
	}

	light_transfer_gen_rate_to_device(cell, in);
}
