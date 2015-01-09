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

double min_thermal_error = 2e-11;

static int thermal_N;
static int thermal_M;
static int *thermal_Ti;
static int *thermal_Tj;
static double *thermal_Tx;
static double *thermal_b;
static double last_theraml_error;

double get_last_thermal_error()
{
	return last_theraml_error;
}

void thermal_init(struct device *in)
{
	thermal_N = in->ymeshpoints * 3 - 2;
	thermal_M = in->ymeshpoints;
	thermal_Ti = malloc(thermal_N * sizeof(int));
	thermal_Tj = malloc(thermal_N * sizeof(int));
	thermal_Tx = malloc(thermal_N * sizeof(double));
	thermal_b = malloc(thermal_M * sizeof(double));
}

void thermal_free(struct device *in)
{
	free(thermal_Ti);
	free(thermal_Tj);
	free(thermal_Tx);
	free(thermal_b);
}

void update_heat(struct device *in)
{
	int i;
	double Ecl = 0.0;
	double Ecr = 0.0;
	double Evl = 0.0;
	double Evr = 0.0;
	double yl = 0.0;
	double yr = 0.0;
	double dh = 0.0;
	double Jn = 0.0;
	double Jp = 0.0;

	for (i = 0; i < in->ymeshpoints; i++) {
		if (i == 0) {
			Ecl = -in->Xi[0] - in->Vapplied;
			Evl = -in->Xi[0] - in->Vapplied - in->Eg[0];
			yl = in->ymesh[0] - (in->ymesh[1] - in->ymesh[0]);
		} else {
			Ecl = in->Ec[i - 1];
			Evl = in->Ev[i - 1];
			yl = in->ymesh[i - 1];
		}

		if (i == in->ymeshpoints - 1) {
			Ecr = -in->Xi[i] - in->Vr;
			Evr = -in->Xi[i] - in->Vr - in->Eg[i];
			yr = in->ymesh[i] + (in->ymesh[i] - in->ymesh[i - 1]);

		} else {
			Ecr = in->Ec[i];
			Evr = in->Ev[i];
			yr = in->ymesh[i];
		}

		dh = yr - yl;
		Jn = in->Jn[i];
		Jp = in->Jp[i];
		in->He[i] =
		    q * (Ecr - Ecl) * Jn / dh + in->Habs[i] / 2.0 +
		    q * in->Eg[i] * (in->Rn[i] + in->Rp[i]) / 2.0 / 2.0;
		in->Hh[i] =
		    q * (Evr - Evl) * Jp / dh + in->Habs[i] / 2.0 +
		    q * in->Eg[i] * (in->Rn[i] + in->Rp[i]) / 2.0 / 2.0;

	}

}

void dump_thermal(struct device *in)
{
	int i;
	FILE *out;
	out = fopena(in->outputpath, "./Te.dat", "w");
	for (i = 0; i < in->ymeshpoints; i++) {
		fprintf(out, "%le ", in->ymesh[i]);
		fprintf(out, "%le ", in->Te[i]);
		fprintf(out, "\n");
	}
	fclose(out);

	out = fopena(in->outputpath, "./Th.dat", "w");
	for (i = 0; i < in->ymeshpoints; i++) {
		fprintf(out, "%le ", in->ymesh[i]);
		fprintf(out, "%le ", in->Th[i]);
		fprintf(out, "\n");
	}
	fclose(out);

	out = fopena(in->outputpath, "./Tl.dat", "w");
	for (i = 0; i < in->ymeshpoints; i++) {
		fprintf(out, "%le ", in->ymesh[i]);
		fprintf(out, "%le ", in->Tl[i]);
		fprintf(out, "\n");
	}
	fclose(out);

	out = fopena(in->outputpath, "./H.dat", "w");
	for (i = 0; i < in->ymeshpoints; i++) {
		fprintf(out, "%le ", in->ymesh[i]);
		fprintf(out, "%le ", in->He[i] + in->Hh[i]);
		fprintf(out, "\n");
	}
	fclose(out);
}

double get_thermal_error(struct device *in)
{
	double tot = 0.0;
	int i;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += fabs(thermal_b[i]);
	}
	return tot;
}

int solve_thermal(struct device *in)
{
	update_heat(in);
	int i;

	double Tll;
	double Tlc;
	double Tlr;
	double yl;
	double yc;
	double yr;
	double dyl = 0.0;
	double dyc = 0.0;
	double dyr = 0.0;
	int ittr = 0;
	int pos = 0;
	double error;
	double kll = 0.0;
	double klc = 0.0;
	double klr = 0.0;
	double kl0 = 0.0;
	double kl1 = 0.0;
	double dTll = 0.0;
	double dTlc = 0.0;
	double dTlr = 0.0;
	double dTl = 0.0;

	double He = 0.0;
	double Hh = 0.0;

	do {

		pos = 0;
		for (i = 0; i < in->ymeshpoints; i++) {

			if (i == 0) {
				kll = in->kl[0];
				if (in->nofluxl == TRUE) {
					Tll = in->Tl[i];
				} else {
					Tll = in->Tll;
				}

				yl = in->ymesh[0] - (in->ymesh[1] -
						     in->ymesh[0]);
			} else {
				kll = in->kl[i - 1];
				Tll = in->Tl[i - 1];
				yl = in->ymesh[i - 1];
			}

			if (i == (in->ymeshpoints - 1)) {
				klr = in->kl[in->ymeshpoints - 1];
				Tlr = in->Tlr;
				yr = in->ymesh[i] + (in->ymesh[i] -
						     in->ymesh[i - 1]);
			} else {
				klr = in->kl[i + 1];
				Tlr = in->Tl[i + 1];
				yr = in->ymesh[i + 1];
			}

			yc = in->ymesh[i];
			dyl = yc - yl;
			dyr = yr - yc;

			dyc = (dyl + dyr) / 2.0;

			klc = in->kl[i];

			kl0 = (kll + klc) / 2.0;
			kl1 = (klc + klr) / 2.0;

			Tlc = in->Tl[i];

			He = in->He[i];
			Hh = in->Hh[i];

			dTll = kl0 / dyl / dyc;
			dTlc = -(kl0 / dyl / dyc + kl1 / dyr / dyc);
			dTlr = kl1 / dyr / dyc;

			if (in->nofluxl == TRUE) {
				dTlc += dTll;
				dTll = 0.0;
			}

			dTl = dTll * Tll + dTlc * Tlc + dTlr * Tlr;

			if (i != 0) {
				thermal_Ti[pos] = i;
				thermal_Tj[pos] = i - 1;
				thermal_Tx[pos] = dTll;
				pos++;

			}

			thermal_Ti[pos] = i;
			thermal_Tj[pos] = i;
			thermal_Tx[pos] = dTlc;
			pos++;

			if (i != (in->ymeshpoints - 1)) {
				thermal_Ti[pos] = i;
				thermal_Tj[pos] = i + 1;
				thermal_Tx[pos] = dTlr;
				pos++;

			}

			thermal_b[i] = -(dTl + He + Hh);

		}

		solver(thermal_M, thermal_N, thermal_Ti, thermal_Tj, thermal_Tx,
		       thermal_b);

		for (i = 0; i < in->ymeshpoints; i++) {
			in->Tl[i] += thermal_b[i];
			in->Te[i] = in->Tl[i];
			in->Th[i] = in->Tl[i];

		}

		error = get_thermal_error(in);

		ittr++;
		printf("Thermal error = %le ittr=%d\n", error, ittr);
	} while ((ittr < 100) && (error > min_thermal_error));
	last_theraml_error = error;

	return 0;
}
