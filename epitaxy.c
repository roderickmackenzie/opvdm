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
#include "epitaxy.h"
#include "inp.h"
#include "util.h"
#include "true_false.h"

void epitaxy_load(struct epitaxy *in, char *file)
{
	int i;
	char dos_file[20];
	char pl_file[20];
	struct inp_file inp;
	in->electrical_layers = 0;

	inp_init(&inp);
	inp_load(&inp, file);
	inp_check(&inp, 1.1);
	inp_reset_read(&inp);
	inp_get_string(&inp);
	sscanf(inp_get_string(&inp), "%d", &(in->layers));

	if (in->layers > 20) {
		ewe("Too many material layers\n");
	}

	if (in->layers < 1) {
		ewe("No material layers\n");
	}

	for (i = 0; i < in->layers; i++) {
		inp_get_string(&inp);
		sscanf(inp_get_string(&inp), "%le", &(in->width[i]));
		in->width[i] = fabs(in->width[i]);
		strcpy(in->mat_file[i], inp_get_string(&inp));
		strcpy(dos_file, inp_get_string(&inp));
		strcpy(pl_file, inp_get_string(&inp));

		char temp[20];
		if (strcmp(dos_file, "none") != 0) {
			strcpy(temp, dos_file);
			strcat(temp, ".inp");
			in->electrical_layer[i] = TRUE;
			if (inp_isfile(temp) != 0) {
				ewe("dos file %s does not exist", temp);
			}
			strcpy(in->dos_file[in->electrical_layers], dos_file);

			strcpy(temp, pl_file);
			strcat(temp, ".inp");
			if (inp_isfile(temp) != 0) {
				ewe("pl file %s does not exist", temp);
			}
			strcpy(in->pl_file[in->electrical_layers], pl_file);

			in->electrical_layers++;
		} else {
			in->electrical_layer[i] = FALSE;
		}

	}

	char *ver = inp_get_string(&inp);
	if (strcmp(ver, "#ver") != 0) {
		ewe("No #ver tag found in file\n");
	}

	inp_free(&inp);
}

double epitaxy_get_electrical_length(struct epitaxy *in)
{
	int i = 0;
	double tot = 0.0;

	for (i = 0; i < in->layers; i++) {
		printf("rod>>>%d\n", in->electrical_layer[i]);
		if (in->electrical_layer[i] == TRUE) {
			tot += in->width[i];
		}
	}

	return tot;
}

double epitaxy_get_optical_length(struct epitaxy *in)
{
	int i = 0;
	double tot = 0.0;

	for (i = 0; i < in->layers; i++) {
		tot += in->width[i];
	}

	return tot;
}

int epitaxy_get_optical_material_layer(struct epitaxy *in, double pos)
{
	int i = 0;
	double layer_end = 0.0;
	for (i = 0; i < in->layers; i++) {
		layer_end += in->width[i];

		if (pos < layer_end) {
			return i;
		}

	}

	return -1;
}

int epitaxy_get_electrical_material_layer(struct epitaxy *in, double pos)
{
	int i = 0;
	double layer_end = 0.0;
	int electrical_layer = 0;

	for (i = 0; i < in->layers; i++) {
		if (in->electrical_layer[i] == TRUE) {
			layer_end += in->width[i];

			if (pos < layer_end) {
				return electrical_layer;
			}
			electrical_layer++;
		}

	}

	return -1;
}

double epitaxy_get_device_start(struct epitaxy *in)
{
	int i = 0;
	double pos = 0.0;
	for (i = 0; i < in->layers; i++) {

		if (in->electrical_layer[i] == TRUE) {
			return pos;
		}
		pos += in->width[i];

	}

	return -1;
}

double epitaxy_get_device_start_i(struct epitaxy *in)
{
	int i = 0;
	for (i = 0; i < in->layers; i++) {

		if (in->electrical_layer[i] == TRUE) {
			return i;
		}

	}

	return -1;
}
