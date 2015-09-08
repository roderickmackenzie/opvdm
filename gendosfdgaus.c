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
#include <openssl/md5.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef windows
#include <zlib.h>
#endif

#include "code_ctrl.h"
#include "sim.h"
#include "inp.h"
#include "util.h"
#include "buffer.h"

static int unused __attribute__ ((unused));

#include "server.h"
#include "dump.h"
#ifdef dos_debug
#define test_dist

double pick[1000];
double pick_x[1000];
double pick_start = -7;
double pick_stop = 0;
double pick_dx;
#endif

static int gendos = TRUE;
static struct dosconfig confige[10];
static struct dosconfig configh[10];

void dos_double_res()
{
	int i;
	for (i = 1; i <= 80; i++) {
		printf("p srhbandp.inp %d 1\n", i);
	}
}

void md5write(char *file_name)
{

	FILE *file;
	char *buffer;
	unsigned long len;
	long l;
	char chkfile[100];
	char temp_path[1000];
	sprintf(temp_path, "%s/%s", sim_input_path(), file_name);
	sprintf(chkfile, "md5.%s.dat", file_name);

	inp_read_buffer(&buffer, &l, temp_path);
	len = (unsigned int)l;

	unsigned char md5[16];
	MD5((const unsigned char *)buffer, len, md5);
	char temp[100];
	sprintf(temp, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x", md5[0], md5[1],
		md5[2], md5[3], md5[4], md5[5], md5[6], md5[7], md5[8], md5[9],
		md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]);

	free(buffer);

	file = fopen(chkfile, "w");
	if (file == NULL) {
		ewe("File %s not found\n", chkfile);
	}
	fprintf(file, "%s\n", temp);
	fclose(file);
}

int md5check(char *file_name)
{

	FILE *file;
	char *buffer;
	unsigned long len;
	unsigned char md5[100];
	char chkfile[100];
	char newcheck[100];
	char fromfile[100];
	char temp_path[1000];
	sprintf(temp_path, "%s/%s", sim_input_path(), file_name);
	sprintf(chkfile, "md5.%s.dat", file_name);
	long l;
	inp_read_buffer(&buffer, &l, temp_path);

	len = (unsigned int)l;
	MD5((const unsigned char *)buffer, len, md5);
	free(buffer);

	strcpy(newcheck, "hello");

	sprintf(newcheck, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x", md5[0], md5[1],
		md5[2], md5[3], md5[4], md5[5], md5[6], md5[7], md5[8], md5[9],
		md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]);

	file = fopen(chkfile, "r");

	if (!file) {
		return FALSE;
	}

	unused = fscanf(file, "%s\n", fromfile);
	fclose(file);

	if (strcmp(newcheck, fromfile) == 0) {
		return TRUE;
	} else {
		return FALSE;
	}

	return 0;
}

void dump_qe()
{
	struct istruct n;
	struct istruct p;

	inter_load(&n, "./dosoutn.dat");
	inter_load(&p, "./dosoutp.dat");
	inter_swap(&p);

	double start = n.x[0];
	double stop = n.x[n.len - 1];
	double step = 5e-3;
	double pos = 0.0;
	double hf = 0.0;
	double hf_step = 1e-2;
	double re = 0.0;
	double rh = 0.0;
	double rhored = 0.0;
	double rhored_tot = 0.0;

	pos = start;
	double max = 0.0;
	FILE *qe = fopen("qe.dat", "w");
	hf_step = 1e-1;
	do {
		pos = start;
		rhored_tot = 0.0;
		do {
			re = inter_get(&n, pos + hf);
			rh = inter_get(&p, pos);
			rhored = re * rh;

			rhored_tot += rhored;
			pos += step;
		} while (pos < stop);

		if (rhored_tot > max)
			max = rhored_tot;

		hf += hf_step;
		printf("%e\n", hf);
	} while (hf < 2.0);

	hf = 0;
	hf_step = 1e-2;

	do {
		pos = start;
		rhored_tot = 0.0;
		do {
			re = inter_get(&n, pos + hf);
			rh = inter_get(&p, pos);
			rhored = re * rh;
			rhored_tot += rhored;

			pos += step;
		} while (pos < stop);

		fprintf(qe, "%e %e\n", hf, rhored_tot / max);
		hf += hf_step;
		printf("%e\n", hf);
	} while (hf < 2.0);

	fclose(qe);
	inter_free(&n);
	inter_free(&p);
}

void pick_init()
{
#ifdef dos_debug
	int i;
	pick_dx = (pick_stop - pick_start) / 1000.0;
	double pos = pick_start;
	for (i = 0; i < 1000; i++) {
		pick[i] = 0.0;
		pick_x[i] = pos;
		pos += pick_dx;
	}
#endif
}

void pick_add(double x, double value)
{
#ifdef dos_debug
	int pos;
	for (pos = 0; pos < 1000; pos++) {
		if (pick_x[pos] > x)
			break;
	}
	if (pos < 0.0)
		return;
	if (pos > 999)
		return;
	if (value > pick[pos])
		pick[pos] = value;
#endif
}

void pick_dump()
{
#ifdef dos_debug
	int i;
	FILE *out;
	out = fopen("./gaus.dat", "w");
	double pos = pick_start;
	for (i = 0; i < 1000; i++) {
		fprintf(out, "%e %e\n", pick_x[i], pick[i]);
		pos += pick_dx;
	}
	fclose(out);
#endif
}

void gen_do(struct dosconfig *in, struct dosconfig *in2, char *outfile,
	    int electrons, int mat)
{
	char name[100];
	char temp[1000];

#ifdef dos_test_stats
	FILE *freetest;
	if (electrons == TRUE) {
		sprintf(name, "./freetestn%d.dat", mat);
		freetest = fopen(name, "w");
		fclose(freetest);
	} else {
		sprintf(name, "./freetestp%d.dat", mat);
		freetest = fopen(name, "w");
		fclose(freetest);
	}
#endif

	double tstart = 0.0;
	double tstop = 0.0;
	double tsteps = 0.0;
	double dt = 0.0;
	double xpos = 0.0;
	double tpos = 0.0;
	int t = 0;
	int x = 0;
	int band;
	double *srh_r1 = (double *)malloc(sizeof(double) * in->srh_bands);
	double *srh_r2 = (double *)malloc(sizeof(double) * in->srh_bands);
	double *srh_r3 = (double *)malloc(sizeof(double) * in->srh_bands);
	double *srh_r4 = (double *)malloc(sizeof(double) * in->srh_bands);
	double *srh_n = (double *)malloc(sizeof(double) * in->srh_bands);
	double *srh_den = (double *)malloc(sizeof(double) * in->srh_bands);
	double *srh_dE_sum = (double *)malloc(sizeof(double) * in->srh_bands);
	double *srh_read = (double *)malloc(sizeof(double) * in->srh_bands);

	double *srh_x = (double *)malloc(sizeof(double) * in->srh_bands);
	double *srh_mid = (double *)malloc(sizeof(double) * in->srh_bands);
	double *band_E_mesh = (double *)malloc(sizeof(double) * in->Esteps);
	int *band_i = (int *)malloc(sizeof(int) * in->Esteps);

	int e = 0;
	double E = 0.0;
	double dE = fabs(in->srh_start) / ((double)in->Esteps);
	double rho = 0.0;
	double rho2 = 0.0;
	double sum = 0.0;
	double f = 0.0;
	double last_n0 = 0;
	double *xmesh = NULL;
	tstart = in->Tstart;
	tstop = in->Tstop;
	tsteps = in->Tsteps;
	dt = (tstop - tstart) / tsteps;

	double dxr = (in->nstop - in->nstart) / ((double)in->npoints);

	xpos = in->nstart;
	tpos = tstart;
	t = 0;
	x = 0;
#ifdef dos_bin
	int buf_len = 0;
	buf_len += 13;
	buf_len += in->npoints;
	buf_len += tsteps;
	buf_len += in->srh_bands;
	buf_len += in->srh_bands;
	buf_len += tsteps * in->npoints * 2;
	buf_len += tsteps * in->npoints * 5 * in->srh_bands;

	int buf_pos = 0;
	double *buf = (double *)malloc(buf_len * sizeof(double));
	buf[buf_pos++] = (double)in->npoints;
	buf[buf_pos++] = (double)tsteps;
	buf[buf_pos++] = (double)in->srh_bands;
	buf[buf_pos++] = (double)in->epsilonr;
	buf[buf_pos++] = (double)in->doping;
	buf[buf_pos++] = (double)in->mu;
	buf[buf_pos++] = (double)in->srh_vth;
	buf[buf_pos++] = (double)in->srh_sigman;
	buf[buf_pos++] = (double)in->srh_sigmap;
	buf[buf_pos++] = (double)in->Nc;
	buf[buf_pos++] = (double)in->Nv;
	buf[buf_pos++] = (double)in->Eg;
	buf[buf_pos++] = (double)in->Xi;
#else
	FILE *out;
	out = fopen(outfile, "w");
	fprintf(out, "%d %d %d %lf %le %le %le %le %le %le %le %le %le\n",
		(int)in->npoints, (int)tsteps, in->srh_bands, in->epsilonr,
		in->doping, in->mu, in->srh_vth, in->srh_sigman, in->srh_sigmap,
		in->Nc, in->Nv, in->Eg, in->Xi);
#endif

	double srh_pos = in->srh_start;
	double srh_delta = fabs(in->srh_start) / (double)(in->srh_bands);
	xmesh = (double *)malloc(sizeof(double) * in->npoints);

	for (x = 0; x < in->npoints; x++) {
#ifdef dos_bin
		buf[buf_pos++] = xpos;
#else
		fprintf(out, "%e\n", xpos);
#endif
		xmesh[x] = xpos;
		xpos += dxr;
	}

	for (t = 0; t < tsteps; t++) {
#ifdef dos_bin
		buf[buf_pos++] = tpos;
#else
		fprintf(out, "%e\n", tpos);
#endif
		tpos += dt;
	}

	for (band = 0; band < in->srh_bands; band++) {
		srh_pos += srh_delta / 2.0;
		srh_mid[band] = srh_pos;
#ifdef dos_bin
		buf[buf_pos++] = srh_pos;
#else
		fprintf(out, "%e\n", srh_pos);
#endif

		srh_pos += srh_delta / 2.0;
		srh_x[band] = srh_pos;

	}

	int i;
	int band_pos = 0;
	int cur_band = 0;
	int points_per_band = in->Esteps / in->srh_bands;
	double pos = in->srh_start;

	for (i = 0; i < in->Esteps; i++) {
		band_E_mesh[i] = pos;
		band_i[i] = cur_band;

		pos += dE;
		band_pos++;
		if (band_pos >= points_per_band) {
			band_pos = 0;
			cur_band++;
		}

	}

	tpos = tstart;

	if (in->dostype == dos_read) {
		FILE *dosread;
		if (electrons == TRUE) {
			sprintf(name, "./srhbandn%d.dat", mat);
			dosread = fopen(name, "r");
		} else {
			sprintf(name, "./srhbandp%d.dat", mat);
			dosread = fopen(name, "r");
		}

		if (dosread == NULL) {
			ewe("can not open srhband file\n");
		}

		for (band = 0; band < in->srh_bands; band++) {
			unused = fscanf(dosread, "%le", &(srh_read[band]));
			printf("%le\n", srh_read[band]);
			srh_read[band] = fabs(srh_read[band]);
		}

		fclose(dosread);
	}
#ifdef test_dist
	FILE *munfile = fopen("./munfile.dat", "w");
	if (munfile == NULL) {
		ewe("problem\n");
	}

	FILE *plotbands;
	plotbands = fopen("./plotbandsn.dat", "w");

	FILE *plotbandsfree;
	plotbandsfree = fopen("./plotbandsfreen.dat", "w");

	FILE *rod = fopen("./gau_test_n.dat", "w");

#endif

#ifdef test_dist
	int first = FALSE;
#endif

	struct buffer dos_out;
	buffer_init(&dos_out);

	if (get_dump_status(dump_write_out_band_structure) == TRUE) {
		if (electrons == TRUE) {
			buffer_malloc(&dos_out);
			dos_out.y_mul = 1.0;
			dos_out.x_mul = 1.0;
			strcpy(dos_out.title, "Discretized density of states");
			strcpy(dos_out.type, "xy");
			strcpy(dos_out.x_label, "Energy");
			strcpy(dos_out.y_label, "Density of states");
			strcpy(dos_out.x_units, "$eV$");
			strcpy(dos_out.y_units, "$m^{-3}eV^{-1}$");
			dos_out.logscale_x = 0;
			dos_out.logscale_y = 0;
			buffer_add_info(&dos_out);
		} else {
			buffer_malloc(&dos_out);
			dos_out.y_mul = 1.0;
			dos_out.x_mul = 1.0;
			strcpy(dos_out.title, "Discretized density of states");
			strcpy(dos_out.type, "xy");
			strcpy(dos_out.x_label, "Energy");
			strcpy(dos_out.y_label, "Density of states");
			strcpy(dos_out.x_units, "$eV$");
			strcpy(dos_out.y_units, "$m^{-3}eV^{-1}$");
			dos_out.logscale_x = 0;
			dos_out.logscale_y = 0;
			buffer_add_info(&dos_out);
		}
	}

	int srh_band = 0;
	double srh_E = srh_mid[0];
	double srh_f = 0.0;
	double f2 = 0.0;
	double sum2 = 0.0;
	for (t = 0; t < tsteps; t++) {
		if (get_dump_status(dump_iodump) == TRUE)
			printf("%d/%d\n", t, (int)tsteps);

		for (x = 0; x < in->npoints; x++) {
			xpos = xmesh[x];
			E = 0.0;
			sum = 0.0;
			sum2 = 0.0;

			for (srh_band = 0; srh_band < (in->srh_bands);
			     srh_band++) {
				srh_r1[srh_band] = 0.0;
				srh_r2[srh_band] = 0.0;
				srh_r3[srh_band] = 0.0;
				srh_r4[srh_band] = 0.0;
				srh_n[srh_band] = 0.0;
				srh_den[srh_band] = 0.0;
				srh_dE_sum[srh_band] = 0.0;
			}

			srh_band = 0;

			for (e = 0; e < in->Esteps; e++) {
				E = band_E_mesh[e];
				srh_band = band_i[e];
				srh_E = srh_mid[srh_band];

				f = 1.0 / (1.0 +
					   exp((E - xpos) * Q / (kb * tpos)));
				f2 = 1.0 / (1.0 +
					    exp((E -
						 (-1.0 * (in->Eg + xpos))) * Q /
						(kb * tpos)));

				srh_f =
				    1.0 / (1.0 +
					   exp((srh_E -
						xpos) * Q / (kb * tpos)));

				if (in->dostype == dos_fd) {
					if (E > 0) {
						rho =
						    (sqrt(E) /
						     (4.0 * PI * PI)) *
						    pow((2.0 * in->m * m0) /
							(hbar * hbar),
							3.0 / 2.0);
					} else {
						rho = 0.0;
					}

				} else if (in->dostype == dos_exp_fd) {

					if (E > 0) {
						rho =
						    2.0 * (sqrt(E) /
							   (4.0 * PI * PI)) *
						    pow((2.0 * in->m * m0) /
							(hbar * hbar),
							3.0 / 2.0);
						rho2 =
						    2.0 * (sqrt(E) /
							   (4.0 * PI * PI)) *
						    pow((2.0 * in2->m * m0) /
							(hbar * hbar),
							3.0 / 2.0);
					} else {
						rho =
						    in->Nt * exp((E) /
								 (in->Et));
						rho2 =
						    in2->Nt * exp((E) /
								  (in2->Et));
					}

				} else if (in->dostype == dos_exp) {

					rho = in->Nt * exp((E) / (in->Et));
					rho2 = in2->Nt * exp((E) / (in2->Et));

					if ((E > in->del_start)
					    && (E < in->del_stop)) {
						rho = 1e20;
					}

				}

				if (x == 0) {
					pick_add(E - in->Xi, rho);
					if (get_dump_status
					    (dump_write_out_band_structure) ==
					    TRUE) {
						if (E > in->srh_start) {
							if (electrons == TRUE) {
								sprintf(temp,
									"%le %le\n",
									E -
									in->Xi,
									rho);
								buffer_add_string
								    (&dos_out,
								     temp);
							} else {
								sprintf(temp,
									"%le %le\n",
									-E -
									in->Xi -
									in->Eg,
									rho);
								buffer_add_string
								    (&dos_out,
								     temp);
							}

						}
					}
				}

				if (E > 0) {
					sum += rho * f * dE;
					sum2 += rho2 * f2 * dE;

				} else {

					if (electrons == TRUE) {
						srh_r1[srh_band] +=
						    in->srh_vth *
						    in->srh_sigman * rho *
						    (1.0 - srh_f) * dE;
						srh_r2[srh_band] +=
						    in->srh_vth *
						    in->srh_sigman * in->Nc *
						    exp((srh_E * Q) /
							(tpos * kb)) * rho *
						    srh_f * dE;
						srh_r3[srh_band] +=
						    in->srh_vth *
						    in->srh_sigmap * rho *
						    srh_f * dE;
						srh_r4[srh_band] +=
						    in->srh_vth *
						    in->srh_sigmap * in->Nv *
						    exp((-in->Eg * Q -
							 srh_E * Q) / (tpos *
								       kb)) *
						    rho * (1.0 - srh_f) * dE;
						srh_n[srh_band] +=
						    rho * srh_f * dE;
						srh_den[srh_band] += rho * dE;
						srh_dE_sum[srh_band] += dE;
					} else {
						srh_r1[srh_band] +=
						    in->srh_vth *
						    in->srh_sigmap * rho *
						    (1.0 - srh_f) * dE;
						srh_r2[srh_band] +=
						    in->srh_vth *
						    in->srh_sigmap * in->Nv *
						    exp((srh_E * Q) /
							(tpos * kb)) * rho *
						    srh_f * dE;
						srh_r3[srh_band] +=
						    in->srh_vth *
						    in->srh_sigman * rho *
						    srh_f * dE;
						srh_r4[srh_band] +=
						    in->srh_vth *
						    in->srh_sigman * in->Nc *
						    exp((-in->Eg * Q -
							 (srh_E * Q)) / (tpos *
									 kb)) *
						    rho * (1.0 - srh_f) * dE;
						srh_n[srh_band] +=
						    rho * srh_f * dE;
						srh_den[srh_band] += rho * dE;
						srh_dE_sum[srh_band] += dE;
					}
				}

#ifdef test_dist
				if (E >= 0.0) {

					if (t == 0)
						if ((x % 10) == 0)
							if ((e % 2) == 0) {
								fprintf
								    (plotbandsfree,
								     "%.20le %.20le\n",
								     E,
								     rho * f *
								     dE);
								if (first ==
								    FALSE)
									fprintf
									    (plotbands,
									     "%.20le %.20le\n",
									     E,
									     rho
									     *
									     f *
									     dE);
								first = TRUE;
							}

				} else {

					if (t == 0)
						if ((x % 10) == 0)
							if ((e % 10) == 0) {
								fprintf
								    (plotbands,
								     "%.20le %.20le\n",
								     E,
								     rho * f *
								     dE);
							}

				}
#endif

			}

#ifdef test_dist
			fprintf(plotbands, "\n");
			fprintf(plotbandsfree, "\n");
#endif

			if (x == 0) {
				for (band = 0; band < in->srh_bands; band++) {
#ifdef dos_bin
					buf[buf_pos++] = srh_den[band];
#else
					fprintf(out, "%le\n", srh_den[band]);
#endif
				}

				if (get_dump_status(dump_band_structure) ==
				    TRUE) {
					FILE *bandsdump;
					if (electrons == TRUE) {
						bandsdump =
						    fopen("./lumo_out.dat",
							  "w");
						for (band = 0;
						     band < in->srh_bands;
						     band++) {
							fprintf(bandsdump,
								"%le\n",
								srh_den[band] /
								srh_dE_sum
								[band]);
						}
						fclose(bandsdump);
					} else {
						bandsdump =
						    fopen("./homo_out.dat",
							  "w");
						for (band = 0;
						     band < in->srh_bands;
						     band++) {
							fprintf(bandsdump,
								"%le\n",
								srh_den[band] /
								srh_dE_sum
								[band]);
						}
						fclose(bandsdump);
					}
				}
			}

			if ((in->dostype == dos_exp)
			    || (in->dostype == dos_read)
			    || (in->dostype == dos_an)) {
				if (electrons == TRUE) {
					sum =
					    in->Nc * exp((xpos * Q) /
							 (kb * tpos));
					sum2 =
					    in->Nv *
					    exp((-(in->Eg + xpos) * Q) /
						(kb * tpos));
				} else {
					sum =
					    in->Nv * exp((xpos * Q) /
							 (kb * tpos));
					sum2 =
					    in->Nc *
					    exp((-(in->Eg + xpos) * Q) /
						(kb * tpos));
				}

			}

			double w0 = sum / ((sum - last_n0) / (dxr));
			if (x == 0)
				w0 = kb * tpos / Q;
#ifdef dos_bin
			buf[buf_pos++] = sum;
			buf[buf_pos++] = w0;
#else
			fprintf(out, "%.20le %.20le ", sum, w0);
#endif
			last_n0 = sum;

#ifdef dos_test_stats
			if (electrons == TRUE) {
				freetest = fopen("./freetestn.dat", "a");

				fprintf(freetest, "%.20le %.20le %.20le\n",
					xpos, (3.0 / 2.0) * w0,
					(3.0 / 2.0) * kb * tpos / Q);
				fclose(freetest);
			} else {
				freetest = fopen("./freetestp.dat", "a");

				fprintf(freetest, "%.20le %.20le %.20le\n",
					xpos, (3.0 / 2.0) * w0,
					(3.0 / 2.0) * kb * tpos / Q);
				fclose(freetest);
			}
#endif

			for (srh_band = 0; srh_band < in->srh_bands; srh_band++) {
#ifdef dos_bin
				buf[buf_pos++] = srh_r1[srh_band];
				buf[buf_pos++] = srh_r2[srh_band];
				buf[buf_pos++] = srh_r3[srh_band];
				buf[buf_pos++] = srh_r4[srh_band];
				buf[buf_pos++] = srh_n[srh_band];
#else
				fprintf(out,
					"%.20le %.20le %.20le %.20le %.20le ",
					srh_r1[srh_band], srh_r2[srh_band],
					srh_r3[srh_band], srh_r4[srh_band],
					srh_n[srh_band]);
#endif
			}

#ifndef dos_bin
			fprintf(out, "\n");
#endif

#ifdef test_dist
			fprintf(rod, "\n");

#endif

		}

		tpos += dt;
	}

#ifdef test_dist
	fclose(rod);
	fclose(plotbands);
	fclose(plotbandsfree);
	fclose(munfile);
#endif

	if (get_dump_status(dump_write_out_band_structure) == TRUE) {
		if (electrons == TRUE) {
			sprintf(name, "./dosoutn%d.dat", mat);
		} else {
			sprintf(name, "./dosoutp%d.dat", mat);
		}

		buffer_dump("./", name, &dos_out);
		buffer_free(&dos_out);

	}
#ifdef dos_bin
	if (buf_len != buf_pos) {
		ewe("Expected dos size is different from generated\n");
	}
#ifndef windows
	gzFile file;
	file = gzopen(outfile, "w9b");
	gzwrite(file, (char *)buf, buf_len * sizeof(double));
	gzclose(file);
#else
	FILE *file;
	file = fopen(outfile, "wb");
	fwrite((char *)buf, buf_len * sizeof(double), 1, file);
	fclose(file);
#endif
	FILE *yes;
	yes = fopen(outfile, "ab");
	int temp1 = buf_len * sizeof(double);
	fwrite((char *)&temp1, sizeof(int), 1, yes);
	fclose(yes);

	free(buf);
#else
	fclose(out);
#endif
	free(xmesh);
	free(srh_r1);
	free(srh_r2);
	free(srh_r3);
	free(srh_r4);
	free(srh_n);
	free(srh_den);
	free(srh_dE_sum);
	free(srh_read);

	free(band_E_mesh);
	free(band_i);

	free(srh_x);
	free(srh_mid);

	return;
}

void gen_dos_fd_gaus_n(int mat)
{
	char temp[100];
	if (get_dump_status(dump_iodump) == TRUE)
		printf("Electrons.... %d\n", mat);
	sprintf(temp, "dosn%d.dat", mat);
	gen_do(&confige[mat], &configh[mat], temp, TRUE, mat);
}

void gen_dos_fd_gaus_p(int mat)
{
	char temp[100];
	if (get_dump_status(dump_iodump) == TRUE)
		printf("Holes.... %d\n", mat);
	sprintf(temp, "dosp%d.dat", mat);
	gen_do(&configh[mat], &confige[mat], temp, FALSE, mat);
}

void gen_load_dos(int mat)
{
	char file_name[100];
	char temp[100];

	sprintf(file_name, "dos%d.inp", mat);
	struct inp_file inp;
	inp_init(&inp);
	inp_load_from_path(&inp, "./", file_name);
	inp_check(&inp, 1.2);

	inp_search_string(&inp, temp, "#dostype");
	confige[mat].dostype = english_to_bin(temp);
	configh[mat].dostype = confige[mat].dostype;

	inp_search_double(&inp, &(confige[mat].m), "#me");
	inp_search_double(&inp, &(configh[mat].m), "#mh");

	inp_search_double(&inp, &(confige[mat].Nt), "#Ntrape");
	inp_search_double(&inp, &(configh[mat].Nt), "#Ntraph");

	confige[mat].Nt = fabs(confige[mat].Nt);
	configh[mat].Nt = fabs(configh[mat].Nt);

	if (confige[mat].Nt < 1e7)
		confige[mat].Nt = 1e7;
	if (configh[mat].Nt < 1e7)
		configh[mat].Nt = 1e7;

	inp_search_double(&inp, &(confige[mat].Et), "#Etrape");
	inp_search_double(&inp, &(configh[mat].Et), "#Etraph");

	confige[mat].Et = fabs(confige[mat].Et);
	configh[mat].Et = fabs(configh[mat].Et);

	if (confige[mat].Et < 2e-3)
		confige[mat].Et = 2e-3;
	if (configh[mat].Et < 2e-3)
		configh[mat].Et = 2e-3;

	if (confige[mat].Et > 200e-3)
		confige[mat].Et = 200e-3;
	if (configh[mat].Et > 200e-3)
		configh[mat].Et = 200e-3;

	inp_search_int(&inp, &(gendos), "#gendos");

	inp_search_double(&inp, &(confige[mat].mu), "#mueffe");
	inp_search_double(&inp, &(configh[mat].mu), "#mueffh");

	confige[mat].mu = fabs(confige[mat].mu);
	configh[mat].mu = fabs(configh[mat].mu);

	inp_search_double(&inp, &(confige[mat].epsilonr), "#epsilonr");
	confige[mat].epsilonr = fabs(confige[mat].epsilonr);
	hard_limit("#epsilonr", &(confige[mat].epsilonr));

	confige[mat].epsilonr = fabs(confige[mat].epsilonr);
	configh[mat].epsilonr = fabs(confige[mat].epsilonr);

	inp_search_double(&inp, &(confige[mat].doping), "#doping");
	configh[mat].doping = confige[mat].doping;

	inp_search_double(&inp, &(confige[mat].Tstart), "#Tstart");
	inp_search_double(&inp, &(confige[mat].Tstop), "#Tstop");
	inp_search_int(&inp, &(confige[mat].Tsteps), "#Tpoints");

	configh[mat].Tstart = confige[mat].Tstart;
	configh[mat].Tstop = confige[mat].Tstop;
	configh[mat].Tsteps = confige[mat].Tsteps;

	inp_search_double(&inp, &(confige[mat].nstart), "#nstart");
	inp_search_double(&inp, &(confige[mat].nstop), "#nstop");
	inp_search_int(&inp, &(confige[mat].npoints), "#npoints");

	inp_search_double(&inp, &(configh[mat].nstart), "#pstart");
	inp_search_double(&inp, &(configh[mat].nstop), "#pstop");
	inp_search_int(&inp, &(configh[mat].npoints), "#ppoints");
	int bands = 0;
	inp_search_int(&inp, &(bands), "#srh_bands");
	confige[mat].srh_bands = bands;
	configh[mat].srh_bands = bands;

	inp_search_double(&inp, &(confige[mat].srh_start), "#srh_start");
	configh[mat].srh_start = confige[mat].srh_start;

	inp_search_double(&inp, &(confige[mat].srh_sigman), "#srhsigman_e");
	confige[mat].srh_sigman = fabs(confige[mat].srh_sigman);

	inp_search_double(&inp, &(confige[mat].srh_sigmap), "#srhsigmap_e");
	confige[mat].srh_sigmap = fabs(confige[mat].srh_sigmap);

	inp_search_double(&inp, &(confige[mat].srh_vth), "#srhvth_e");
	confige[mat].srh_vth = fabs(confige[mat].srh_vth);
	if (confige[mat].srh_vth < 1e2)
		confige[mat].srh_vth = 1e2;

	inp_search_double(&inp, &(configh[mat].srh_sigman), "#srhsigman_h");
	configh[mat].srh_sigman = fabs(configh[mat].srh_sigman);

	inp_search_double(&inp, &(configh[mat].srh_sigmap), "#srhsigmap_h");
	configh[mat].srh_sigmap = fabs(configh[mat].srh_sigmap);

	inp_search_double(&inp, &(configh[mat].srh_vth), "#srhvth_h");
	configh[mat].srh_vth = fabs(configh[mat].srh_vth);
	if (configh[mat].srh_vth < 1e2)
		configh[mat].srh_vth = 1e2;

	inp_search_double(&inp, &(confige[mat].Nc), "#Nc");

	inp_search_double(&inp, &(confige[mat].Nv), "#Nv");

	inp_search_double(&inp, &(confige[mat].srh_cut), "#srh_cut");
	confige[mat].srh_cut = -fabs(confige[mat].srh_cut);
	configh[mat].srh_cut = confige[mat].srh_cut;

	inp_search_double(&inp, &(confige[mat].del_start), "#lumodelstart");

	inp_search_double(&inp, &(confige[mat].del_stop), "#lumodelstop");

	inp_search_double(&inp, &(configh[mat].del_start), "#homodelstart");

	inp_search_double(&inp, &(configh[mat].del_stop), "#homodelstop");

	inp_search_double(&inp, &(confige[mat].Xi), "#Xi");

	inp_search_double(&inp, &(confige[mat].Eg), "#Eg");
	confige[mat].Eg = fabs(confige[mat].Eg);
	hard_limit("#Eg", &(confige[mat].Eg));

	inp_search_double(&inp, &(confige[mat].gaus_mull), "#gaus_mull");
	configh[mat].gaus_mull = confige[mat].gaus_mull;

	int Esteps = 0;
	inp_search_int(&inp, &(Esteps), "#Esteps");
	int Estep_div = (Esteps / bands) * bands;
	if (Estep_div != Esteps) {
		printf("Esteps wanted= %d, given= %d \n", Esteps, Estep_div);
	}

	confige[mat].Esteps = Estep_div;
	configh[mat].Esteps = Estep_div;
	int dump;
	inp_search_int(&inp, &dump, "#dump_band_structure");
	set_dump_status(dump_band_structure, dump);

	inp_free(&inp);

	configh[mat].Xi = confige[mat].Xi;
	configh[mat].Eg = confige[mat].Eg;

	confige[mat].Nc = fabs(confige[mat].Nc);
	confige[mat].Nv = fabs(confige[mat].Nv);
	if (confige[mat].Nc < 1e16)
		confige[mat].Nc = 1e16;
	if (confige[mat].Nv < 1e16)
		confige[mat].Nv = 1e16;
	confige[mat].m =
	    pow(confige[mat].Nc / 2.0,
		2.0 / 3.0) * hp * hp / kb / 300.0 / m0 / 2.0 / PI;
	configh[mat].m =
	    pow(confige[mat].Nv / 2.0,
		2.0 / 3.0) * hp * hp / kb / 300.0 / m0 / 2.0 / PI;

	configh[mat].Nc = confige[mat].Nc;
	configh[mat].Nv = confige[mat].Nv;

}

void gen_dos_fd_gaus_fd()
{

	char name[100];

	int file_bandn = FALSE;
	int file_bandp = FALSE;
	int file_dos = FALSE;
	int launch_server = FALSE;

	FILE *file;
	int mat = 0;
	int problem_with_dos = FALSE;
	file_bandn = FALSE;
	file_bandp = FALSE;
	file_dos = FALSE;

	pick_init(mat);
	gen_load_dos(mat);

	problem_with_dos = FALSE;
	sprintf(name, "dos%d.inp", mat);

	if (md5check(name) == FALSE)
		problem_with_dos = TRUE;

	sprintf(name, "dosn%d.dat", mat);
	file = fopen(name, "r");
	if (!file) {
		problem_with_dos = TRUE;
	} else {
		fclose(file);
	}
	sprintf(name, "dosp%d.dat", mat);
	file = fopen(name, "r");
	if (!file) {
		problem_with_dos = TRUE;
	} else {
		fclose(file);
	}

	if (problem_with_dos == TRUE) {
		file_dos = TRUE;
		file_bandn = TRUE;
		file_bandp = TRUE;
		launch_server = TRUE;
	}

	if (confige[mat].dostype == dos_read) {
		sprintf(name, "srhbandn%d.inp", mat);
		if (md5check(name) == FALSE) {
			file_bandn = TRUE;
			launch_server = TRUE;
		}

		sprintf(name, "srhbandp%d.inp", mat);
		if (md5check(name) == FALSE) {
			file_bandp = TRUE;
			launch_server = TRUE;
		}
	}

	if ((gendos == TRUE) && (launch_server == TRUE)) {
		sprintf(name, "gendosn%d.inp", mat);

		if (file_bandn == TRUE)
			server_add_job(&globalserver, name, name);
		sprintf(name, "gendosp%d.inp", mat);
		if (file_bandp == TRUE)
			server_add_job(&globalserver, name, name);

		pick_dump();
		sprintf(name, "dos%d.inp", mat);
		if (file_dos == TRUE)
			md5write(name);

		if (confige[mat].dostype == dos_read) {
			sprintf(name, "srhbandn%d.inp", mat);
			safe_file(name);
			if (file_bandn == TRUE)
				md5write(name);

			sprintf(name, "srhbandp%d.inp", mat);
			safe_file(name);
			if (file_bandp == TRUE)
				md5write(name);
		}

	}

	print_jobs(&globalserver);

	server_run_jobs(&globalserver);
	printf("Finished generating....\n");

}
