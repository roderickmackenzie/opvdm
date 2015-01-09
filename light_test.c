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
#include <errno.h>
#include <util.h>
#include <true_false.h>
#include "../../dump_ctrl.h"
#include "../../complex_solver.h"
#include "../../const.h"
#include "../../light.h"

int light_test_air_abs_interface(struct light *in, int lam)
{
#ifdef del
	if (get_dump_status(dump_optics) == TRUE) {
		char one[100];
		sprintf(one, "Solve light optical slice at %lf nm\n",
			in->l[lam] * 1e9);

		waveprint(one, in->l[lam] * 1e9);
	}
	int i;

	double complex Epl = 0.0 + 0.0 * I;
	double complex Epc = 0.0 + 0.0 * I;
	double complex Epr = 0.0 + 0.0 * I;

	double complex Enc = 0.0 + 0.0 * I;
	double complex Enr = 0.0 + 0.0 * I;

	double complex r0 = 0.0 + 0.0 * I;
	double complex r1 = 0.0 + 0.0 * I;
	double complex t0 = 0.0 + 0.0 * I;
	double complex t1 = 0.0 + 0.0 * I;

	double dyl = 0.0;
	double xl = 0.0;
	double xc = 0.0;

	int j = 0;
	double dj = 0.0;
	for (i = 0; i < in->points - 1; i++) {
		if (in->n[lam][i] != in->n[lam][i + 1]) {
			j = i;
			break;
		}

		if (in->alpha[lam][i] != in->alpha[lam][i + 1]) {
			j = i;
			break;
		}

	}
	dj = in->x[j];
	printf("Interface at %d %le\n", j, dj);

	double kappa0 = in->alpha[lam][j] * (in->l[lam] / (pi * 4.0));
	double complex n0 = in->n[lam][j] + kappa0 * I;

	double kappa1 = in->alpha[lam][j + 1] * (in->l[lam] / (pi * 4.0));
	double complex n1 = in->n[lam][j + 1] + kappa1 * I;

	complex double r = (n0 - n1) / (n0 + n1);
	complex double t = (2.0 * n0) / (n0 + n1);

	int ittr = 0;
	int pos = 0;
	double l = in->l[lam];
	double nc;
	double alpha;
	double tot_error = 0.0;
	int quit = FALSE;
	double test = FALSE;
	double delta = (in->x[1] - in->x[0]);
	printf("%le\n", delta);
	for (i = 0; i < in->points; i++) {
		double x = in->x[i];
		double len = in->x[in->points - 1] + in->x[0] / 2;
		double kappa = in->alpha[lam][i] * (in->l[lam] / (pi * 4.0));
		double complex n = in->n[lam][i] + kappa * I;
		double complex beta0 = 0.0 + 0.0 * I;
		double complex beta1 = 0.0 + 0.0 * I;
		complex double Ep = 0.0 + 0.0 * I;
		complex double En = 0.0 + 0.0 * I;

		beta0 = (2 * pi * n0 / in->l[lam]);
		beta1 = (2 * pi * n1 / in->l[lam]);

		if (x < dj) {
			Ep = cexp(beta0 * x * I);
			En = r * cexp(beta0 * (dj) * I) * cexp(-beta0 *
							       (x - dj) * I);

		} else {
			Ep = t * cexp(beta0 * dj * I) * cexp(beta1 * (x - dj) *
							     I);
			En = 0.0 + 0.0 * I;
		}

		in->Ep[lam][i] = creal(Ep);
		in->Epz[lam][i] = cimag(Ep);
		in->En[lam][i] = creal(En);
		in->Enz[lam][i] = cimag(En);

	}

#endif
	return 0;
}

int light_test_air_metal(struct light *in, int lam)
{
#ifdef del
	if (get_dump_status(dump_optics) == TRUE) {
		char one[100];
		sprintf(one, "Solve light optical slice at %lf nm\n",
			in->l[lam] * 1e9);

		waveprint(one, in->l[lam] * 1e9);
	}
	int i;

	double complex Epl = 0.0 + 0.0 * I;
	double complex Epc = 0.0 + 0.0 * I;
	double complex Epr = 0.0 + 0.0 * I;

	double complex Enc = 0.0 + 0.0 * I;
	double complex Enr = 0.0 + 0.0 * I;

	double complex r0 = 0.0 + 0.0 * I;
	double complex r1 = 0.0 + 0.0 * I;
	double complex t0 = 0.0 + 0.0 * I;
	double complex t1 = 0.0 + 0.0 * I;

	double dyl = 0.0;

	double xl = 0.0;
	double xc = 0.0;

	int ittr = 0;
	int pos = 0;

	double l = in->l[lam];
	double nc;
	double alpha;
	double tot_error = 0.0;
	int quit = FALSE;
	double test = FALSE;

	for (i = 0; i < in->points; i++) {
		double x = in->x[i];
		double len = in->x[in->points - 1] + in->x[0] / 2;
		double kappa = in->alpha[lam][i] * (in->l[lam] / (pi * 4.0));
		double complex n = 1.0 + kappa * I;
		double complex beta = (2 * pi * n / in->l[lam]);
		complex double Ep = cexp(beta * x * I);
		complex double En = -cexp(-beta * (x - 2.0 * len) * I);
		in->Ep[lam][i] = creal(Ep);
		in->Epz[lam][i] = cimag(Ep);
		in->En[lam][i] = creal(En);
		in->Enz[lam][i] = cimag(En);

	}

#endif
	return 0;
}
