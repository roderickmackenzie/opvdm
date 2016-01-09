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
#include "sim.h"
#include "solver.h"
#include "buffer.h"
#include "log.h"

double min_pos_error = 1e-4;

void pos_dump(struct device *in)
{
	if (get_dump_status(dump_first_guess) == TRUE) {
		char out_dir[1000];
		join_path(2, out_dir, in->outputpath, "equilibrium");
		struct buffer buf;
		buffer_init(&buf);
		char name[200];
		int band = 0;
		int i = 0;
		FILE *out;
		out = fopena(in->outputpath, "first_guess.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e %e\n", in->ymesh[i], in->Fn[i],
				in->Fp[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "first_guess_Fi.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Fi[i]);
		}
		fclose(out);

		buffer_malloc(&buf);
		sprintf(name, "%s%s", "first_guess_Ec", ".dat");
		buf.y_mul = 1.0;
		buf.x_mul = 1e9;
		strcpy(buf.title, "LUMO energy - position");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Position");
		strcpy(buf.y_label, "LUMO");
		strcpy(buf.x_units, "nm");
		strcpy(buf.y_units, "$E_{LUMO}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, in->ymesh, in->Ec, in->ymeshpoints);
		buffer_dump_path(out_dir, name, &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		sprintf(name, "%s%s", "first_guess_Ev", ".dat");
		buf.y_mul = 1.0;
		buf.x_mul = 1e9;
		strcpy(buf.title, "HOMO energy - position");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Position");
		strcpy(buf.y_label, "LUMO");
		strcpy(buf.x_units, "nm");
		strcpy(buf.y_units, "$E_{HOMO}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, in->ymesh, in->Ev, in->ymeshpoints);
		buffer_dump_path(out_dir, name, &buf);
		buffer_free(&buf);

		out = fopena(in->outputpath, "first_guess_n.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->n[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "first_guess_p.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->p[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "first_guess_phi.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->phi[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "first_guess_np.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e %e\n", in->ymesh[i], in->n[i],
				in->p[i]);
		}
		fclose(out);

		out = fopena(in->outputpath, "first_guess_np_trap.dat", "w");
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
		printf_log("Solve pos\n");
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

	int quit = FALSE;
	int adv_step = 0;
	int adv = FALSE;
	int band;

	double kTq = (in->Te[0] * kb / Q);

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
				    (Q / (kb * in->Tl[i])) * in->Nc[i] *
				    exp(((in->Fi[i] -
					  (-in->phi[i] - in->Xi[i]))) / (kTq));

			} else {
				dphidn =
				    get_dn_den(in->Fi[i] - in->Ec[i], in->Tl[i],
					       in->imat[i]);

			}

			double dphidp = 0.0;
			if (adv == FALSE) {
				dphidp =
				    -(Q / (kb * in->Tl[i])) * in->Nv[i] *
				    exp((((-in->phi[i] - in->Xi[i] -
					   in->Eg[i]) - in->Fi[i])) / (kTq));
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

			dphic_d += -Q * (dphidn - dphidp);	// just put in the _d to get it working again.

			//if (adv==TRUE)
			//{
			//      for (band=0;band<in->srh_bands;band++)
			//      {
			//              dphic_d+=(-q*(in->dnt[i][band]-in->dpt[i][band]));
			//      }
			//}

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
				b[i] = -(dphi - Q * (in->n[i] - in->p[i] - in->Nad[i]));	//
				if (adv == TRUE) {
					for (band = 0; band < in->srh_bands;
					     band++) {
						b[i] +=
						    -(-Q *
						      (in->nt[i][band] -
						       in->pt[i][band]));
					}
				}
			}
			//in->n[i]=in->Nc[i]*exp(((in->Fi[i]-in->Ec[i])*q)/(kb*in->Tl[i]));

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
				//printf("%d\n",get_clamp_state());

				double clamp_temp = 300.0;
				update =
				    b[i] / (1.0 +
					    fabs(b[i] / in->posclamp /
						 (clamp_temp * kb / Q)));
				in->phi[i] += update;

				//printf("%le %le\n",i,b[i]);
			}
		}

		//getchar();

		for (i = 0; i < in->ymeshpoints; i++) {
			in->Ec[i] = -in->phi[i] - in->Xi[i];
			in->Ev[i] = -in->phi[i] - in->Xi[i] - in->Eg[i];

			if (adv == FALSE) {
				in->n[i] =
				    in->Nc[i] *
				    exp(((in->Fi[i] -
					  in->Ec[i]) * Q) / (kb * in->Tl[i]));
				in->dn[i] =
				    (Q / (kb * in->Tl[i])) * in->Nc[i] *
				    exp((Q * (in->Fi[i] - in->Ec[i])) /
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
					  in->tp[i]) * Q) / (kb * in->Tl[i]));
				in->dp[i] =
				    (Q / (kb * in->Tl[i])) * in->Nv[i] *
				    exp(((in->xp[i] -
					  in->tp[i]) * Q) / (kb * in->Tl[i]));
			} else {
				in->p[i] =
				    get_p_den(in->xp[i] - in->tp[i], in->Tl[i],
					      in->imat[i]);
				in->dp[i] =
				    get_dp_den(in->xp[i] - in->tp[i], in->Tl[i],
					       in->imat[i]);
			}

			for (band = 0; band < in->srh_bands; band++) {
				//printf("%lf %lf\n",in->xpt[i][band],in->Fpt[i][band]);
				//getchar();

				in->Fnt[i][band] =
				    -in->phi[i] - in->Xi[i] +
				    dos_srh_get_fermi_n(in->n[i], in->p[i],
							band, in->imat[i],
							in->Te[i]);
				in->Fpt[i][band] =
				    -in->phi[i] - in->Xi[i] - in->Eg[i] -
				    dos_srh_get_fermi_p(in->n[i], in->p[i],
							band, in->imat[i],
							in->Th[i]);

				in->xt[i][band] = in->phi[i] + in->Fnt[i][band];
				in->nt[i][band] =
				    get_n_pop_srh(in->xt[i][band] + in->tt[i],
						  in->Te[i], band, in->imat[i]);
				in->dnt[i][band] =
				    get_dn_pop_srh(in->xt[i][band] + in->tt[i],
						   in->Te[i], band,
						   in->imat[i]);

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
		//#ifdef print_newtonerror

		if (get_dump_status(dump_print_pos_error) == TRUE)
			printf_log("%d Pos error = %e %d\n", ittr, error, adv);
		//#endif

#ifdef dump_converge

		/*in->converge=fopena(in->outputpath,"converge.dat","a");
		   fprintf(in->converge,"%e\n",error);
		   fclose(in->converge); */
#endif

//double N=2.0*pow(((2.0*pi*kb*in->Tl[0]*in->me[0]*m0)/(hp*hp)),1.5);
//double test=N*exp((-3.000000e-03*Q)/(kb*in->Tl[0]));
//printf("Check now %e %e\n",get_n_den(-3.000000e-03,in->Tl[0],in->me[0],in->dostype[0],dos_all),test);
//getchar();

//getchar();
		ittr++;

		if (adv == TRUE) {
			adv_step++;
		}

		if (ittr > pos_max_ittr) {
			quit = TRUE;
		}

		if ((error < min_pos_error) && (adv_step > 3)) {
			quit = TRUE;
		}

	} while (quit == FALSE);
	//getchar();

	pos_dump(in);

	update_arrays(in);

	if (in->srh_sim == TRUE) {
		time_init(in);

	}

	in->odes += in->ymeshpoints;

	for (i = 0; i < in->ymeshpoints; i++) {
		in->nf_save[i] = in->n[i];
		in->pf_save[i] = in->p[i];
		in->nt_save[i] = 0.0;	//in->nt[i];
		in->pt_save[i] = 0.0;	//in->pt[i];
	}

	free(Ti);
	free(Tj);
	free(Tx);
	free(b);

	printf_log("Solved pos\n");
	printf_log("Vl=%le Vr=%le phi_mid=%le\n", in->Vl, in->Vr,
		   in->phi[in->ymeshpoints / 2]);

	return 0;
}
