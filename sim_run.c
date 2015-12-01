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
#include <unistd.h>
#include "sim.h"
#include "dos.h"
#include "dump.h"
#include "complex_solver.h"
#include "elec_plugins.h"
#include "ntricks.h"

struct device cell;

int run_simulation(char *outputpath, char *inputpath)
{
	char temp[1000];
	printf("Run_simulation\n");

	cell.kl_in_newton = FALSE;

	if (strcmp(outputpath, "") != 0)
		strcpy(cell.outputpath, outputpath);

	if (strcmp(inputpath, "") != 0)
		strcpy(cell.inputpath, inputpath);

	dump_init(&cell);
	dump_load_config(&cell);

	int i;

	printf("Load config\n");
	load_config(&cell);

	if (cell.simmode != sim_mode_optics) {
		printf("Loading DoS for %d layers\n",
		       cell.my_epitaxy.electrical_layers);
		char tempn[100];
		char tempp[100];
		i = 0;
		dos_init(i);
		printf("Load DoS %d/%d\n", i,
		       cell.my_epitaxy.electrical_layers);
		sprintf(tempn, "%s_dosn.dat", cell.my_epitaxy.dos_file[i]);
		sprintf(tempp, "%s_dosp.dat", cell.my_epitaxy.dos_file[i]);
		load_dos(&cell, tempn, tempp, i);

		if (get_dump_status(dump_write_converge) == TRUE) {
			cell.converge =
			    fopena(cell.outputpath, "converge.dat", "w");
			fclose(cell.converge);

			cell.tconverge =
			    fopena(cell.outputpath, "tconverge.dat", "w");
			fclose(cell.tconverge);
		}
	}
	device_init(&cell);

	join_path(2, temp, cell.outputpath, "equilibrium");
	remove_dir(temp);

	join_path(2, temp, cell.outputpath, "snapshots");
	remove_dir(temp);

	join_path(2, temp, cell.outputpath, "light_dump");
	remove_dir(temp);

	join_path(2, temp, cell.outputpath, "dynamic");
	remove_dir(temp);

	for (i = 0; i < cell.ymeshpoints; i++) {
		cell.Nad[i] = get_dos_doping(cell.imat[i]);
	}

	init_mat_arrays(&cell);

	if (cell.simmode != sim_mode_optics) {
		for (i = 0; i < cell.ymeshpoints; i++) {
			cell.phi[i] = 0.0;
			cell.R[i] = 0.0;
			cell.n[i] = 0.0;
		}

		cell.C =
		    cell.xlen * cell.zlen * epsilon0 * cell.epsilonr[0] /
		    (cell.ylen + cell.other_layers);
		if (get_dump_status(dump_print_text) == TRUE)
			printf("C=%le\n", cell.C);
		cell.A = cell.xlen * cell.zlen;
		cell.Vol = cell.xlen * cell.zlen * cell.ylen;

		light_init(&cell.mylight, &cell, cell.outputpath);
		light_set_dx(&cell.mylight, cell.ymesh[1] - cell.ymesh[0]);
		light_load_config(&cell.mylight);
		if (get_dump_status(dump_iodump) == FALSE)
			set_dump_status(dump_optics, FALSE);

		cell.Vapplied = 0.0;
		get_initial(&cell);

		remesh_shrink(&cell);

		if (cell.math_enable_pos_solver == TRUE)
			solve_pos(&cell);

		time_init(&cell);

		cell.N = 0;
		cell.M = 0;

		solver_realloc(&cell);

		plot_open(&cell);

		cell.stop = FALSE;

		plot_now(&cell, "plot");

		find_n0(&cell);

		draw_gaus(&cell);

		if (cell.onlypos == TRUE) {
			dump_1d_slice(&cell, "equilibrium", "");
			device_free(&cell);
			return 0;
		}
	}
#include "run_list.c"

	device_free(&cell);

	if (cell.simmode != sim_mode_optics) {
		plot_close(&cell);

		for (i = 0; i < cell.my_epitaxy.electrical_layers; i++) {
			dos_free(i);
		}

		solver_free_memory(&cell);

		light_free(&cell.mylight);
	}

	return cell.odes;
}

char *sim_output_path()
{
	return cell.outputpath;
}

char *sim_input_path()
{
	return cell.inputpath;
}
