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
#include "solver.h"
double min_pos_error = 1e-4;

void pos_dump(struct device *in)
{
	if (get_dump_status(dump_iodump) == TRUE) {
		int band = 0;
		int i = 0;
		FILE *out;
		out = fopena(in->outputpath, "./equ.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e %e\n", in->ymesh[i], in->Fn[i],
				in->Fp[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "./equ_Fi.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Fi[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "./equ_Ec.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Ec[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "./equ_Ev.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Ev[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "./equ_n.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->n[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "./equ_p.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->p[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "./equ_phi.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->phi[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "./equ_np.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e %e\n", in->ymesh[i], in->n[i],
				in->p[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "./equ_np_trap.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e ", in->ymesh[i]);
			for (band = 0; band < in->srh_bands; band++) {
				fprintf(out, "%e %e ", in->nt[i][band],
					in->pt[i][band]);
			}
			fprintf(out, "\n");
		}
		fclose(out);

	}
}

double get_p_error(struct device *in, double *b)
{
	double tot = 0.0;
	int i;
	for (i = 0; i < in->ymeshpoints; i++) {
		if ((in->interfaceleft == TRUE) && (i == 0)) {
		} else
		    if ((in->interfaceright == TRUE)
			&& (i == in->ymeshpoints - 1)) {
		} else {
			tot += fabs(b[i]);
		}
	}
	return tot;
}

int solve_pos(struct device *in)
{
	if (get_dump_status(dump_iodump) == TRUE)
		printf("Solve pos\n");
	int i;

	int N = in->ymeshpoints * 3 - 2;

	int M = in->ymeshpoints;
	int *Ti = malloc(N * sizeof(int));
	int *Tj = malloc(N * sizeof(int));
	double *Tx = malloc(N * sizeof(double));
	double *b = malloc(M * sizeof(double));

	double phil;
	double phic;
	double phir;
	double yl;
	double yc;
	double yr;
	double dyl;
	double dyr;
	double dyc = 0.0;
	int ittr = 0;
	int pos = 0;
	double error = 1000;
	double el = 0.0;
	double ec = 0.0;
	double er = 0.0;
	double e0 = 0.0;
	double e1 = 0.0;
	int pos_max_ittr = 250;
	if (antje1() == TRUE) {
		pos_max_ittr = 2000;
		min_pos_error = 1e-15;
	}
	in->enabled = get_dump_status(dump_lock_enable);
	int adv = FALSE;
	int band;

	do {

		if (in->interfaceleft == TRUE) {
			in->phi[0] = in->Vl;
		}

		if (in->interfaceright == TRUE) {
			in->phi[in->ymeshpoints - 1] = in->Vr;
		}

		pos = 0;

		for (i = 0; i < in->ymeshpoints; i++) {

			if (i == 0) {
				phil = in->Vl;
				el = in->epsilonr[0] * epsilon0;
				yl = in->ymesh[0] - (in->ymesh[1] -
						     in->ymesh[0]);

			} else {
				el = in->epsilonr[i - 1] * epsilon0;
				phil = in->phi[i - 1];
				yl = in->ymesh[i - 1];
			}

			if (i == (in->ymeshpoints - 1)) {
				phir = in->Vr;
				er = in->epsilonr[i] * epsilon0;
				yr = in->ymesh[i] + (in->ymesh[i] -
						     in->ymesh[i - 1]);
			} else {
				er = in->epsilonr[i + 1] * epsilon0;
				phir = in->phi[i + 1];
				yr = in->ymesh[i + 1];

			}

			yc = in->ymesh[i];
			dyl = yc - yl;
			dyr = yr - yc;
			dyc = (dyl + dyr) / 2.0;

			ec = in->epsilonr[i] * epsilon0;
			e0 = (el + ec) / 2.0;
			e1 = (ec + er) / 2.0;
			phic = in->phi[i];

			double dphidn = 0.0;
			if (adv == FALSE) {
				dphidn =
				    (q / (kb * in->Tl[i])) * in->Nc[i] *
				    exp(((in->Fi[i] -
					  (-in->phi[i] -
					   in->Xi[i]))) / (in->kTq));

			} else {
				dphidn =
				    get_dn_den(in->Fi[i] - in->Ec[i], in->Tl[i],
					       in->imat[i]);

			}

			double dphidp = 0.0;
			if (adv == FALSE) {
				dphidp =
				    -(q / (kb * in->Tl[i])) * in->Nv[i] *
				    exp((((-in->phi[i] - in->Xi[i] -
					   in->Eg[i]) -
					  in->Fi[i])) / (in->kTq));
			} else {
				dphidp =
				    -get_dp_den(in->xp[i] - in->tp[i],
						in->Tl[i], in->imat[i]);
			}
			double dphil = e0 / dyl / dyc;
			double dphic = -(e0 / dyl / dyc + e1 / dyr / dyc);
			double dphir = e1 / dyr / dyc;

			double dphil_d = dphil;
			double dphic_d = dphic;
			double dphir_d = dphir;

			if (in->interfaceleft == TRUE) {

				if (i == 1) {
					dphil_d = 0.0;
					phil = in->Vl;

				}

				if (i == 0) {
					dphil_d = 0.0;
					dphic_d = 1e-6;
					dphir_d = 0.0;

				}
			}

			if (in->interfaceright == TRUE) {

				if (i == in->ymeshpoints - 2) {
					dphir_d = 0.0;
					phir = in->Vr;

				}

				if (i == in->ymeshpoints - 1) {
					dphil_d = 0.0;
					dphic_d = 1e-6;
					dphir_d = 0.0;

				}
			}

			double dphi =
			    dphil * phil + dphic * phic + dphir * phir;

			dphic_d += -q * (dphidn - dphidp);

			if (i != 0) {
				Ti[pos] = i;
				Tj[pos] = i - 1;
				Tx[pos] = dphil_d;
				pos++;
			}

			Ti[pos] = i;
			Tj[pos] = i;
			Tx[pos] = dphic_d;

			pos++;

			if (i != (in->ymeshpoints - 1)) {
				Ti[pos] = i;
				Tj[pos] = i + 1;
				Tx[pos] = dphir_d;
				pos++;

			}

			if ((in->interfaceleft == TRUE) && (i == 0)) {
				b[i] = -0.0;
			} else
			    if ((in->interfaceright == TRUE)
				&& (i == in->ymeshpoints - 1)) {
				b[i] = -0.0;
			} else {
				b[i] =
				    -(dphi -
				      q * (in->n[i] - in->p[i] - in->Nad[i]));

			}

		}

		error = get_p_error(in, b);

		solver(M, N, Ti, Tj, Tx, b);

		for (i = 0; i < in->ymeshpoints; i++) {
			if ((in->interfaceleft == TRUE) && (i == 0)) {
			} else
			    if ((in->interfaceright == TRUE)
				&& (i == in->ymeshpoints - 1)) {
			} else {
				double update;

				if (get_clamp_state() == TRUE) {
					update =
					    b[i] / (1.0 +
						    fabs(b[i] / in->posclamp /
							 (300.0 * kb / q)));
				} else {
					update = b[i];
				}
				in->phi[i] += update;

			}
		}

		for (i = 0; i < in->ymeshpoints; i++) {
			in->Ec[i] = -in->phi[i] - in->Xi[i];
			in->Ev[i] = -in->phi[i] - in->Xi[i] - in->Eg[i];

			if (adv == FALSE) {
				in->n[i] =
				    in->Nc[i] *
				    exp(((in->Fi[i] -
					  in->Ec[i]) * q) / (kb * in->Tl[i]));
				in->dn[i] =
				    (q / (kb * in->Tl[i])) * in->Nc[i] *
				    exp((q * (in->Fi[i] - in->Ec[i])) /
					(kb * in->Tl[i]));
			} else {
				in->n[i] =
				    get_n_den(in->Fi[i] - in->Ec[i], in->Tl[i],
					      in->imat[i]);
				in->dn[i] =
				    get_dn_den(in->Fi[i] - in->Ec[i], in->Tl[i],
					       in->imat[i]);
			}

			in->Fn[i] = in->Fi[i];
			in->Fp[i] = in->Fi[i];
			in->x[i] = in->phi[i] + in->Fn[i];

			in->xp[i] = -(in->phi[i] + in->Fp[i]);

			if (adv == FALSE) {
				in->p[i] =
				    in->Nv[i] *
				    exp(((in->xp[i] -
					  in->tp[i]) * q) / (kb * in->Tl[i]));
				in->dp[i] =
				    (q / (kb * in->Tl[i])) * in->Nv[i] *
				    exp(((in->xp[i] -
					  in->tp[i]) * q) / (kb * in->Tl[i]));
			} else {
				in->p[i] =
				    get_p_den(in->xp[i] - in->tp[i], in->Tl[i],
					      in->imat[i]);
				in->dp[i] =
				    get_dp_den(in->xp[i] - in->tp[i], in->Tl[i],
					       in->imat[i]);
			}

			for (band = 0; band < in->srh_bands; band++) {
				in->Fnt[i][band] =
				    -in->phi[i] - in->Xi[i] +
				    dos_srh_get_fermi_n(in->n[i], in->p[i],
							band, in->imat[i]);
				in->xt[i][band] = in->phi[i] + in->Fnt[i][band];
				in->nt[i][band] =
				    get_n_pop_srh(in->xt[i][band] + in->tt[i],
						  in->Te[i], band, in->imat[i]);
				in->dnt[i][band] =
				    get_dn_pop_srh(in->xt[i][band] + in->tt[i],
						   in->Te[i], band,
						   in->imat[i]);

				in->Fpt[i][band] =
				    -in->phi[i] - in->Xi[i] - in->Eg[i] -
				    dos_srh_get_fermi_p(in->n[i], in->p[i],
							band, in->imat[i]);
				in->xpt[i][band] =
				    -(in->phi[i] + in->Fpt[i][band]);
				in->pt[i][band] =
				    get_p_pop_srh(in->xpt[i][band] - in->tpt[i],
						  in->Th[i], band, in->imat[i]);
				in->dpt[i][band] =
				    get_dp_pop_srh(in->xpt[i][band] -
						   in->tpt[i], in->Th[i], band,
						   in->imat[i]);
			}

		}

		update_arrays(in);
		in->xnl_left = in->x[0];
		in->xpl_left = in->xp[0];

		if (error < 1) {
			adv = TRUE;
		}

		if (get_dump_status(dump_print_pos_error) == TRUE)
			printf("%d Pos error = %e %d\n", ittr, error, adv);

#ifdef dump_converge

		/*in->converge=fopena(in->outputpath,"./converge.dat","a");
		   fprintf(in->converge,"%e\n",error);
		   fclose(in->converge); */
#endif

		ittr++;

	} while ((ittr < pos_max_ittr) && (error > min_pos_error));

	pos_dump(in);

	update_arrays(in);

	if (in->srh_sim == TRUE) {
		time_init(in);

	}

	in->odes += in->ymeshpoints;

	for (i = 0; i < in->ymeshpoints; i++) {
		in->nfinit[i] = in->n[i];
		in->pfinit[i] = in->p[i];
		in->ntinit[i] = 0.0;
		in->ptinit[i] = 0.0;
	}

	free(Ti);
	free(Tj);
	free(Tx);
	free(b);

	if (in->enabled == FALSE) {
		antje2();
	}

	if (get_dump_status(dump_iodump) == TRUE) {
		printf("Solved pos\n");
		printf("Vl=%le Vr=%le phi_mid=%le\n", in->Vl, in->Vr,
		       in->phi[in->ymeshpoints / 2]);
	}
	return 0;
}
