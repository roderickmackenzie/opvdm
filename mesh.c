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
#include "device.h"
#include "mesh.h"
#include "inp.h"
#include "util.h"
#include "true_false.h"
#include "hard_limit.h"

void mesh_remesh(struct device *in)
{
	in->ymeshlayers = 1;
	in->meshdata[0].len = in->ylen;
	in->meshdata[0].number = 40;
	mesh_save(in);
	mesh_free(in);
	mesh_load(in);
}

void mesh_save(struct device *in)
{
	int i = 0;
	char buffer[2000];
	char temp[2000];
	char full_file_name[200];

	strcpy(buffer, "");
	strcat(buffer, "#mesh_layers\n");

	sprintf(temp, "%d\n", in->ymeshlayers);
	strcat(buffer, temp);

	for (i = 0; i < in->ymeshlayers; i++) {
		strcat(buffer, "#mesh_layer_length0\n");

		sprintf(temp, "%le\n", in->meshdata[i].len);
		strcat(buffer, temp);

		strcat(buffer, "#mesh_layer_points0\n");

		sprintf(temp, "%d\n", (int)(in->meshdata[i].number));
		strcat(buffer, temp);
	}

	strcat(buffer, "#ver\n");
	strcat(buffer, "1.0\n");
	strcat(buffer, "#end\n");

	strcpy(full_file_name, "mesh.inp");
	zip_write_buffer(full_file_name, buffer, strlen(buffer));

}

void mesh_free(struct device *in)
{
	in->ymeshpoints = 0;
	free(in->meshdata);
	free(in->imat);
}

void mesh_load(struct device *in)
{
	int i;
	struct inp_file inp;
	char token0[200];
	char token1[200];

	inp_init(&inp);
	inp_load_from_path(&inp, in->inputpath, "mesh.inp");

	inp_check(&inp, 1.0);

	inp_reset_read(&inp);

	inp_get_string(&inp);	//"#mesh_layers"

	sscanf(inp_get_string(&inp), "%d", &(in->ymeshlayers));

	//config_read_line_to_int(&(in->ymeshlayers),config,"#mesh_layers");

	in->meshdata = malloc(in->ymeshlayers * sizeof(struct mesh));

	in->ymeshpoints = 0;

	for (i = 0; i < in->ymeshlayers; i++) {
		sscanf(inp_get_string(&inp), "%s", token0);
		sscanf(inp_get_string(&inp), "%lf", &(in->meshdata[i].len));

		sscanf(inp_get_string(&inp), "%s", token1);
		sscanf(inp_get_string(&inp), "%lf", &(in->meshdata[i].number));

		in->meshdata[i].len = fabs(in->meshdata[i].len);
		hard_limit(token0, &(in->meshdata[i].len));
		in->meshdata[i].den =
		    in->meshdata[i].len / in->meshdata[i].number;
		in->ymeshpoints += in->meshdata[i].number;
	}

	inp_free(&inp);

	in->ymesh = malloc(in->ymeshpoints * sizeof(double));
	in->imat = malloc(in->ymeshpoints * sizeof(int));

	int pos = 0;
	int ii;
	double dpos = 0.0;
	for (i = 0; i < in->ymeshlayers; i++) {

		for (ii = 0; ii < in->meshdata[i].number; ii++) {
			dpos += in->meshdata[i].den / 2.0;
			in->ymesh[pos] = dpos;
			in->imat[pos] =
			    epitaxy_get_electrical_material_layer(&
								  (in->
								   my_epitaxy),
								  dpos);
			dpos += in->meshdata[i].den / 2.0;
			pos++;
		}
	}

}
