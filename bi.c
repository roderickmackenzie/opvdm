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
#include <math.h>

double min_bi_error = 1e10;

double get_bi_error(int ymeshpoints, double *b)
{
	double tot = 0.0;
	int i;
	for (i = 0; i < ymeshpoints; i++) {

		tot += mod(b[i]);
	}
	return tot;
}

void solve_bi()
{
	printf("Solve sdot\n");
	int i;

	int ymeshpoints = 2000;
	int N = ymeshpoints * 3 - 2;
	int M = ymeshpoints;

	FILE *jfile = fopen("./jfile.dat", "w");
	fprintf(jfile, "#rod");
	fclose(jfile);
	int *Ti = malloc(N * sizeof(int));
	int *Tj = malloc(N * sizeof(int));
	double *Tx = malloc(N * sizeof(double));

	double *b = malloc(M * sizeof(double));
	double *ymesh = malloc(M * sizeof(double));
	double *p = malloc(M * sizeof(double));
	double *pt = malloc(M * sizeof(double));
	double *J = malloc(M * sizeof(double));
	double *k = malloc(M * sizeof(double));
	double dy = (40 * 1e-9) / ((double)ymeshpoints);
	double start = 1e-9;

	double prb = 1e22;

	double meshpos = start;

	for (i = 0; i < ymeshpoints; i++) {
		pt[i] = prb;
		p[i] = prb;
		ymesh[i] = meshpos;
		meshpos += dy;
	}

	double epsilonr = 3.8;
	double dt = 1e-12;
	double yl;
	double yc;
	double yr;
	double dyl;
	double dyr;
	double dyc = 0.0;
	int ittr = 0;
	int pos = 0;
	double error = 1000;
	double pl;
	double pc;
	double pr;
	double mun = 1e-7;
	double mup = 1e-8;
	double D = ((kb * 300.0 / q) * ((mun + mup) / 2.0));
	double r = 0.0;
	double plb = 0.0;
	double ec = 0.0;
	double u;
	double du;
	double ddu;
	double T = 300.0;
	double beta = 1.0 / (T * kb);
	int tstep = 0;
	int fixed = FALSE;
	double constant = 0.0;
	do {
		printf("Doing time step %e\n", ((double)tstep) * dt);
		do {

			pos = 0;
			for (i = 0; i < ymeshpoints; i++) {

				if (i == 0) {
					if (fixed == TRUE) {
						pl = plb;
					} else {
						double mul = 0.0;
						pl = p[0] * mul;
						constant = mul;
					}

					yl = ymesh[0] - (ymesh[1] - ymesh[0]);

				} else {
					pl = p[i - 1];
					yl = ymesh[i - 1];
				}

				if (i == (ymeshpoints - 1)) {
					pr = prb;
					yr = ymesh[i] + (ymesh[i] -
							 ymesh[i - 1]);
				} else {
					pr = p[i + 1];
					yr = ymesh[i + 1];

				}

				yc = ymesh[i];
				dyl = yc - yl;
				dyr = yr - yc;
				dyc = (dyl + dyr) / 2.0;

				r = ymesh[i];

				pc = p[i];

				u = q * q / (4 * pi * epsilon0 * epsilonr * r);
				du = -1.0 * q * q / (4 * pi * epsilon0 *
						     epsilonr * r * r);
				ddu =
				    2.0 * q * q / (4 * pi * epsilon0 *
						   epsilonr * r * r * r);

				ec = epsilonr * epsilon0;

				double dpl = D / dyl / dyc;
				double dpc = -(D / dyl / dyc + D / dyr / dyc);
				double dpr = D / dyr / dyc;

				double dp =
				    dpl * pl + dpc * pc + dpr * pr +
				    D * beta * du * (pr - pl) / (dyr + dyl) +
				    D * beta * pc * ddu - (p[i] - pt[i]) / dt;
				J[i] =
				    -(D * (pr - pl) / (dyr + dyl) +
				      D * beta * pc * du);
				if (i == 0) {
					printf("%e %e %e\n", J[i],
					       D * (pr - pl) / (dyr + dyl),
					       D * beta * pc * du);
					getchar();
				}
				k[i] = J[i] * 4 * pi * r * r / prb;
				dpc += D * beta * ddu - 1.0 / dt;

				dpl += D * beta * du * (-1.0) / (dyr + dyl);
				dpr += D * beta * du / (dyr + dyl);

				if (fixed == FALSE) {
					if (i == 0) {
						dpc += constant * dpl;
						dpl = 0.0;
					}
				}

				if (i != 0) {
					Ti[pos] = i;
					Tj[pos] = i - 1;
					Tx[pos] = dpl;
					pos++;

				}

				Ti[pos] = i;
				Tj[pos] = i;
				Tx[pos] = dpc;
				pos++;

				if (i != (ymeshpoints - 1)) {
					Ti[pos] = i;
					Tj[pos] = i + 1;
					Tx[pos] = dpr;
					pos++;

				}

				b[i] = -(dp);

			}

			solver(M, N, Ti, Tj, Tx, b);

			for (i = 0; i < ymeshpoints; i++) {
				p[i] += b[i];
			}

			error = get_bi_error(ymeshpoints, b);

			printf("%d Pos error = %e\n", ittr, error);

			ittr++;
		} while ((ittr < 10) && (error > min_bi_error));

		FILE *out;
		out = fopen("./rbi_calc.dat", "w");
		for (i = 0; i < ymeshpoints; i++) {
			fprintf(out, "%e %e %e %e\n", ymesh[i], p[i], J[i],
				k[i]);
		}
		fclose(out);
		tstep++;
		ittr = 0;

		for (i = 0; i < ymeshpoints; i++) {
			pt[i] = p[i];
		}

		FILE *jfile = fopen("./jfile.dat", "a");
		fprintf(jfile, "%e %e %e\n", dt * ((double)tstep), J[0], k[0]);
		printf("%e %e %e\n", dt * ((double)tstep), J[0], k[0]);
		fclose(jfile);

	} while (1);

	free(k);
	free(Ti);
	free(Tj);
	free(Tx);
	free(b);
	free(J);
	free(ymesh);
	free(p);
	free(pt);
	printf("Done\n");
	exit(0);
}
