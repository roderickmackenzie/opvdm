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

static int unused __attribute__ ((unused));

int config_search_token(char *file_name, char *token)
{
	FILE *in;
	char buf[400];
	in = fopen(file_name, "r");
	if (in == NULL) {
		printf("file %s not found\n", file_name);
		exit(0);
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
		printf("Line not found in file %s %s\n", token, file_name);
		l = -1;
		exit(0);
	}
	return l;
}

double config_get_token_val(char *file_name, char *token)
{
	FILE *in;
	char buf[400];
	in = fopen(file_name, "r");
	if (in == NULL) {
		printf("file %s not found\n", file_name);
		exit(0);
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
		printf("Line not found in file %s %s\n", token, file_name);
		l = -1;
		exit(0);
	}
	return ret;
}

void config_read_line_to_double(double *data, FILE * in, char *id)
{
	char name[100];
	unused = fscanf(in, "%s", name);
	unused = fscanf(in, "%le", data);
	if (strcmp(name, id) != 0) {
		printf("Error read %s but expected %s\n", name, id);
		exit(0);
	}
}

void config_read_line_string_decode_to_int(int *data, FILE * in, char *id)
{
	char name[100];
	unused = fscanf(in, "%s", name);
	if (strcmp(name, id) != 0) {
		printf("Error read %s but expected %s\n", name, id);
		exit(0);
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
		printf("Error read %s but expected %s\n", name, id);
		exit(0);
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
	config = fopena(in->outputpath, "device_epitaxy.inp", "r");
	if (config == NULL) {
		printf("device_epitaxy.inp not found\n");
		exit(0);
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

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->ymeshlayers));

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

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(ver));
	if (ver != 1.0) {
		printf("File compatability problem %s\n", "device_epitaxy.inp");
		exit(0);
	}

	unused = fscanf(config, "%s", name);
	if (strcmp("#end", name) != 0) {
		printf
		    ("Problem with device_epitaxy.inp file last item read %s!\n",
		     name);
		exit(0);
	}

	fclose(config);

	config = fopena(in->outputpath, simfile, "r");
	if (config == NULL) {
		printf("device.inp not found\n");
		exit(0);
	}

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%s", temp);
	in->lr_bias = english_to_bin(temp);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%s", temp);
	in->lr_pcontact = english_to_bin(temp);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%s", temp);
	in->invert_applied_bias = english_to_bin(temp);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%le", &(in->xlen));
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%le", &(in->zlen));

	in->area = in->xlen * in->zlen;

	config_read_line_to_double(&(in->Rshunt), config, "#Rshunt");
	in->Rshunt = fabs(in->Rshunt);

	config_read_line_to_double(&(in->Rcontact), config, "#Rcontact");
	in->Rcontact = fabs(in->Rcontact);

	config_read_line_to_double(&(in->Rshort), config, "#Rshort");

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->Dphoton));

	config_read_line_to_double(&(in->Dphotoneff), config, "#Dphotoneff");
	in->Dphotoneff = fabs(in->Dphotoneff);

	config_read_line_to_double(&(in->lcharge), config, "#lcharge");
	in->lcharge = fabs(in->lcharge);

	config_read_line_to_double(&(in->rcharge), config, "#rcharge");
	in->rcharge = fabs(in->rcharge);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%le", &(in->other_layers));
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%le", &(in->B));
	in->B = fabs(in->B);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->interfaceleft));
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->interfaceright));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->phibleft));
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->phibright));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->vl_e));
	in->vl_e = fabs(in->vl_e);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->vl_h));
	in->vl_h = fabs(in->vl_h);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->vr_e));
	in->vr_e = fabs(in->vr_e);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->vr_h));
	in->vr_h = fabs(in->vr_h);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(ver));
	if (ver != 1.12) {
		printf("File compatability problem %s\n", simfile);
		exit(0);
	}

	unused = fscanf(config, "%s", name);
	if (strcmp("#end", name) != 0) {
		printf("Problem with device.inp file last item read %s!\n",
		       name);
		exit(0);
	}
	fclose(config);

	config = fopena(in->outputpath, "math.inp", "r");
	if (config == NULL) {
		printf("math.inp not found\n");
		exit(0);
	}

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->max_electrical_itt0));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->electrical_clamp0));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->max_electrical_itt));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->electrical_clamp));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->posclamp));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%le", &(in->min_cur_error));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->newton_clever_exit));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->newton_min_itt));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->remesh));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->newmeshsize));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(ver));
	if (ver != 1.4) {
		printf("File compatability problem %s\n", "math.inp");
		exit(0);
	}

	unused = fscanf(config, "%s", name);
	if (strcmp("#end", name) != 0) {
		printf("Problem with math.inp file last item read %s!\n", name);
		exit(0);
	}
	fclose(config);

	config = fopena(in->outputpath, "thermal.inp", "r");
	if (config == NULL) {
		printf("thermal.inp not found\n");
		exit(0);
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
		printf("Problem with thermal.inp file last item read %s!\n",
		       name);
		exit(0);
	}
	fclose(config);

	config = fopena(in->outputpath, "sim.inp", "r");
	if (config == NULL) {
		printf("sim.inp not found\n");
		exit(0);
	}

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%s", name);
	in->simmode = english_to_bin(name);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->stoppoint));

	in->srh_sim = TRUE;
	in->ntrapnewton = TRUE;
	in->ptrapnewton = TRUE;

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(ver));
	if (ver != 1.1) {
		printf("File compatability problem %s\n", "sim.inp");
		exit(0);
	}

	unused = fscanf(config, "%s", name);
	if (strcmp("#end", name) != 0) {
		printf("Problem with sim.inp file last item read %s!\n", name);
		exit(0);
	}
	fclose(config);

	config = fopena(in->outputpath, "dump.inp", "r");
	if (config == NULL) {
		printf("dump.inp not found\n");
		exit(0);
	}

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%s", name);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(dump));
	unused = fscanf(config, "%d", &(dump));
	set_dump_status(dump_plot, dump);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(dump));
	set_dump_status(dump_newton, dump);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->plottime));
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->stop_start));
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->dumpitdos));
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->slave));
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%s", in->slave_path);
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->master));
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%s", in->plot_file);

	config_read_line_to_double(&(in->start_stop_time), config,
				   "#start_stop_time");

	config_read_line_to_int(&(dump), config, "#dump_iodump");
	set_dump_status(dump_iodump, dump);

	config_read_line_to_int(&(in->dump_movie), config, "#dump_movie");

	config_read_line_to_int(&(dump), config, "#dump_optics");
	set_dump_status(dump_optics, dump);

	config_read_line_to_int(&(dump), config, "#dump_slices");
	set_dump_status(dump_slices, dump);

	config_read_line_to_int(&(dump), config, "#dump_energy_slice_switch");
	set_dump_status(dump_energy_slice_switch, dump);

	config_read_line_to_int(&(in->dump_slicepos), config,
				"#dump_energy_slice_pos");
	if (in->dump_slicepos >= in->ymeshpoints)
		in->dump_slicepos = 0;

	config_read_line_to_int(&(dump), config, "#dump_print_newtonerror");
	set_dump_status(dump_print_newtonerror, dump);

	config_read_line_to_int(&(dump), config, "#dump_print_converge");
	set_dump_status(dump_print_converge, dump);

	config_read_line_to_int(&(dump), config, "#dump_print_pos_error");
	set_dump_status(dump_print_pos_error, dump);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(ver));

	if (ver != 1.22) {
		printf("File compatability problem %s\n", "dump.inp");
		exit(0);
	}

	unused = fscanf(config, "%s", name);
	if (strcmp("#end", name) != 0) {
		printf("Problem with dump.inp file last item read %s!\n", name);
		exit(0);
	}
	fclose(config);

	if (get_dump_status(dump_iodump) == FALSE) {
		in->dumpitdos = FALSE;
		set_dump_status(dump_optics, FALSE);
		set_dump_status(dump_slices, FALSE);
	}

	config = fopena(in->outputpath, "light.inp", "r");
	if (config == NULL) {
		printf("light.inp not found\n");
		exit(0);
	}

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%le", &(in->Psun));
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%d", &(in->laser));
	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(in->fgen));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%le", &in->simplephotondensity);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%le", &in->simple_alpha);

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(ver));
	if (ver != 1.1) {
		printf("File compatability problem %s\n", "light.inp");
		exit(0);
	}

	unused = fscanf(config, "%s", name);
	if (strcmp("#end", name) != 0) {
		printf("Problem with light.inp file last item read %s!\n",
		       name);
		exit(0);
	}
	fclose(config);

	in->ylen = 0.0;
	double mesh_len = 0.0;
	for (i = 0; i < in->mat.number; i++) {
		in->ylen += in->mat.l[i].height;
		mesh_len += in->meshdata[i].len;
	}
	if (in->ylen != mesh_len) {
		printf("Mesh and device length do not match\n");
		exit(1);
	}
}
