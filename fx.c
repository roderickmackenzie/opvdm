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
#include <math.h>
#include "sim.h"
#include "inp.h"
#include "fx.h"

static int unused __attribute__ ((unused));

#ifdef enable_fx
static double *fx_mesh;
static int mesh_len = 0;
static int mesh_pos = 0;
#endif

void fx_mesh_save()
{
#ifdef enable_fx
	int i;
	FILE *out;
	out = fopen("fxmesh_save.dat", "w");
	if (out == NULL) {
		ewe("can not save fx mesh file\n");
	}

	fprintf(out, "%d\n", mesh_len);

	for (i = 0; i < mesh_len; i++) {
		fprintf(out, "%le\n", fx_mesh[i]);
	}
	fprintf(out, "#ver\n");
	fprintf(out, "1.0\n");
	fprintf(out, "#end\n");

	fclose(out);
#endif
}

void fx_load_mesh(struct device *in, int number)
{
#ifdef enable_fx
	int i;
	struct inp_file inp;
	char mesh_file[200];
	int segments = 0;
	double end_fx = 0.0;
	double fx_start = 0.0;
	double read_len = 0.0;
	double dfx = 0.0;
	double mul = 0.0;
	int buffer_len = 2000;
	double fx = 0.0;
	int ii = 0;

	fx_mesh = (double *)malloc(buffer_len * sizeof(double));

	sprintf(mesh_file, "fxmesh%d.inp", number);

	inp_init(&inp);
	inp_load_from_path(&inp, in->inputpath, mesh_file);
	inp_check(&inp, 1.0);

	inp_reset_read(&inp);

	inp_get_string(&inp);
	sscanf(inp_get_string(&inp), "%le", &fx_start);
	fx = fx_start;

	inp_get_string(&inp);
	sscanf(inp_get_string(&inp), "%d", &segments);

	for (i = 0; i < segments; i++) {
		inp_get_string(&inp);
		sscanf(inp_get_string(&inp), "%le", &read_len);

		inp_get_string(&inp);
		sscanf(inp_get_string(&inp), "%le", &dfx);

		inp_get_string(&inp);
		sscanf(inp_get_string(&inp), "%le", &mul);

		if ((dfx != 0.0) && (mul != 0.0)) {
			end_fx = fx + read_len;

			while (fx < end_fx) {
				fx_mesh[ii] = fx;
				fx = fx + dfx;
				ii++;
				dfx = dfx * mul;
			}
		}
	}

	mesh_len = ii;
	mesh_pos = 0;
	inp_free(&inp);

#endif
}

void fx_step()
{
#ifdef enable_fx
	mesh_pos++;
#endif
}

int fx_points()
{
#ifdef enable_fx
	return mesh_len;
#else
	return 0;
#endif
}

int fx_run()
{
#ifdef enable_fx
	if (mesh_pos < (mesh_len - 1)) {
		return TRUE;
	} else {
		return FALSE;
	}
#else
	return 0;
#endif
}

double fx_get_fx()
{
#ifdef enable_fx
	return fx_mesh[mesh_pos];
#else
	return 0.0;
#endif
}

void fx_memory_free()
{
#ifdef enable_fx
	free(fx_mesh);
#endif
}
