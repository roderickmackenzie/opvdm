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
#include "util.h"
#include <pwd.h>
#include <unistd.h>
#include "sim.h"
#include "dos.h"
#include "dump.h"
#include "elec_plugins.h"
#include "complex_solver.h"

void find_n0(struct device *in)
{
	printf("Finding n0\n");
	double oldsun = in->Psun;
	double oldv = in->Vapplied;
	double B_temp;
	in->Vapplied = 0;
	in->Psun = 0;

	light_solve_and_update(in, &(in->mylight), in->Psun, 0.0);
	dump_for_plot(in, "");

	B_temp = in->B;
	in->B = 0.0;
	double save_clamp = in->electrical_clamp;
	int save_ittr = in->max_electrical_itt;
	in->electrical_clamp = in->electrical_clamp0;
	in->max_electrical_itt = in->max_electrical_itt0;

	solve_all(in);

	in->max_electrical_itt = save_ittr;
	in->electrical_clamp = save_clamp;
	solve_all(in);
	in->B = B_temp;

	reset_npinit(in);
	reset_npequlib(in);

	FILE *outfile;
	outfile = fopena(in->outputpath, "./voc_mue.dat", "w");
	fprintf(outfile, "%le", get_avg_mue(in));
	fclose(outfile);

	outfile = fopena(in->outputpath, "./voc_muh.dat", "w");
	fprintf(outfile, "%le", get_avg_muh(in));
	fclose(outfile);

	in->Psun = oldsun;
	in->Vapplied = oldv;
	light_solve_and_update(in, &(in->mylight), in->Psun, 0.0);

}
