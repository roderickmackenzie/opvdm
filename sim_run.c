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
#include <signal.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <util.h>
#include <pwd.h>
#include <unistd.h>
#include "sim.h"
#include "dos.h"
#include "dump.h"
#include "complex_solver.h"
#include "elec_plugins.h"
#include "ntricks.h"

struct device cell;

int run_simulation(char *runpath)
{

	printf("Run_simulation\n");

	cell.newton_cur = FALSE;

	if (strcmp(runpath, "") != 0)
		strcpy(cell.outputpath, runpath);
	int i;

	cell.kTq = (300.0 * kb / q);
	printf("Load config\n");
	load_config("device.inp", &cell);

	printf("Loading DoS for %d layers\n", cell.mat_layers);
	char tempn[100];
	char tempp[100];
	i = 0;
	dos_init(i);
	printf("Load DoS %d/%d\n", i, cell.mat_layers);
	sprintf(tempn, "dosn%d.dat", i);
	sprintf(tempp, "dosp%d.dat", i);
	load_dos(&cell, tempn, tempp, i);
	device_init(&cell);

	if (get_dump_status(dump_print_converge) == TRUE) {
		cell.converge = fopena(cell.outputpath, "./converge.dat", "w");
		fclose(cell.converge);

		cell.tconverge =
		    fopena(cell.outputpath, "./tconverge.dat", "w");
		fclose(cell.tconverge);
	}

	int curlayer = 0;
	double end = cell.mat.l[0].height;

	for (i = 0; i < cell.ymeshpoints; i++) {
		if (cell.ymesh[i] >= end) {
			curlayer++;
			end += cell.mat.l[curlayer].height;
		}
		cell.imat[i] = curlayer;
		cell.Nad[i] = get_dos_doping(cell.imat[i]);

	}

	init_mat_arrays(&cell);

	for (i = 0; i < cell.ymeshpoints; i++) {
		cell.phi[i] = 0.0;
		cell.R[i] = 0.0;
		cell.n[i] = 0.0;
	}

	cell.C =
	    cell.xlen * cell.zlen * epsilon0 * cell.epsilonr[0] / (cell.ylen +
								   cell.
								   other_layers);
	if (get_dump_status(dump_print_text) == TRUE)
		printf("C=%le\n", cell.C);
	cell.A = cell.xlen * cell.zlen;
	cell.Vol = cell.xlen * cell.zlen * cell.ylen;

	light_init(&cell.mylight, &cell);
	light_set_dx(&cell.mylight, cell.ymesh[1] - cell.ymesh[0]);
	light_load_config(&cell.mylight, cell.outputpath);
	if (get_dump_status(dump_iodump) == FALSE)
		set_dump_status(dump_optics, FALSE);

	cell.Vapplied = 0.0;
	get_initial(&cell);

	remesh_shrink(&cell);

	solve_pos(&cell);

	remove_dir(cell.outputpath, "snapshots");
	remove_dir(cell.outputpath, "light_dump");
	remove_dir(cell.outputpath, "dynamic");

	time_init(&cell);

	cell.N = 0;
	cell.M = 0;

	solver_realloc(&cell);

	if (cell.onlypos == TRUE) {
		device_free(&cell);
		return 0;
	}

	plot_open(&cell);

	cell.stop = FALSE;

	plot_now(&cell, "plot");

	find_n0(&cell);

	draw_gaus(&cell);

#include "run_list.c"

	device_free(&cell);

	plot_close(&cell);

	for (i = 0; i < cell.mat_layers; i++) {
		dos_free(i);
	}

	solver_free_memory(&cell);
	light_free(&cell.mylight);

	return cell.odes;
}

char *sim_output_path()
{
	return cell.outputpath;
}
