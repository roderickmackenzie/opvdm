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
#include <zlib.h>
#include "code_ctrl.h"
#include "server.h"
#include "sim.h"
#include "dump.h"

#define dos_warn
double max = -1000;
double min = 1000;
static struct dos dosn[1];
static struct dos dosp[1];

double get_dos_filled_n(struct device *in)
{
	int i;
	int band;
	double n_tot = 0.0;
	double n_tot_den = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		for (band = 0; band < in->srh_bands; band++) {
			n_tot += in->nt[i][band];
			n_tot_den += dosn[in->imat[i]].srh_den[band];
		}
	}
	n_tot = (n_tot) / (n_tot_den);
	return n_tot;
}

double get_dos_filled_p(struct device *in)
{
	int i;
	int band;
	double p_tot = 0.0;
	double p_tot_den = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		for (band = 0; band < in->srh_bands; band++) {
			p_tot += in->pt[i][band];
			p_tot_den += dosp[in->imat[i]].srh_den[band];
		}
	}
	p_tot = (p_tot) / (p_tot_den);
	return p_tot;
}

double dos_srh_get_fermi_n(double n, double p, int band, int mat)
{
	double srh_sigman = dosn[mat].config.srh_sigman;
	double srh_sigmap = dosn[mat].config.srh_sigmap;
	double Nc = dosn[mat].config.Nc;
	double Nv = dosn[mat].config.Nv;
	double srh_vth = dosn[mat].config.srh_vth;

	double srh_en =
	    srh_vth * srh_sigman * Nc * exp((q * dosn[mat].srh_E[band]) /
					    (300.0 * kb));
	double srh_ep =
	    srh_vth * srh_sigmap * Nv *
	    exp((q * (-1.0 - dosn[mat].srh_E[band])) / (300.0 * kb));

	double f = 0.0;
	f = (n * srh_vth * srh_sigman + srh_ep) / (n * srh_vth * srh_sigman +
						   p * srh_vth * srh_sigmap +
						   srh_en + srh_ep);
	double level = 0.0;
	level = dosn[mat].srh_E[band] - 300.0 * kb * log((1.0 / f) - 1.0) / q;
	return level;
}

double dos_srh_get_fermi_p(double n, double p, int band, int mat)
{
	double srh_sigmap = dosp[mat].config.srh_sigmap;
	double srh_sigman = dosp[mat].config.srh_sigman;
	double Nc = dosp[mat].config.Nc;
	double Nv = dosp[mat].config.Nv;
	double srh_vth = dosp[mat].config.srh_vth;

	double srh_ep =
	    srh_vth * srh_sigmap * Nv * exp((q * dosp[mat].srh_E[band]) /
					    (300.0 * kb));
	double srh_en =
	    srh_vth * srh_sigman * Nc *
	    exp((q * (-1.0 - dosp[mat].srh_E[band])) / (300.0 * kb));
	double f = 0.0;
	f = (p * srh_vth * srh_sigmap + srh_en) / (p * srh_vth * srh_sigmap +
						   n * srh_vth * srh_sigman +
						   srh_ep + srh_en);
	double level = 0.0;
	level = dosp[mat].srh_E[band] - 300.0 * kb * log((1.0 / f) - 1.0) / q;
	return level;
}

void dos_init(int mat)
{
	dosn[mat].used = FALSE;
	dosp[mat].used = FALSE;
}

void dos_free_now(struct dos *mydos)
{
	int ii;
	int iii;
	if (mydos->used == TRUE) {
		free(mydos->x);
		free(mydos->t);
		free(mydos->srh_E);
		free(mydos->srh_den);

		for (ii = 0; ii < mydos->tlen; ii++) {
			for (iii = 0; iii < mydos->xlen; iii++) {
				free(mydos->srh_r1[ii][iii]);
				free(mydos->srh_r2[ii][iii]);
				free(mydos->srh_r3[ii][iii]);
				free(mydos->srh_r4[ii][iii]);
				free(mydos->srh_c[ii][iii]);

			}

			free(mydos->c[ii]);
			free(mydos->w[ii]);
			free(mydos->srh_r1[ii]);
			free(mydos->srh_r2[ii]);
			free(mydos->srh_r3[ii]);
			free(mydos->srh_r4[ii]);
			free(mydos->srh_c[ii]);
		}

		free(mydos->c);
		free(mydos->w);

		free(mydos->srh_r1);
		free(mydos->srh_r2);
		free(mydos->srh_r3);
		free(mydos->srh_r4);
		free(mydos->srh_c);

		mydos->xlen = 0;
		mydos->tlen = 0;
		mydos->srh_bands = 0;
	}

}

void dos_free(int mat)
{
	dos_free_now(&dosn[mat]);
	dos_free_now(&dosp[mat]);
}

double get_Nc_free(int mat)
{
	return dosn[mat].config.Nc;

}

double get_Nv_free(int mat)
{
	return dosp[mat].config.Nv;
}

void load_dos_file(struct dos *mydos, char *file)
{
#ifndef dos_bin
	double srh_r1 = 0.0;
	double srh_r2 = 0.0;
	double srh_r3 = 0.0;
	double srh_r4 = 0.0;
	double srh_c = 0.0;
	double w0;
	double n;
#endif
	mydos->used = TRUE;
	mydos->used = TRUE;

	if (get_dump_status(dump_print_text) == TRUE)
		printf("Loading %s\n", file);

#ifdef dos_bin
	gzFile in;
#else
	FILE *in;
#endif
	/*FILE *rt;
	   #ifdef dos_bin
	   if (strcmp(file,"./dosn.dat")==0)
	   {
	   rt = fopen("./rtbinn.inp", "w");
	   }else
	   {
	   rt = fopen("./rtbinp.inp", "w");
	   }
	   #else
	   if (strcmp(file,"./dosn.dat")==0)
	   {
	   rt = fopen("./rtnormn.inp", "w");
	   }else
	   {
	   rt = fopen("./rtnormp.inp", "w");
	   }
	   #endif */
#ifdef dos_bin

	int len;

	FILE *tl = fopen(file, "rb");
	fseek(tl, -4, SEEK_END);
	fread((char *)&len, 4, 1, tl);

	fclose(tl);

	in = gzopen(file, "rb");
	if (in == Z_NULL) {
		printf("DOS file not found\n");
		exit(0);
	}

	int buf_len = len / sizeof(double);

	double *buf = (double *)malloc(sizeof(double) * buf_len);

	int buf_pos = 0;

	gzread(in, (char *)buf, len);

	gzclose(in);

#else
	in = fopen(file, "r");
	if (in == NULL) {
		printf("DOS n file not found\n");
	}
#endif

	int t = 0;
	int x = 0;
	int srh_band = 0;
#ifdef dos_bin
	mydos->xlen = (int)buf[buf_pos++];
	mydos->tlen = (int)buf[buf_pos++];
	mydos->srh_bands = (int)buf[buf_pos++];
	mydos->config.epsilonr = buf[buf_pos++];
	mydos->config.doping = buf[buf_pos++];
	mydos->config.mu = buf[buf_pos++];
	mydos->config.srh_vth = buf[buf_pos++];
	mydos->config.srh_sigman = buf[buf_pos++];
	mydos->config.srh_sigmap = buf[buf_pos++];
	mydos->config.Nc = buf[buf_pos++];
	mydos->config.Nv = buf[buf_pos++];
	mydos->config.Eg = buf[buf_pos++];
	mydos->config.Xi = buf[buf_pos++];
#else
	fscanf(in, "%d %d %d %le %le %le %le %le %le %le %le %le %le\n",
	       &(mydos->xlen), &(mydos->tlen), &(mydos->srh_bands),
	       &(mydos->config.epsilonr), &(mydos->config.doping),
	       &(mydos->config.mu), &(mydos->config.srh_vth),
	       &(mydos->config.srh_sigman), &(mydos->config.srh_sigmap),
	       &(mydos->config.Nc), &(mydos->config.Nv), &(mydos->config.Eg),
	       &(mydos->config.Xi));
#endif

	double xsteps = mydos->xlen;
	double tsteps = mydos->tlen;
	mydos->x = (double *)malloc(sizeof(double) * (int)xsteps);
	mydos->t = (double *)malloc(sizeof(double) * (int)tsteps);
	mydos->srh_E =
	    (double *)malloc(sizeof(double) * (int)(mydos->srh_bands));
	mydos->srh_den =
	    (double *)malloc(sizeof(double) * (int)(mydos->srh_bands));

	mydos->srh_r1 =
	    (double ***)malloc(sizeof(double **) * (int)mydos->tlen);
	mydos->srh_r2 =
	    (double ***)malloc(sizeof(double **) * (int)mydos->tlen);
	mydos->srh_r3 =
	    (double ***)malloc(sizeof(double **) * (int)mydos->tlen);
	mydos->srh_r4 =
	    (double ***)malloc(sizeof(double **) * (int)mydos->tlen);
	mydos->srh_c = (double ***)malloc(sizeof(double **) * (int)mydos->tlen);

	mydos->w = (double **)malloc(sizeof(double **) * (int)mydos->tlen);
	mydos->c = (double **)malloc(sizeof(double *) * (int)mydos->tlen);

	for (t = 0; t < tsteps; t++) {
		mydos->c[t] = (double *)malloc(sizeof(double) * (int)xsteps);
		mydos->w[t] = (double *)malloc(sizeof(double) * (int)xsteps);
		mydos->srh_r1[t] =
		    (double **)malloc(sizeof(double *) * (int)xsteps);
		mydos->srh_r2[t] =
		    (double **)malloc(sizeof(double *) * (int)xsteps);
		mydos->srh_r3[t] =
		    (double **)malloc(sizeof(double *) * (int)xsteps);
		mydos->srh_r4[t] =
		    (double **)malloc(sizeof(double *) * (int)xsteps);
		mydos->srh_c[t] =
		    (double **)malloc(sizeof(double *) * (int)xsteps);

		for (x = 0; x < xsteps; x++) {
			mydos->srh_r1[t][x] =
			    (double *)malloc(sizeof(double) *
					     (int)(mydos->srh_bands));
			mydos->srh_r2[t][x] =
			    (double *)malloc(sizeof(double) *
					     (int)(mydos->srh_bands));
			mydos->srh_r3[t][x] =
			    (double *)malloc(sizeof(double) *
					     (int)(mydos->srh_bands));
			mydos->srh_r4[t][x] =
			    (double *)malloc(sizeof(double) *
					     (int)(mydos->srh_bands));
			mydos->srh_c[t][x] =
			    (double *)malloc(sizeof(double) *
					     (int)(mydos->srh_bands));
		}

	}

	for (x = 0; x < xsteps; x++) {
#ifdef dos_bin
		mydos->x[x] = buf[buf_pos++];
#else
		fscanf(in, "%le", &(mydos->x[x]));
#endif

	}

	for (t = 0; t < tsteps; t++) {
#ifdef dos_bin
		mydos->t[t] = buf[buf_pos++];
#else
		fscanf(in, "%le", &(mydos->t[t]));
#endif

	}

	for (srh_band = 0; srh_band < (mydos->srh_bands); srh_band++) {
#ifdef dos_bin
		mydos->srh_E[srh_band] = buf[buf_pos++];
#else
		fscanf(in, "%le", &(mydos->srh_E[srh_band]));
#endif

	}

	for (srh_band = 0; srh_band < (mydos->srh_bands); srh_band++) {
#ifdef dos_bin
		mydos->srh_den[srh_band] = buf[buf_pos++];
#else
		fscanf(in, "%le", &(mydos->srh_den[srh_band]));
#endif

	}

	for (t = 0; t < tsteps; t++) {
		for (x = 0; x < xsteps; x++) {

#ifdef dos_bin
			mydos->c[t][x] = buf[buf_pos++];
			mydos->w[t][x] = buf[buf_pos++];
#else
			fscanf(in, "%le %le ", &n, &w0);
			mydos->c[t][x] = n;
			mydos->w[t][x] = w0;
#endif

			for (srh_band = 0; srh_band < mydos->srh_bands;
			     srh_band++) {
#ifdef dos_bin
				mydos->srh_r1[t][x][srh_band] = buf[buf_pos++];
				mydos->srh_r2[t][x][srh_band] = buf[buf_pos++];
				mydos->srh_r3[t][x][srh_band] = buf[buf_pos++];
				mydos->srh_r4[t][x][srh_band] = buf[buf_pos++];
				mydos->srh_c[t][x][srh_band] = buf[buf_pos++];

#else
				fscanf(in, "%le %le %le %le %le ", &srh_r1,
				       &srh_r2, &srh_r3, &srh_r4, &srh_c);
				mydos->srh_r1[t][x][srh_band] = srh_r1;
				mydos->srh_r2[t][x][srh_band] = srh_r2;
				mydos->srh_r3[t][x][srh_band] = srh_r3;
				mydos->srh_r4[t][x][srh_band] = srh_r4;
				mydos->srh_c[t][x][srh_band] = srh_c;
#endif

			}

		}

	}
#ifdef dos_bin
	free(buf);
#else
	fclose(in);
#endif

}

double get_dos_doping(int mat)
{
	return dosn[mat].config.doping;
}

double get_dos_epsilonr(int mat)
{
	return dosn[mat].config.epsilonr;
}

double dos_get_band_energy_n(int band, int mat)
{
	return dosn[mat].srh_E[band];
}

double dos_get_band_energy_p(int band, int mat)
{
	return dosp[mat].srh_E[band];
}

double get_dos_Eg(int mat)
{
	return dosn[mat].config.Eg;
}

double get_dos_Xi(int mat)
{
	return dosn[mat].config.Xi;
}

void load_dos(struct device *dev, char *namen, char *namep, int mat)
{
	load_dos_file(&(dosn[mat]), namen);
	load_dos_file(&(dosp[mat]), namep);
	dev->srh_bands = dosn[mat].srh_bands;
}

double get_dos_E_n(int band, int mat)
{
	return dosn[mat].srh_E[band];
}

double get_dos_E_p(int band, int mat)
{
	return dosp[mat].srh_E[band];
}

double get_n_w(double top, double T, int mat)
{
	double ret = (3.0 / 2.0) * kb * T;
	return ret;
}

double get_p_w(double top, double T, int mat)
{
	double ret = (3.0 / 2.0) * kb * T;
	return ret;
}

double get_dpdT_den(double top, double T, int mat)
{
	double ret = 0.0;
	double N = dosp[mat].config.Nv;
	ret = -((top * q) / kb) * N * exp((top * q) / (kb * T)) * pow(T, -2.0);
	return ret;
}

double get_dndT_den(double top, double T, int mat)
{
	double ret = 0.0;
	double N = dosn[mat].config.Nc;
	ret = -((top * q) / kb) * N * exp((top * q) / (kb * T)) * pow(T, -2.0);
	return ret;
}

double get_top_from_n(double n, double T, int mat)
{
	double ret = (kb * T / q) * log((fabs(n)) / dosn[mat].config.Nc);
	return ret;
}

double get_top_from_p(double p, double T, int mat)
{
	double ret = (kb * T / q) * log((fabs(p)) / dosp[mat].config.Nv);
	return ret;
}

double get_n_den(double top, double T, int mat)
{
	double ret = dosn[mat].config.Nc * exp((q * top) / (T * kb));
	return ret;
}

double get_p_den(double top, double T, int mat)
{
	double ret = dosp[mat].config.Nv * exp((q * top) / (T * kb));
	return ret;
}

double get_n_mu(int mat)
{
	return dosn[mat].config.mu;
}

double get_p_mu(int mat)
{
	return dosp[mat].config.mu;
}

double get_dn_den(double top, double T, int mat)
{
	double ret =
	    (q / (T * kb)) * dosn[mat].config.Nc * exp((q * top) / (T * kb));
	return ret;
}

double get_dp_den(double top, double T, int mat)
{
	double ret =
	    (q / (T * kb)) * dosp[mat].config.Nv * exp((q * top) / (T * kb));
	return ret;
}

double get_n_srh(double top, double T, int trap, int r, int mat)
{
	double ret = 0.0;
	double c0 = 0.0;
	double c1 = 0.0;
	double x0 = 0.0;
	double x1 = 0.0;
	double t0 = 0.0;
	double t1 = 0.0;
	double c = 0.0;
	double xr = 0.0;
	double tr = 0.0;
	double c00 = 0.0;
	double c01 = 0.0;
	double c10 = 0.0;
	double c11 = 0.0;
	int t = 0;
	int x = 0;

#ifdef dos_warn
	if ((dosn[mat].x[0] > top) || (dosn[mat].x[dosn[mat].xlen - 1] < top)) {
		printf("Electrons asking for %e but range %e %e\n", top,
		       dosn[mat].x[0], dosn[mat].x[dosn[mat].xlen - 1]);
		if (get_dump_status(dump_exit_on_dos_error) == TRUE) {
			server_stop_and_exit();
		}
	}
#endif

	x = hashget(dosn[mat].x, dosn[mat].xlen, top);

	x0 = dosn[mat].x[x];
	x1 = dosn[mat].x[x + 1];
	xr = (top - x0) / (x1 - x0);

	if (dosn[mat].tlen > 1) {
		t = hashget(dosn[mat].t, dosn[mat].tlen, T);
		if (t < 0)
			t = 0;

		t0 = dosn[mat].t[t];
		t1 = dosn[mat].t[t + 1];
		tr = (T - t0) / (t1 - t0);
	} else {
		tr = 0.0;
	}

	switch (r) {
	case 1:
		c00 = dosn[mat].srh_r1[t][x][trap];
		c01 = dosn[mat].srh_r1[t][x + 1][trap];
		c0 = c00 + xr * (c01 - c00);

		if (dosn[mat].tlen > 1) {
			c10 = dosn[mat].srh_r1[t + 1][x][trap];
			c11 = dosn[mat].srh_r1[t + 1][x + 1][trap];
			c1 = c10 + xr * (c11 - c10);
		}
		break;

	case 2:
		c00 = dosn[mat].srh_r2[t][x][trap];
		c01 = dosn[mat].srh_r2[t][x + 1][trap];
		c0 = c00 + xr * (c01 - c00);

		if (dosn[mat].tlen > 1) {
			c10 = dosn[mat].srh_r2[t + 1][x][trap];
			c11 = dosn[mat].srh_r2[t + 1][x + 1][trap];
			c1 = c10 + xr * (c11 - c10);
		}
		break;

	case 3:
		c00 = dosn[mat].srh_r3[t][x][trap];
		c01 = dosn[mat].srh_r3[t][x + 1][trap];
		c0 = c00 + xr * (c01 - c00);

		if (dosn[mat].tlen > 1) {
			c10 = dosn[mat].srh_r3[t + 1][x][trap];
			c11 = dosn[mat].srh_r3[t + 1][x + 1][trap];
			c1 = c10 + xr * (c11 - c10);
		}
		break;

	case 4:
		c00 = dosn[mat].srh_r4[t][x][trap];
		c01 = dosn[mat].srh_r4[t][x + 1][trap];
		c0 = c00 + xr * (c01 - c00);

		if (dosn[mat].tlen > 1) {
			c10 = dosn[mat].srh_r4[t + 1][x][trap];
			c11 = dosn[mat].srh_r4[t + 1][x + 1][trap];
			c1 = c10 + xr * (c11 - c10);
		}
		break;
	}

	c = c0 + tr * (c1 - c0);
	ret = c;

	return ret;
}

double get_p_srh(double top, double T, int trap, int r, int mat)
{
	double ret = 0.0;
	double c0 = 0.0;
	double c1 = 0.0;
	double x0 = 0.0;
	double x1 = 0.0;
	double t0 = 0.0;
	double t1 = 0.0;
	double c = 0.0;
	double xr = 0.0;
	double tr = 0.0;
	double c00 = 0.0;
	double c01 = 0.0;
	double c10 = 0.0;
	double c11 = 0.0;
	int t = 0;
	int x = 0;

#ifdef dos_warn
	if ((dosp[mat].x[0] > top) || (dosp[mat].x[dosp[mat].xlen - 1] < top)) {
		printf("Holes asking for %e but range %e %e\n", top,
		       dosp[mat].x[0], dosp[mat].x[dosp[mat].xlen - 1]);
		if (get_dump_status(dump_exit_on_dos_error) == TRUE)
			server_stop_and_exit();

	}
#endif

	x = hashget(dosp[mat].x, dosp[mat].xlen, top);

	x0 = dosp[mat].x[x];
	x1 = dosp[mat].x[x + 1];
	xr = (top - x0) / (x1 - x0);

	if (dosp[mat].tlen > 1) {
		t = hashget(dosp[mat].t, dosp[mat].tlen, T);
		if (t < 0)
			t = 0;

		t0 = dosp[mat].t[t];
		t1 = dosp[mat].t[t + 1];
		tr = (T - t0) / (t1 - t0);
	} else {
		tr = 0.0;
	}

	switch (r) {
	case 1:
		c00 = dosp[mat].srh_r1[t][x][trap];
		c01 = dosp[mat].srh_r1[t][x + 1][trap];
		c0 = c00 + xr * (c01 - c00);

		if (dosp[mat].tlen > 1) {
			c10 = dosp[mat].srh_r1[t + 1][x][trap];
			c11 = dosp[mat].srh_r1[t + 1][x + 1][trap];
			c1 = c10 + xr * (c11 - c10);
		}
		break;

	case 2:
		c00 = dosp[mat].srh_r2[t][x][trap];
		c01 = dosp[mat].srh_r2[t][x + 1][trap];
		c0 = c00 + xr * (c01 - c00);

		if (dosp[mat].tlen > 1) {
			c10 = dosp[mat].srh_r2[t + 1][x][trap];
			c11 = dosp[mat].srh_r2[t + 1][x + 1][trap];
			c1 = c10 + xr * (c11 - c10);
		}
		break;

	case 3:
		c00 = dosp[mat].srh_r3[t][x][trap];
		c01 = dosp[mat].srh_r3[t][x + 1][trap];
		c0 = c00 + xr * (c01 - c00);

		if (dosp[mat].tlen > 1) {
			c10 = dosp[mat].srh_r3[t + 1][x][trap];
			c11 = dosp[mat].srh_r3[t + 1][x + 1][trap];
			c1 = c10 + xr * (c11 - c10);
		}
		break;

	case 4:
		c00 = dosp[mat].srh_r4[t][x][trap];
		c01 = dosp[mat].srh_r4[t][x + 1][trap];
		c0 = c00 + xr * (c01 - c00);

		if (dosp[mat].tlen > 1) {
			c10 = dosp[mat].srh_r4[t + 1][x][trap];
			c11 = dosp[mat].srh_r4[t + 1][x + 1][trap];
			c1 = c10 + xr * (c11 - c10);
		}
		break;
	}

	c = c0 + tr * (c1 - c0);
	ret = c;

	return ret;
}

double get_dn_srh(double top, double T, int trap, int r, int mat)
{
	double ret = 0.0;
	double c0 = 0.0;
	double c1 = 0.0;
	double x0 = 0.0;
	double x1 = 0.0;
	double t0 = 0.0;
	double t1 = 0.0;
	double c = 0.0;
	double xr = 0.0;
	double tr = 0.0;
	double c00 = 0.0;
	double c01 = 0.0;
	double c10 = 0.0;
	double c11 = 0.0;

	int t = 0;
	int x;

#ifdef dos_warn
	if ((dosn[mat].x[0] > top) || (dosn[mat].x[dosn[mat].xlen - 1] < top)) {
		printf("Electrons asking for %e but range %e %e\n", top,
		       dosn[mat].x[0], dosn[mat].x[dosn[mat].xlen - 1]);
		if (get_dump_status(dump_exit_on_dos_error) == TRUE)
			server_stop_and_exit();

	}
#endif

	x = hashget(dosn[mat].x, dosn[mat].xlen, top);

	x0 = dosn[mat].x[x];
	x1 = dosn[mat].x[x + 1];
	xr = 1.0 / (x1 - x0);

	if (dosn[mat].tlen > 1) {
		t = hashget(dosn[mat].t, dosn[mat].tlen, T);
		t0 = dosn[mat].t[t];
		t1 = dosn[mat].t[t + 1];
		tr = (T - t0) / (t1 - t0);
	} else {
		tr = 0.0;
	}

	switch (r) {

	case 1:
		c00 = dosn[mat].srh_r1[t][x][trap];
		c01 = dosn[mat].srh_r1[t][x + 1][trap];
		c0 = xr * (c01 - c00);

		if (dosn[mat].tlen > 1) {
			c10 = dosn[mat].srh_r1[t + 1][x][trap];
			c11 = dosn[mat].srh_r1[t + 1][x + 1][trap];
			c1 = xr * (c11 - c10);
		}
		break;

	case 2:
		c00 = dosn[mat].srh_r2[t][x][trap];
		c01 = dosn[mat].srh_r2[t][x + 1][trap];
		c0 = xr * (c01 - c00);

		if (dosn[mat].tlen > 1) {
			c10 = dosn[mat].srh_r2[t + 1][x][trap];
			c11 = dosn[mat].srh_r2[t + 1][x + 1][trap];
			c1 = xr * (c11 - c10);
		}
		break;

	case 3:
		c00 = dosn[mat].srh_r3[t][x][trap];
		c01 = dosn[mat].srh_r3[t][x + 1][trap];
		c0 = xr * (c01 - c00);

		if (dosn[mat].tlen > 1) {
			c10 = dosn[mat].srh_r3[t + 1][x][trap];
			c11 = dosn[mat].srh_r3[t + 1][x + 1][trap];
			c1 = xr * (c11 - c10);
		}
		break;

	case 4:
		c00 = dosn[mat].srh_r4[t][x][trap];
		c01 = dosn[mat].srh_r4[t][x + 1][trap];
		c0 = xr * (c01 - c00);

		if (dosn[mat].tlen > 1) {
			c10 = dosn[mat].srh_r4[t + 1][x][trap];
			c11 = dosn[mat].srh_r4[t + 1][x + 1][trap];
			c1 = xr * (c11 - c10);
		}
		break;
	}

	c = c0 + tr * (c1 - c0);

	ret = c;

	return ret;
}

double get_dp_srh(double top, double T, int trap, int r, int mat)
{
	double ret = 0.0;
	double c0 = 0.0;
	double c1 = 0.0;
	double x0 = 0.0;
	double x1 = 0.0;
	double t0 = 0.0;
	double t1 = 0.0;
	double c = 0.0;
	double xr = 0.0;
	double tr = 0.0;
	double c00 = 0.0;
	double c01 = 0.0;
	double c10 = 0.0;
	double c11 = 0.0;

	int t = 0;
	int x;

#ifdef dos_warn
	if ((dosp[mat].x[0] > top) || (dosp[mat].x[dosp[mat].xlen - 1] < top)) {
		printf("Holes asking for %e but range %e %e\n", top,
		       dosp[mat].x[0], dosp[mat].x[dosp[mat].xlen - 1]);
		if (get_dump_status(dump_exit_on_dos_error) == TRUE)
			server_stop_and_exit();

	}
#endif

	x = hashget(dosp[mat].x, dosp[mat].xlen, top);

	x0 = dosp[mat].x[x];
	x1 = dosp[mat].x[x + 1];
	xr = 1.0 / (x1 - x0);

	if (dosp[mat].tlen > 1) {
		t = hashget(dosp[mat].t, dosp[mat].tlen, T);
		t0 = dosp[mat].t[t];
		t1 = dosp[mat].t[t + 1];
		tr = (T - t0) / (t1 - t0);
	} else {
		tr = 0.0;
	}

	switch (r) {
	case 1:
		c00 = dosp[mat].srh_r1[t][x][trap];
		c01 = dosp[mat].srh_r1[t][x + 1][trap];
		c0 = xr * (c01 - c00);

		if (dosp[mat].tlen > 1) {
			c10 = dosp[mat].srh_r1[t + 1][x][trap];
			c11 = dosp[mat].srh_r1[t + 1][x + 1][trap];
			c1 = xr * (c11 - c10);
		}
		break;

	case 2:
		c00 = dosp[mat].srh_r2[t][x][trap];
		c01 = dosp[mat].srh_r2[t][x + 1][trap];
		c0 = xr * (c01 - c00);

		if (dosp[mat].tlen > 1) {
			c10 = dosp[mat].srh_r2[t + 1][x][trap];
			c11 = dosp[mat].srh_r2[t + 1][x + 1][trap];
			c1 = xr * (c11 - c10);
		}
		break;

	case 3:
		c00 = dosp[mat].srh_r3[t][x][trap];
		c01 = dosp[mat].srh_r3[t][x + 1][trap];
		c0 = xr * (c01 - c00);

		if (dosp[mat].tlen > 1) {
			c10 = dosp[mat].srh_r3[t + 1][x][trap];
			c11 = dosp[mat].srh_r3[t + 1][x + 1][trap];
			c1 = xr * (c11 - c10);
		}
		break;

	case 4:
		c00 = dosp[mat].srh_r4[t][x][trap];
		c01 = dosp[mat].srh_r4[t][x + 1][trap];
		c0 = xr * (c01 - c00);

		if (dosp[mat].tlen > 1) {
			c10 = dosp[mat].srh_r4[t + 1][x][trap];
			c11 = dosp[mat].srh_r4[t + 1][x + 1][trap];
			c1 = xr * (c11 - c10);
		}
		break;
	}

	c = c0 + tr * (c1 - c0);

	ret = c;

	return ret;
}

double get_n_pop_srh(double top, double T, int trap, int mat)
{
	double ret = 0.0;
	double c0 = 0.0;
	double c1 = 0.0;
	double x0 = 0.0;
	double x1 = 0.0;
	double t0 = 0.0;
	double t1 = 0.0;
	double c = 0.0;
	double xr = 0.0;
	double tr = 0.0;
	double c00 = 0.0;
	double c01 = 0.0;
	double c10 = 0.0;
	double c11 = 0.0;
	int t = 0;
	int x = 0;

#ifdef dos_warn
	if ((dosn[mat].x[0] > top) || (dosn[mat].x[dosn[mat].xlen - 1] < top)) {
		printf("Electrons asking for %e but range %e %e\n", top,
		       dosn[mat].x[0], dosn[mat].x[dosn[mat].xlen - 1]);
		if (get_dump_status(dump_exit_on_dos_error) == TRUE)
			server_stop_and_exit();

	}
#endif

	x = hashget(dosn[mat].x, dosn[mat].xlen, top);

	x0 = dosn[mat].x[x];
	x1 = dosn[mat].x[x + 1];
	xr = (top - x0) / (x1 - x0);
	if (dosn[mat].tlen > 1) {
		t = hashget(dosn[mat].t, dosn[mat].tlen, T);

		t0 = dosn[mat].t[t];
		t1 = dosn[mat].t[t + 1];
		tr = (T - t0) / (t1 - t0);
	} else {
		tr = 0.0;
	}

	c00 = dosn[mat].srh_c[t][x][trap];
	c01 = dosn[mat].srh_c[t][x + 1][trap];
	c0 = c00 + xr * (c01 - c00);

	if (dosn[mat].tlen > 1) {
		c10 = dosn[mat].srh_c[t + 1][x][trap];
		c11 = dosn[mat].srh_c[t + 1][x + 1][trap];
		c1 = c10 + xr * (c11 - c10);
	}

	c = c0 + tr * (c1 - c0);
	ret = c;

	return ret;
}

double get_p_pop_srh(double top, double T, int trap, int mat)
{
	double ret = 0.0;
	double c0 = 0.0;
	double c1 = 0.0;
	double x0 = 0.0;
	double x1 = 0.0;
	double t0 = 0.0;
	double t1 = 0.0;
	double c = 0.0;
	double xr = 0.0;
	double tr = 0.0;
	double c00 = 0.0;
	double c01 = 0.0;
	double c10 = 0.0;
	double c11 = 0.0;
	int t = 0;
	int x = 0;

#ifdef dos_warn
	if ((dosp[mat].x[0] > top) || (dosp[mat].x[dosp[mat].xlen - 1] < top)) {
		printf("Holes asking for %e but range %e %e\n", top,
		       dosp[mat].x[0], dosp[mat].x[dosp[mat].xlen - 1]);
		if (get_dump_status(dump_exit_on_dos_error) == TRUE)
			server_stop_and_exit();

	}
#endif

	x = hashget(dosp[mat].x, dosp[mat].xlen, top);

	x0 = dosp[mat].x[x];
	x1 = dosp[mat].x[x + 1];
	xr = (top - x0) / (x1 - x0);
	if (dosp[mat].tlen > 1) {
		t = hashget(dosp[mat].t, dosp[mat].tlen, T);
		t0 = dosp[mat].t[t];
		t1 = dosp[mat].t[t + 1];
		tr = (T - t0) / (t1 - t0);
	} else {
		tr = 0.0;
	}

	c00 = dosp[mat].srh_c[t][x][trap];
	c01 = dosp[mat].srh_c[t][x + 1][trap];
	c0 = c00 + xr * (c01 - c00);

	if (dosp[mat].tlen > 1) {
		c10 = dosp[mat].srh_c[t + 1][x][trap];
		c11 = dosp[mat].srh_c[t + 1][x + 1][trap];
		c1 = c10 + xr * (c11 - c10);
	}

	c = c0 + tr * (c1 - c0);
	ret = c;

	return ret;
}

double get_dn_pop_srh(double top, double T, int trap, int mat)
{
	double ret = 0.0;
	double c0 = 0.0;
	double c1 = 0.0;
	double x0 = 0.0;
	double x1 = 0.0;
	double t0 = 0.0;
	double t1 = 0.0;
	double c = 0.0;
	double xr = 0.0;
	double tr = 0.0;
	double c00 = 0.0;
	double c01 = 0.0;
	double c10 = 0.0;
	double c11 = 0.0;

	int t = 0;
	int x;

#ifdef dos_warn
	if ((dosn[mat].x[0] > top) || (dosn[mat].x[dosn[mat].xlen - 1] < top)) {
		printf("Electrons asking for %e but range %e %e\n", top,
		       dosn[mat].x[0], dosn[mat].x[dosn[mat].xlen - 1]);
		if (get_dump_status(dump_exit_on_dos_error) == TRUE)
			server_stop_and_exit();

	}
#endif

	x = hashget(dosn[mat].x, dosn[mat].xlen, top);

	x0 = dosn[mat].x[x];
	x1 = dosn[mat].x[x + 1];
	xr = 1.0 / (x1 - x0);

	if (dosn[mat].tlen > 1) {
		t = hashget(dosn[mat].t, dosn[mat].tlen, T);
		t0 = dosn[mat].t[t];
		t1 = dosn[mat].t[t + 1];
		tr = (T - t0) / (t1 - t0);
	} else {
		tr = 0.0;
	}

	c00 = dosn[mat].srh_c[t][x][trap];
	c01 = dosn[mat].srh_c[t][x + 1][trap];
	c0 = xr * (c01 - c00);

	if (dosn[mat].tlen > 1) {
		c10 = dosn[mat].srh_c[t + 1][x][trap];
		c11 = dosn[mat].srh_c[t + 1][x + 1][trap];
		c1 = xr * (c11 - c10);
	}

	c = c0 + tr * (c1 - c0);

	ret = c;

	return ret;
}

double get_dp_pop_srh(double top, double T, int trap, int mat)
{
	double ret = 0.0;
	double c0 = 0.0;
	double c1 = 0.0;
	double x0 = 0.0;
	double x1 = 0.0;
	double t0 = 0.0;
	double t1 = 0.0;
	double c = 0.0;
	double xr = 0.0;
	double tr = 0.0;
	double c00 = 0.0;
	double c01 = 0.0;
	double c10 = 0.0;
	double c11 = 0.0;

	int t = 0;
	int x;

#ifdef dos_warn
	if ((dosp[mat].x[0] > top) || (dosp[mat].x[dosp[mat].xlen - 1] < top)) {
		printf("Holes asking for %e but range %e %e\n", top,
		       dosp[mat].x[0], dosp[mat].x[dosp[mat].xlen - 1]);
		if (get_dump_status(dump_exit_on_dos_error) == TRUE)
			server_stop_and_exit();

	}
#endif

	x = hashget(dosp[mat].x, dosp[mat].xlen, top);

	x0 = dosp[mat].x[x];
	x1 = dosp[mat].x[x + 1];
	xr = 1.0 / (x1 - x0);

	if (dosp[mat].tlen > 1) {
		t = hashget(dosp[mat].t, dosp[mat].tlen, T);
		t0 = dosp[mat].t[t];
		t1 = dosp[mat].t[t + 1];
		tr = (T - t0) / (t1 - t0);
	} else {
		tr = 0.0;
	}

	c00 = dosp[mat].srh_c[t][x][trap];
	c01 = dosp[mat].srh_c[t][x + 1][trap];
	c0 = xr * (c01 - c00);

	if (dosp[mat].tlen > 1) {
		c10 = dosp[mat].srh_c[t + 1][x][trap];
		c11 = dosp[mat].srh_c[t + 1][x + 1][trap];
		c1 = xr * (c11 - c10);
	}

	c = c0 + tr * (c1 - c0);

	ret = c;

	return ret;
}

void draw_gaus(struct device *in)
{
}
