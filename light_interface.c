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
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>

#include "inp.h"
#include "light_interface.h"
#include "true_false.h"
#include "device.h"
#include "dump_ctrl.h"
#include "config.h"

static int unused __attribute__ ((unused));

void light_init(struct light *in, struct device *cell)
{
	char *error;
	char mode[20];
	char lib_path[200];
	double ver;
	double temp;
	inp_load(cell->outputpath, "light.inp");
	inp_check(1.21);

	inp_search_double(&(temp), "#Psun");
	cell->Psun = fabs(temp);

	inp_search_string(mode, "#light_model");

	inp_search_double(&(in->electron_eff), "#electron_eff");

	inp_search_double(&(in->hole_eff), "#hole_eff");

	inp_search_double(&(in->Dphotoneff), "#Dphotoneff");

	inp_search_double(&(in->ND), "#NDfilter");

	inp_search_double(&(temp), "#high_sun_scale");

	if (cell->Psun > 5000.0)
		cell->Psun *= fabs(temp);

	inp_search_double(&(ver), "#ver");

	inp_free();

	sprintf(lib_path, "./light/%s.so", mode);

	if (access(lib_path, F_OK) == -1) {
		sprintf(lib_path, "/usr/lib64/opvdm/%s.so", mode);
	}

	in->lib_handle = dlopen(lib_path, RTLD_LAZY);
	if (!in->lib_handle) {
		fprintf(stderr, "%s\n", dlerror());
		exit(0);
	}

	in->fn_init = dlsym(in->lib_handle, "light_init");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	in->fn_load_config = dlsym(in->lib_handle, "light_load_config");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	in->fn_solve_and_update =
	    dlsym(in->lib_handle, "light_solve_and_update");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	in->fn_free = dlsym(in->lib_handle, "light_free");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	in->fn_cal_photon_density =
	    dlsym(in->lib_handle, "light_cal_photon_density");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	in->fn_solve_lam_slice = dlsym(in->lib_handle, "light_solve_lam_slice");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	(*in->fn_init) ();
}

void light_solve_and_update(struct device *cell, struct light *in,
			    double Psun_in, double laser_eff_in)
{
	(*in->fn_solve_and_update) (cell, in, Psun_in, laser_eff_in);
}

void light_load_config(struct light *in, char *output_path)
{
	printf("Loading config\n");
	(*in->fn_load_config) (in, output_path);
}

double light_cal_photon_density(struct light *in)
{
	return (*in->fn_cal_photon_density) (in);
}

int light_solve_lam_slice(struct light *in, int lam)
{
	return (*in->fn_solve_lam_slice) (in, lam);
}

void light_free(struct light *in)
{
	dlclose(in->lib_handle);
}
