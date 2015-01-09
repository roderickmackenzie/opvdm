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
#include "dump.h"
#include "slice.h"
#include "config.h"
#include <math.h>
#include "inp.h"

static int unused __attribute__ ((unused));

int config_search_token(char *file_name, char *token)
{
	FILE *in;
	char buf[400];
	in = fopen(file_name, "r");
	if (in == NULL) {
		ewe("file %s not found\n", file_name);
	}

	int l = 0;
	int found = FALSE;
	do {
		l++;
		unused = fscanf(in, "%s", buf);
		if ((!feof(in))) {
			if (strcmp(buf, token) == 0) {
				found = TRUE;
				l++;
				break;
			}
		}
	} while (!feof(in));

	fclose(in);
	if (found == FALSE) {
		ewe("Line not found in file %s %s\n", token, file_name);
		l = -1;
	}
	return l;
}

double config_get_token_val(char *file_name, char *token)
{
	FILE *in;
	char buf[400];
	in = fopen(file_name, "r");
	if (in == NULL) {
		ewe("file %s not found\n", file_name);
	}

	int l = 0;
	int found = FALSE;
	double ret = 0.0;
	do {
		l++;
		unused = fscanf(in, "%s", buf);
		if ((!feof(in))) {
			if (strcmp(buf, token) == 0) {
				found = TRUE;
				unused = fscanf(in, "%le", &ret);
				break;
			}
		}
	} while (!feof(in));

	fclose(in);
	if (found == FALSE) {
		ewe("Line not found in file %s %s\n", token, file_name);
		l = -1;
	}
	return ret;
}

void config_read_line_to_double(double *data, FILE * in, char *id)
{
	char name[100];
	unused = fscanf(in, "%s", name);
	unused = fscanf(in, "%le", data);
	if (strcmp(name, id) != 0) {
		ewe("Error read %s but expected %s\n", name, id);
	}
}

void config_read_line_string_decode_to_int(int *data, FILE * in, char *id)
{
	char name[100];
	unused = fscanf(in, "%s", name);
	if (strcmp(name, id) != 0) {
		ewe("Error read %s but expected %s\n", name, id);
	}
	unused = fscanf(in, "%s", name);
	*data = english_to_bin(name);
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

void load_config(char *simfile, struct device *in)
{
	double ver;
	int i;
	int dump;
	FILE *config;
	char temp[100];
	char name[1000];
	slice_init(in);

	char device_epitaxy[100];

	inp_load(in->outputpath, "sim.inp");
	inp_check(1.2);
	inp_search_string(name, "#simmode");
	in->simmode = english_to_bin(name);

	inp_search_int(&(in->stoppoint), "#stoppoint");
	inp_search_string(device_epitaxy, "#epitaxy");

	in->srh_sim = TRUE;
	in->ntrapnewton = TRUE;
	in->ptrapnewton = TRUE;

	inp_free();

	config = fopena(in->outputpath, device_epitaxy, "r");
	if (config == NULL) {
		ewe("epitaxy file not found\n");
	}

	config_read_line_to_int(&(in->mat.number), config, "#layers");
	get_max_layers(in->mat.number);

	in->mat_layers = in->mat.number;
	in->mat.l = malloc(in->mat.number * sizeof(struct layer));

	for (i = 0; i < in->mat.number; i++) {
		unused =
		    fscanf(config, "%s %le", in->mat.l[i].name,
			   &(in->mat.l[i].height));

	}

	config_read_line_to_int(&(in->ymeshlayers), config, "#mesh");

	in->meshdata = malloc(in->ymeshlayers * sizeof(struct mesh));

	in->ymeshpoints = 0;

	for (i = 0; i < in->ymeshlayers; i++) {
		unused =
		    fscanf(config, "%lf %lf", &(in->meshdata[i].len),
			   &(in->meshdata[i].number));
		in->meshdata[i].den =
		    in->meshdata[i].len / in->meshdata[i].number;
		in->ymeshpoints += in->meshdata[i].number;
	}

	in->imat = malloc(in->ymeshpoints * sizeof(int));

	in->ymesh = malloc(in->ymeshpoints * sizeof(double));

	int pos = 0;
	int ii;
	double dpos = 0.0;
	for (i = 0; i < in->ymeshlayers; i++) {

		for (ii = 0; ii < in->meshdata[i].number; ii++) {
			dpos += in->meshdata[i].den / 2.0;
			in->ymesh[pos] = dpos;
			in->imat[pos] = i;
			dpos += in->meshdata[i].den / 2.0;
			pos++;
		}
	}

	config_read_line_to_double(&(ver), config, "#ver");
	if (ver != 1.0) {
		ewe("File compatability problem %s\n", "epitaxy file");
	}

	unused = fscanf(config, "%s", name);
	if (strcmp("#end", name) != 0) {
		ewe("Problem with epitaxy file last item read %s!\n", name);
	}

	fclose(config);

	inp_load(in->outputpath, "device.inp");
	inp_check(1.14);
	inp_search_string(temp, "#lr_bias");
	in->lr_bias = english_to_bin(temp);

	inp_search_string(temp, "#lr_pcontact");
	in->lr_pcontact = english_to_bin(temp);

	inp_search_string(temp, "#invert_applied_bias");
	in->invert_applied_bias = english_to_bin(temp);

	inp_search_double(&(in->xlen), "#xlen");
	inp_search_double(&(in->zlen), "#zlen");
	in->area = in->xlen * in->zlen;

	inp_search_double(&(in->Rshunt), "#Rshunt");
	in->Rshunt = fabs(in->Rshunt);

	inp_search_double(&(in->Rcontact), "#Rcontact");
	in->Rcontact = fabs(in->Rcontact);

	inp_search_double(&(in->Rshort), "#Rshort");
	in->Rshort = fabs(in->Rshort);

	inp_search_int(&(in->Dphoton), "#Dphoton");

	inp_search_double(&(in->lcharge), "#lcharge");
	in->lcharge = fabs(in->lcharge);

	inp_search_double(&(in->rcharge), "#rcharge");
	in->rcharge = fabs(in->rcharge);

	inp_search_double(&(in->other_layers), "#otherlayers");

	inp_search_double(&(in->B), "#free_to_free_recombination");
	in->B = fabs(in->B);

	inp_search_int(&(in->interfaceleft), "#interfaceleft");
	inp_search_int(&(in->interfaceright), "#interfaceright");
	inp_search_double(&(in->phibleft), "#phibleft");
	inp_search_double(&(in->phibright), "#phibright");

	inp_search_double(&(in->vl_e), "#vl_e");
	in->vl_e = fabs(in->vl_e);

	inp_search_double(&(in->vl_h), "#vl_h");
	in->vl_h = fabs(in->vl_h);

	inp_search_double(&(in->vr_e), "#vr_e");
	in->vr_e = fabs(in->vr_e);

	inp_search_double(&(in->vr_h), "#vr_h");
	in->vr_h = fabs(in->vr_h);
	inp_free();

	inp_load(in->outputpath, "math.inp");
	inp_check(1.4);
	inp_search_int(&(in->max_electrical_itt0), "#maxelectricalitt_first");
	inp_search_double(&(in->electrical_clamp0), "#electricalclamp_first");
	inp_search_int(&(in->max_electrical_itt), "#maxelectricalitt");
	inp_search_double(&(in->electrical_clamp), "#electricalclamp");
	inp_search_double(&(in->posclamp), "#posclamp");
	inp_search_double(&(in->min_cur_error), "#electricalerror");
	inp_search_int(&(in->newton_clever_exit), "#newton_clever_exit");
	inp_search_int(&(in->newton_min_itt), "#newton_min_itt");
	inp_search_int(&(in->remesh), "#remesh");
	inp_search_int(&(in->newmeshsize), "#newmeshsize");
	inp_free();

	config = fopena(in->outputpath, "thermal.inp", "r");
	if (config == NULL) {
		ewe("thermal.inp not found\n");
	}

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->newton_enable_external_thermal));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->nofluxl));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->Tll));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->Tlr));

	unused = fscanf(config, "%s", name);
	if (strcmp("#end", name) != 0) {
		ewe("Problem with thermal.inp file last item read %s!\n", name);
	}
	fclose(config);

	inp_load(in->outputpath, "dump.inp");
	inp_check(1.24);

	inp_search_int(&(dump), "#plot");
	set_dump_status(dump_plot, dump);

	inp_search_int(&(dump), "#newton_dump");
	set_dump_status(dump_newton, dump);

	inp_search_int(&(in->plottime), "#plottime");

	inp_search_int(&(in->stop_start), "#startstop");

	inp_search_int(&(in->dumpitdos), "#dumpitdos");

	inp_search_string(in->plot_file, "#plotfile");

	inp_search_double(&(in->start_stop_time), "#start_stop_time");

	inp_search_int(&(dump), "#dump_iodump");
	set_dump_status(dump_iodump, dump);

	inp_search_int(&(in->dump_movie), "#dump_movie");

	inp_search_int(&(dump), "#dump_optics");
	set_dump_status(dump_optics, dump);

	inp_search_int(&(dump), "#dump_optics_verbose");
	set_dump_status(dump_optics_verbose, dump);

	inp_search_int(&(dump), "#dump_slices_by_time");
	set_dump_status(dump_slices_by_time, dump);

	inp_search_int(&(dump), "#dump_all_slices");
	set_dump_status(dump_all_slices, dump);

	inp_search_int(&(dump), "#dump_energy_slice_switch");
	set_dump_status(dump_energy_slice_switch, dump);

	inp_search_int(&(in->dump_slicepos), "#dump_energy_slice_pos");
	if (in->dump_slicepos >= in->ymeshpoints)
		in->dump_slicepos = 0;

	inp_search_int(&(dump), "#dump_print_newtonerror");
	set_dump_status(dump_print_newtonerror, dump);

	inp_search_int(&(dump), "#dump_print_converge");
	set_dump_status(dump_print_converge, dump);

	inp_search_int(&(dump), "#dump_print_pos_error");
	set_dump_status(dump_print_pos_error, dump);

	inp_search_int(&(dump), "#dump_pl");
	set_dump_status(dump_pl, dump);

	inp_free();

	if (get_dump_status(dump_iodump) == FALSE) {
		in->dumpitdos = FALSE;
		set_dump_status(dump_optics, FALSE);
		set_dump_status(dump_slices_by_time, FALSE);
		set_dump_status(dump_all_slices, FALSE);
	}

	in->ylen = 0.0;
	double mesh_len = 0.0;
	for (i = 0; i < in->mat.number; i++) {
		in->ylen += in->mat.l[i].height;
		mesh_len += in->meshdata[i].len;
	}
	if (in->ylen != mesh_len) {
		printf("Mesh and device length do not match\n");
	}
}
