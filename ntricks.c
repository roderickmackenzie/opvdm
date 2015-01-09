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
#include "sim.h"
#include "dump.h"
#include "ntricks.h"

static int unused __attribute__ ((unused));

struct newton_math_state math_save_state;

void newton_push_state(struct device *in)
{
	math_save_state.min_cur_error = in->min_cur_error;
	math_save_state.max_electrical_itt = in->max_electrical_itt;
	math_save_state.newton_min_itt = in->newton_min_itt;
	math_save_state.electrical_clamp = in->electrical_clamp;
	math_save_state.newton_clever_exit = in->newton_clever_exit;
}

void newton_pop_state(struct device *in)
{
	in->min_cur_error = math_save_state.min_cur_error;
	in->max_electrical_itt = math_save_state.max_electrical_itt;
	in->newton_min_itt = math_save_state.newton_min_itt;
	in->electrical_clamp = math_save_state.electrical_clamp;
	in->newton_clever_exit = math_save_state.newton_clever_exit;
}

void ramp_externalv(struct device *in, double from, double to)
{
	double V = from;
	double dV = 0.1;
	if ((to - from) < 0.0)
		dV *= -1.0;
	printf("dV=%le\n", dV);
	printf("Ramping: from=%e to=%e\n", from, to);

	if (fabs(to - from) <= fabs(dV))
		return;

	do {
		V += dV;
		if (get_dump_status(dump_print_text) == TRUE)
			printf("ramp: %lf %lf %d\n", V, to, in->newton_cur);
		sim_externalv(in, V);

		plot_now(in, "jv.plot");

		if (fabs(in->Vapplied - to) < fabs(dV)) {
			break;
		}

	} while (1);

	if (V != to) {
		V = to;
		sim_externalv(in, V);
	}

	return;
}

void ramp(struct device *in, double from, double to, double steps)
{
	in->newton_cur = FALSE;
	solver_realloc(in);

	in->Vapplied = from;
	newton_push_state(in);
	double dV = 0.20;
	in->min_cur_error = 1e-5;
	in->max_electrical_itt = 12;
	in->newton_min_itt = 3;
	in->electrical_clamp = 2.0;
	in->newton_clever_exit = FALSE;
	if ((to - from) < 0.0)
		dV *= -1.0;
	printf("dV=%le\n", dV);
	printf("Ramping: from=%e to=%e\n", from, to);

	if (fabs(to - from) <= fabs(dV))
		return;

	do {
		in->Vapplied += dV;

		if (get_dump_status(dump_print_text) == TRUE)
			printf("ramp: %lf %lf %d\n", in->Vapplied, to,
			       in->newton_cur);
		solve_all(in);
		plot_now(in, "jv_vars.plot");

		if (fabs(in->Vapplied - to) < fabs(dV)) {

			break;
		}

	} while (1);

	newton_pop_state(in);

	if (in->Vapplied != to) {
		in->Vapplied = to;
		solve_all(in);
	}

	printf("Finished with ramp\n");
	return;
}

void save_state(struct device *in, double to)
{

	printf("Dumping state\n");
	int i;
	int band;
	FILE *state;
	state = fopena(in->outputpath, "./state.dat", "w");

	fprintf(state, "%le ", to);

	for (i = 0; i < in->ymeshpoints; i++) {
		fprintf(state, "%le %le %le ", in->phi[i], in->x[i], in->xp[i]);

		for (band = 0; band < in->srh_bands; band++) {
			fprintf(state, "%le %le ", in->xt[i][band],
				in->xpt[i][band]);
		}

	}
	fclose(state);

}

int load_state(struct device *in, double voltage)
{

	printf("Load state\n");
	int i;
	int band;
	double vtest;
	FILE *state;
	state = fopena(in->outputpath, "./state.dat", "r");
	if (!state) {
		printf("State not found\n");
		return FALSE;
	}

	unused = fscanf(state, "%le", &vtest);
	printf("%le %le", voltage, vtest);
	if (vtest != voltage) {
		printf("State not found\n");
		return FALSE;
	}
	printf("Loading state\n");

	in->Vapplied = vtest;

	for (i = 0; i < in->ymeshpoints; i++) {
		unused =
		    fscanf(state, "%le %le %le ", &(in->phi[i]), &(in->x[i]),
			   &(in->xp[i]));

		for (band = 0; band < in->srh_bands; band++) {
			unused =
			    fscanf(state, "%le %le ", &(in->xt[i][band]),
				   &(in->xpt[i][band]));
		}

	}
	fclose(state);
	return TRUE;

}

double sim_externalv_ittr(struct device *in, double wantedv)
{
	double clamp = 0.1;
	double step = 0.01;
	double e0;
	double e1;
	double i0;
	double i1;
	double deriv;
	double Rs = in->Rcontact;
	solve_all(in);
	i0 = get_I(in);

	double itot = i0 + in->Vapplied / in->Rshunt;

	e0 = fabs(itot * Rs + in->Vapplied - wantedv);
	in->Vapplied += step;
	solve_all(in);

	i1 = get_I(in);
	itot = i1 + in->Vapplied / in->Rshunt;

	e1 = fabs(itot * Rs + in->Vapplied - wantedv);

	deriv = (e1 - e0) / step;
	step = -e1 / deriv;

	in->Vapplied += step;
	int count = 0;
	int max = 1000;
	do {
		e0 = e1;
		solve_all(in);
		itot = i1 + in->Vapplied / in->Rshunt;
		e1 = fabs(itot * Rs + in->Vapplied - wantedv);

		deriv = (e1 - e0) / step;
		step = -e1 / deriv;

		step = step / (1.0 + fabs(step / clamp));
		in->Vapplied += step;
		if (count > max)
			break;
		count++;
	} while (e1 > 1e-8);

	double ret = get_I(in) + in->Vapplied / in->Rshunt;

	return ret;
}

double sim_externalv(struct device *in, double wantedv)
{
	in->newton_cur = FALSE;
	solver_realloc(in);
	sim_externalv_ittr(in, wantedv);
	return 0.0;
}

void solve_all(struct device *in)
{
	solve_cur(in);
}

void newton_sim_jv(struct device *in)
{
	in->newton_cur = FALSE;
	solver_realloc(in);

	solve_all(in);
}
