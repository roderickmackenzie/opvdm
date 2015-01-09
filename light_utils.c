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
#include "light.h"
#include "device.h"
#include "const.h"
#include "config.h"

static int unused __attribute__ ((unused));

void light_free_memory(struct light *in)
{
	int i;
	free(in->H1d);
	free(in->thick);
	free(in->x);

	for (i = 0; i < in->layers; i++) {
		inter_free(&(in->mat[i]));
		inter_free(&(in->mat_n[i]));
	}
	free(in->mat);
	free(in->mat_n);

	for (i = 0; i < in->lpoints; i++) {
		free(in->En[i]);
		free(in->Enz[i]);
		free(in->Ep[i]);
		free(in->Epz[i]);

		free(in->photons_asb[i]);
		free(in->alpha[i]);
		free(in->photons[i]);
		free(in->pointing_vector[i]);
		free(in->E_tot_r[i]);
		free(in->E_tot_i[i]);
		free(in->n[i]);
		free(in->t[i]);
		free(in->r[i]);
		free(in->H[i]);
	}

	free(in->layer_end);
	free(in->G_percent);
	free(in->enable_gen);
	free(in->t);
	free(in->r);
	free(in->En);
	free(in->Enz);
	free(in->Ep);
	free(in->Epz);
	free(in->photons_asb);
	free(in->alpha);
	free(in->photons);
	free(in->pointing_vector);
	free(in->E_tot_r);
	free(in->E_tot_i);
	free(in->n);
	free(in->H);

	free(in->sun);
	free(in->sun_norm);
	free(in->sun_photons);
	free(in->sun_E);
	free(in->Gn);
	free(in->Gp);
	free(in->photons_tot);
	free(in->layer);
	free(in->Ti);
	free(in->Tj);
	free(in->Tx);
	free(in->Txz);
	free(in->b);
	free(in->bz);
	free(in->l);
	inter_free(&(in->sun_read));

}

void light_init_mesh(struct light *in, char *output_path)
{
	char temp[200];
	char file_path[200];
	char physdir[200];

	FILE *pfile = fopen("physdir.inp", "r");
	if (pfile == NULL) {
		ewe("physdir.inp not found\n");
	}
	config_read_line_to_string(physdir, pfile, "#physdir");

	fclose(pfile);

	strcpy(in->output_path, output_path);
	sprintf(in->config_file, "%soptics.inp", in->output_path);
	printf("Load optics config %s\n", in->config_file);
	FILE *config = fopen(in->config_file, "r");
	if (config == NULL) {
		ewe("Optics config file not found\n");
	}
	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%s", temp);
	FILE *epi = fopena(in->output_path, temp, "r");
	if (epi == NULL) {
		ewe("Optics epitaxy file not found\n");
	}
	int i = 0;
	double pos = 0.0;

	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%s", temp);
	sprintf(file_path, "%s%s", physdir, temp);
	inter_load_a(&(in->sun_read), file_path, 0, 1);
	inter_sort(&(in->sun_read));

	unused = fscanf(epi, "%s", temp);
	unused = fscanf(epi, "%d", &in->layers);
	in->thick = (double *)malloc(in->layers * sizeof(double));
	in->G_percent = (double *)malloc(in->layers * sizeof(double));
	in->enable_gen = (int *)malloc(in->layers * sizeof(int));
	in->mat = (struct istruct *)malloc(in->layers * sizeof(struct istruct));
	in->mat_n =
	    (struct istruct *)malloc(in->layers * sizeof(struct istruct));
	in->laser_photon_density = -1.0;
	in->ylen = 0.0;
	in->device_start = 0.0;
	int device_started = FALSE;
	for (i = 0; i < in->layers; i++) {
		unused = fscanf(epi, "%s", temp);
		unused = fscanf(epi, "%le", &(in->thick[i]));

		unused = fscanf(epi, "%s", temp);
		sprintf(file_path, "%s%s/alpha.inp", physdir, temp);
		inter_load_a(&(in->mat[i]), file_path, 0, 1);
		inter_sort(&(in->mat[i]));

		sprintf(file_path, "%s%s/n.inp", physdir, temp);
		inter_load_a(&(in->mat_n[i]), file_path, 0, 1);
		inter_sort(&(in->mat_n[i]));

		int device;
		unused = fscanf(epi, "%d", &device);
		if (device == TRUE) {
			device_started = TRUE;
		}

		if (device_started == FALSE) {
			in->device_start += in->thick[i];
			in->device_start_layer = i;
		}

		unused = fscanf(epi, "%d", &(in->enable_gen[i]));
		in->ylen += in->thick[i];
	}

	double ver;
	unused = fscanf(epi, "%s", temp);
	unused = fscanf(epi, "%lf", &(ver));

	if (ver != 1.1) {
		ewe("File compatability problem %s\n", "optics_epitaxy.inp");
	}

	unused = fscanf(epi, "%s", temp);
	if (strcmp("#end", temp) != 0) {
		ewe("Problem with device_epitaxy.inp file last item read %s!\n",
		    temp);
	}

	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%d", &in->align_mesh);

	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%d", &in->points);

	if (in->align_mesh == FALSE) {
		in->dx = in->ylen / ((double)in->points);
	} else {
		in->points = (int)(in->ylen / in->dx);
	}

	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%d", &in->lpoints);

	in->x = (double *)malloc(in->points * sizeof(double));
	in->H1d = (double *)malloc(in->points * sizeof(double));
	in->l = (double *)malloc(in->points * sizeof(double));
	in->Gn = (double *)malloc(in->points * sizeof(double));
	in->Gp = (double *)malloc(in->points * sizeof(double));
	in->photons_tot = (double *)malloc(in->points * sizeof(double));
	in->sun = (double *)malloc(in->lpoints * sizeof(double));
	in->layer_end = (double *)malloc(in->points * sizeof(double));
	in->sun_norm = (double *)malloc(in->lpoints * sizeof(double));
	in->sun_photons = (double *)malloc(in->lpoints * sizeof(double));
	in->sun_E = (double *)malloc(in->lpoints * sizeof(double));
	in->En = (double **)malloc(in->lpoints * sizeof(double *));
	in->Enz = (double **)malloc(in->lpoints * sizeof(double *));
	in->Ep = (double **)malloc(in->lpoints * sizeof(double *));
	in->Epz = (double **)malloc(in->lpoints * sizeof(double *));
	in->photons_asb = (double **)malloc(in->lpoints * sizeof(double *));
	in->H = (double **)malloc(in->lpoints * sizeof(double *));
	in->alpha = (double **)malloc(in->lpoints * sizeof(double *));
	in->photons = (double **)malloc(in->lpoints * sizeof(double *));
	in->pointing_vector = (double **)malloc(in->lpoints * sizeof(double *));
	in->E_tot_r = (double **)malloc(in->lpoints * sizeof(double *));
	in->E_tot_i = (double **)malloc(in->lpoints * sizeof(double *));
	in->n = (double **)malloc(in->lpoints * sizeof(double *));
	in->t =
	    (double complex **)malloc(in->lpoints * sizeof(double complex *));
	in->r =
	    (double complex **)malloc(in->lpoints * sizeof(double complex *));

	for (i = 0; i < in->lpoints; i++) {
		in->En[i] = (double *)malloc(in->points * sizeof(double));
		in->Enz[i] = (double *)malloc(in->points * sizeof(double));
		in->Ep[i] = (double *)malloc(in->points * sizeof(double));
		in->Epz[i] = (double *)malloc(in->points * sizeof(double));
		in->photons_asb[i] =
		    (double *)malloc(in->points * sizeof(double));
		in->alpha[i] = (double *)malloc(in->points * sizeof(double));
		in->photons[i] = (double *)malloc(in->points * sizeof(double));
		in->pointing_vector[i] =
		    (double *)malloc(in->points * sizeof(double *));
		in->E_tot_r[i] =
		    (double *)malloc(in->points * sizeof(double *));
		in->E_tot_i[i] =
		    (double *)malloc(in->points * sizeof(double *));
		in->n[i] = (double *)malloc(in->points * sizeof(double));
		in->t[i] =
		    (double complex *)malloc(in->points *
					     sizeof(double complex));
		in->r[i] =
		    (double complex *)malloc(in->points *
					     sizeof(double complex));
		in->H[i] = (double *)malloc(in->points * sizeof(double));

	}

	in->layer = (int *)malloc(in->points * sizeof(int));

	pos = in->dx / 2.0;

	int layer = 0;
	double layer_end = in->thick[layer];

	for (i = 0; i < in->points; i++) {
		in->x[i] = pos;
		in->layer_end[i] = layer_end - pos;
		in->layer[i] = layer;
		if (in->device_start_layer >= layer)
			in->device_start_i = i;

		pos += in->dx;

		if (pos > layer_end) {

			if (layer < (in->layers - 1)) {
				layer++;

				layer_end = layer_end + in->thick[layer];
			}
		}

	}
	in->device_start_i++;

	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%le", &in->lstart);

	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%le", &in->lstop);
	in->dl = (in->lstop - in->lstart) / ((double)in->lpoints);

	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%le", &in->laser_wavelength);
	in->laser_pos = (int)((in->laser_wavelength - in->lstart) / in->dl);

	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%le", &in->spotx);

	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%le", &in->spoty);

	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%le", &in->pulseJ);

	unused = fscanf(config, "%s", temp);
	unused = fscanf(config, "%le", &in->pulseL);

	pos = in->lstart;
	for (i = 0; i < in->lpoints; i++) {
		in->l[i] = pos;
		pos += in->dl;
	}

	int ii;
	double nl = 0.0;
	double nc = 0.0;
	double kl = 0.0;
	double kc = 0.0;
	double complex n0 = 0.0 + 0.0 * I;
	double complex n1 = 0.0 + 0.0 * I;
	for (i = 0; i < in->lpoints; i++) {
		for (ii = 0; ii < in->points; ii++) {
			in->alpha[i][ii] =
			    inter_get_noend(&(in->mat[in->layer[ii]]),
					    in->l[i]);
			in->n[i][ii] =
			    inter_get_noend(&(in->mat_n[in->layer[ii]]),
					    in->l[i]);
			if (ii == 0) {
				nl = in->n[i][ii];
				kl = in->alpha[i][ii] * (in->l[i] / (pi * 4.0));
			} else {
				nl = in->n[i][ii - 1];
				kl = in->alpha[i][ii -
						  1] * (in->l[i] / (pi * 4.0));
			}
			nc = in->n[i][ii];
			kc = in->alpha[i][ii] * (in->l[i] / (pi * 4.0));

			n0 = nl + kl * I;
			n1 = nc + kc * I;

			in->r[i][ii] = (n0 - n1) / (n0 + n1);
			in->t[i][ii] = (2.0 * n0) / (n0 + n1);

		}
		memset(in->En[i], 0.0, in->points * sizeof(double));
		memset(in->Ep[i], 0.0, in->points * sizeof(double));
		memset(in->Enz[i], 0.0, in->points * sizeof(double));
		memset(in->Epz[i], 0.0, in->points * sizeof(double));

	}

	for (i = 0; i < in->lpoints; i++) {
		in->sun_norm[i] = inter_get_noend(&(in->sun_read), in->l[i]);
	}

	double tot = 0.0;
	for (i = 0; i < in->lpoints; i++) {
		tot += in->dl * in->sun_norm[i];
	}

	for (i = 0; i < in->lpoints; i++) {
		in->sun_norm[i] /= tot;
	}

	config_read_line_to_int(&(in->gather), config, "#gather");

	config_read_line_to_double(&(in->dlength), config, "#dlength");

	config_read_line_to_double(&(in->Eg), config, "#Eg");

	config_read_line_to_double(&(ver), config, "#ver");

	if (ver != 1.8) {
		ewe("File compatability problem %s\n", "optics.inp");
	}

	unused = fscanf(config, "%s", temp);
	if (strcmp(temp, "#end") != 0) {
		ewe("Error at end of lightfile file\n");
	}

	fclose(epi);
	fclose(config);

	in->N = 0.0;
	in->N += in->points * 2 - 1;
	in->N += in->points * 2 - 1;
	in->N += in->points;
	in->N += in->points;

	in->M = in->points + in->points;
	in->Ti = malloc(in->N * sizeof(int));
	in->Tj = malloc(in->N * sizeof(int));
	in->Tx = malloc(in->N * sizeof(double));
	in->Txz = malloc(in->N * sizeof(double));
	in->b = malloc(in->M * sizeof(double));
	in->bz = malloc(in->M * sizeof(double));

}

void light_set_sun_power(struct light *in, double power, double laser_eff)
{
	int i;

	double E = 0.0;

	for (i = 0; i < in->lpoints; i++) {
		in->sun[i] = in->sun_norm[i] * power;

		E = hp * cl / in->l[i];
		in->sun_photons[i] = in->sun[i] / E;
		if (i == in->laser_pos) {
			if (in->laser_photon_density == -1.0) {

				in->sun_photons[i] +=
				    laser_eff * ((in->pulseJ / in->pulseL / E) /
						 (in->spotx * in->spoty)) /
				    in->dl;

			} else {
				in->sun_photons[i] +=
				    in->laser_photon_density / in->dl;
			}
		}

		in->sun_E[i] =
		    pow(2.0 * (in->sun_photons[i] * E) /
			(epsilon0 * cl * in->n[i][0]), 0.5);

	}

}

void light_solve_all(struct light *in)
{
	int i;
	for (i = 0; i < in->lpoints; i++) {
		if (in->sun_E[i] != 0.0) {
			light_solve_lam_slice(in, i);
		} else {
			memset(in->En[i], 0.0, in->points * sizeof(double));
			memset(in->Ep[i], 0.0, in->points * sizeof(double));
			memset(in->Enz[i], 0.0, in->points * sizeof(double));
			memset(in->Epz[i], 0.0, in->points * sizeof(double));
		}
	}
}

void light_set_unity_power(struct light *in)
{
	int i;

	for (i = 0; i < in->lpoints; i++) {

		in->sun[i] = 0.0;
		in->sun_photons[i] = 0.0;
		in->sun_E[i] = 1.0;
	}

}

void light_set_unity_laser_power(struct light *in, int lam)
{
	memset(in->sun, 0.0, in->lpoints * sizeof(double));
	memset(in->sun_photons, 0.0, in->lpoints * sizeof(double));
	memset(in->sun_E, 0.0, in->lpoints * sizeof(double));
	in->sun_E[lam] = 1.0;
}

void light_get_mode(struct istruct *mode, int lam, struct light *in)
{
	int ii;

	for (ii = 0; ii < mode->len; ii++) {
		mode->data[ii] =
		    inter_get_raw(in->x, in->photons[lam], in->points,
				  in->device_start + mode->x[ii]);
	}

}

void light_set_dx(struct light *in, double dx)
{
	in->dx = dx;
}

int light_find_wavelength(struct light *in, double lam)
{
	int i = 0;
	int l = 0;
	for (i = 0; i < in->lpoints; i++) {
		if (in->l[i] >= lam) {
			l = i;
			break;
		}
	}

	return l;
}

double light_convert_density(struct device *in, double start, double width)
{
	double ratio = 0.0;
	return ratio;
}
