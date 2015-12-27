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
//    the Free Software Foundation; version 2 of the License
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
#include "dump.h"
#include "config.h"
#include <math.h>
#include "inp.h"
#include "util.h"
#include "hard_limit.h"
#include "epitaxy.h"
#include "mesh.h"

static int unused __attribute__ ((unused));

void config_read_line_to_double(double *data, FILE * in, char *id)
{
	char name[100];
	unused = fscanf(in, "%s", name);
	unused = fscanf(in, "%le", data);
	if (strcmp(name, id) != 0) {
		ewe("Error read %s but expected %s\n", name, id);
	}
}

void config_read_line_to_int(int *data, FILE * in, char *id)
{
	char name[100];
	unused = fscanf(in, "%s", name);
	unused = fscanf(in, "%d", data);
	if (strcmp(name, id) != 0) {
		ewe("Error read %s but expected %s\n", name, id);
	}
}

void config_read_line_to_string(char *data, FILE * in, char *id)
{
	char name[100];
	unused = fscanf(in, "%s", name);
	unused = fscanf(in, "%s", data);
	if (strcmp(name, id) != 0) {
		ewe("Error read %s but expected %s\n", name, id);
	}
}

void load_config(struct device *in)
{
	int i;
	char temp[100];

	char device_epitaxy[100];

	struct inp_file inp;
	inp_init(&inp);
	inp_load_from_path(&inp, in->inputpath, "sim.inp");
	inp_check(&inp, 1.2);

	inp_search_string(&inp, in->simmode, "#simmode");

	inp_search_int(&inp, &(in->stoppoint), "#stoppoint");
	inp_search_string(&inp, device_epitaxy, "#epitaxy");

	in->srh_sim = TRUE;
	in->ntrapnewton = TRUE;
	in->ptrapnewton = TRUE;

	inp_free(&inp);

/////////////////////////////////////////

	char device_file_path[1000];

	join_path(2, device_file_path, in->inputpath, "epitaxy.inp");

	epitaxy_load(&(in->my_epitaxy), device_file_path);

	mesh_load(in);

///////////////////////////////

	in->ylen = 0.0;
	double mesh_len = 0.0;

	in->ylen = epitaxy_get_electrical_length(&(in->my_epitaxy));

	for (i = 0; i < in->ymeshlayers; i++) {
		mesh_len += in->meshdata[i].len;
	}

	if (fabs(in->ylen - mesh_len) > 1e-14) {
		mesh_remesh(in);
		printf
		    ("Warning: Length of epitaxy and computational mesh did not match, so I remesshed the device.\n");
	}

	inp_init(&inp);
	inp_load_from_path(&inp, in->inputpath, "device.inp");
	inp_check(&inp, 1.19);
	inp_search_string(&inp, temp, "#lr_bias");
	in->lr_bias = english_to_bin(temp);

	inp_search_string(&inp, temp, "#lr_pcontact");
	in->lr_pcontact = english_to_bin(temp);

	inp_search_string(&inp, temp, "#invert_applied_bias");
	in->invert_applied_bias = english_to_bin(temp);

	inp_search_double(&inp, &(in->xlen), "#xlen");
	inp_search_double(&inp, &(in->zlen), "#zlen");
	in->area = in->xlen * in->zlen;

	inp_search_double(&inp, &(in->Rshunt), "#Rshunt");
	in->Rshunt = fabs(in->Rshunt);

	inp_search_double(&inp, &(in->Rcontact), "#Rcontact");
	in->Rcontact = fabs(in->Rcontact);

	inp_search_double(&inp, &(in->Rshort), "#Rshort");
	in->Rshort = fabs(in->Rshort);

	inp_search_double(&inp, &(in->lcharge), "#lcharge");
	in->lcharge = fabs(in->lcharge);
//if (in->lcharge<1e4) in->lcharge=1e4;

	inp_search_double(&inp, &(in->rcharge), "#rcharge");
	in->rcharge = fabs(in->rcharge);
//if (in->rcharge<1e4) in->rcharge=1e4;

	inp_search_double(&inp, &(in->other_layers), "#otherlayers");

	inp_search_int(&inp, &(in->interfaceleft), "#interfaceleft");
	inp_search_int(&inp, &(in->interfaceright), "#interfaceright");
	inp_search_double(&inp, &(in->phibleft), "#phibleft");
	inp_search_double(&inp, &(in->phibright), "#phibright");

	inp_search_double(&inp, &(in->vl_e), "#vl_e");
	in->vl_e = fabs(in->vl_e);

	inp_search_double(&inp, &(in->vl_h), "#vl_h");
	in->vl_h = fabs(in->vl_h);

	inp_search_double(&inp, &(in->vr_e), "#vr_e");
	in->vr_e = fabs(in->vr_e);

	inp_search_double(&inp, &(in->vr_h), "#vr_h");
	in->vr_h = fabs(in->vr_h);

	inp_free(&inp);

	inp_init(&inp);
	inp_load_from_path(&inp, in->inputpath, "math.inp");
	inp_check(&inp, 1.44);
	inp_search_int(&inp, &(in->max_electrical_itt0),
		       "#maxelectricalitt_first");
	inp_search_double(&inp, &(in->electrical_clamp0),
			  "#electricalclamp_first");
	inp_search_double(&inp, &(in->electrical_error0),
			  "#math_electrical_error_first");
	inp_search_int(&inp, &(in->math_enable_pos_solver),
		       "#math_enable_pos_solver");
	inp_search_int(&inp, &(in->max_electrical_itt), "#maxelectricalitt");
	inp_search_double(&inp, &(in->electrical_clamp), "#electricalclamp");
	inp_search_double(&inp, &(in->posclamp), "#posclamp");
	inp_search_double(&inp, &(in->min_cur_error), "#electricalerror");
	inp_search_int(&inp, &(in->newton_clever_exit), "#newton_clever_exit");
	inp_search_int(&inp, &(in->newton_min_itt), "#newton_min_itt");
	inp_search_int(&inp, &(in->remesh), "#remesh");
	inp_search_int(&inp, &(in->newmeshsize), "#newmeshsize");
	inp_search_int(&inp, &(in->pos_max_ittr), "#pos_max_ittr");
	inp_search_int(&inp, &(in->config_kl_in_newton), "#kl_in_newton");
	inp_free(&inp);

	inp_init(&inp);
	inp_load_from_path(&inp, in->inputpath, "thermal.inp");
	inp_check(&inp, 1.0);
	inp_search_int(&inp, &(in->newton_enable_external_thermal), "#thermal");
	inp_search_int(&inp, &(in->nofluxl), "#nofluxl");
	inp_search_double(&inp, &(in->Tll), "#Tll");
	inp_search_double(&inp, &(in->Tlr), "#Tlr");
	inp_free(&inp);

}
