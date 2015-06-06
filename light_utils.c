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
#include <unistd.h>
#include <dirent.h>
#include "light.h"
#include "device.h"
#include "const.h"
#include "dump.h"
#include "config.h"
#include "inp.h"
#include "util.h"

static int unused __attribute__ ((unused));

void light_solve_optical_problem(struct light *in)
{
	int i;

	double Psun = in->Psun * pow(10.0, -in->ND);
	light_set_sun_power(in, Psun, in->laser_eff);
	if ((in->laser_eff == 0) && (in->Psun == 0)) {

		if (get_dump_status(dump_optics) == TRUE)
			printf("It's dark I know what the answer is\n");
		for (i = 0; i < in->lpoints; i++) {
			memset(in->En[i], 0.0, in->points * sizeof(double));
			memset(in->Ep[i], 0.0, in->points * sizeof(double));
			memset(in->Enz[i], 0.0, in->points * sizeof(double));
			memset(in->Epz[i], 0.0, in->points * sizeof(double));
		}

	} else {

		light_solve_all(in);
	}

	light_cal_photon_density(in);

	for (i = 0; i < in->lpoints; i++) {
		light_dump_1d(in, i, "");
	}

}

double light_cal_photon_density(struct light *in)
{

	int i;
	int ii;
	double tot = 0.0;

	double H_tot = 0.0;
	double photons_tot = 0.0;

	for (ii = 0; ii < in->points; ii++) {
		tot = 0.0;
		H_tot = 0.0;
		photons_tot = 0.0;
		for (i = 0; i < in->lpoints; i++) {
			in->E_tot_r[i][ii] = in->Ep[i][ii] + in->En[i][ii];
			in->E_tot_i[i][ii] = in->Enz[i][ii] + in->Epz[i][ii];
			in->pointing_vector[i][ii] =
			    0.5 * epsilon0 * cl * in->n[i][ii] *
			    (pow(in->E_tot_r[i][ii], 2.0) +
			     pow(in->E_tot_i[i][ii], 2.0));

			if (strcmp(in->mode, "bleach") != 0) {
				in->photons[i][ii] =
				    in->pointing_vector[i][ii] * (in->l[i] /
								  (hp * cl));
				in->photons_asb[i][ii] =
				    in->photons[i][ii] * in->alpha[i][ii];
			}
			double E = ((hp * cl) / in->l[i]) / Q - in->Eg;

			if (E > 0.0) {
				in->H[i][ii] = E * Q * in->photons_asb[i][ii];
			} else {
				in->H[i][ii] = 0.0;
			}

			photons_tot += in->photons[i][ii] * in->dl;
			tot += in->photons_asb[i][ii] * in->dl;
			H_tot += in->H[i][ii] * in->dl;

		}

		in->Gn[ii] = tot;
		in->Gp[ii] = tot;

		in->H1d[ii] = H_tot;
		in->photons_tot[ii] = photons_tot;

		for (i = 0; i < in->lpoints; i++) {
			in->reflect[i] =
			    (pow(in->En[i][0], 2.0) +
			     pow(in->Enz[i][0], 2.0)) / (pow(in->Ep[i][0],
							     2.0) +
							 pow(in->Epz[i][0],
							     2.0));
		}

	}

	return 0.0;
}

void light_free_memory(struct light *in)
{

	light_free_epitaxy(in);
	light_free_materials(in);
	int i;
	free(in->H1d);
	free(in->x);
	free(in->layer_end);

	for (i = 0; i < in->lpoints; i++) {
		free(in->En[i]);
		free(in->Enz[i]);
		free(in->Ep[i]);
		free(in->Epz[i]);

		free(in->photons_asb[i]);
		free(in->alpha[i]);
		free(in->alpha0[i]);
		free(in->photons[i]);
		free(in->pointing_vector[i]);
		free(in->E_tot_r[i]);
		free(in->E_tot_i[i]);
		free(in->n[i]);
		free(in->t[i]);
		free(in->r[i]);
		free(in->nbar[i]);
		free(in->H[i]);
	}

	free(in->t);
	free(in->r);
	free(in->nbar);
	free(in->En);
	free(in->Enz);
	free(in->Ep);
	free(in->Epz);
	free(in->photons_asb);
	free(in->alpha);
	free(in->alpha0);
	free(in->photons);
	free(in->pointing_vector);
	free(in->E_tot_r);
	free(in->E_tot_i);
	free(in->n);
	free(in->H);
	free(in->reflect);

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
	printf("Light free memory\n");
}

void light_load_materials(struct light *in)
{
	int i = 0;
	char fit_file[1000];
	char physdir[200];
	char file_path[400];

	DIR *theFolder;
	char pwd[1000];
	char temp[1000];
	getcwd(pwd, 1000);

	join_path(2, temp, pwd, "phys");
	int found = FALSE;
	theFolder = opendir(temp);
	if (theFolder != NULL) {
		closedir(theFolder);
		strcpy(physdir, temp);
		found = TRUE;
	}

	if (found == FALSE) {
		FILE *pfile = fopen("physdir.inp", "r");
		if (pfile == NULL) {
			ewe("physdir.inp not found\n");
		}
		config_read_line_to_string(temp, pfile, "#physdir");
		fclose(pfile);

		theFolder = opendir(temp);
		if (theFolder != NULL) {
			closedir(theFolder);
			strcpy(physdir, temp);
			found = TRUE;
		}
	}

	if (found == FALSE) {
#ifndef windows
		strcpy(temp, "/usr/share/opvdm/phys");
#else
		strcpy(temp, "c:\\opvdm\\phys");
#endif

		theFolder = opendir(temp);
		if (theFolder != NULL) {
			closedir(theFolder);
			strcpy(physdir, temp);
			found = TRUE;
		}
	}

	if (found == FALSE) {
		ewe("No optical materials found\n");
	}

	join_path(2, file_path, physdir, in->suns_spectrum_file);

	inter_load(&(in->sun_read), file_path);
	inter_sort(&(in->sun_read));

	in->mat = (struct istruct *)malloc(in->layers * sizeof(struct istruct));
	in->mat_n =
	    (struct istruct *)malloc(in->layers * sizeof(struct istruct));

	double alpha_mul = 1.0;
	double n_mul = 1.0;
	double wavelength_shift_n = 0.0;
	double wavelength_shift_alpha = 0.0;
	int patch = FALSE;
	int inter = FALSE;
	struct inp_file inp;
	inp_init(&inp);
	char patch_file[400];
	char out_file[400];
	char token[400];
	int ii = 0;
	double b = 0.0;
	double a = 0.0;
	double c = 0.0;
	char type[40];
	int spectrum = FALSE;
	for (i = 0; i < in->layers; i++) {
		join_path(3, fit_file, physdir, in->material_dir_name[i],
			  "fit.inp");

		inp_load(&inp, fit_file);

		inp_search_double(&inp, &alpha_mul, "#alpha_mul");
		alpha_mul = fabs(alpha_mul);
		hard_limit("#alpha_mul", &alpha_mul);

		inp_search_double(&inp, &n_mul, "#n_mul");
		n_mul = fabs(n_mul);
		hard_limit("#n_mul", &n_mul);

		inp_search_double(&inp, &wavelength_shift_n,
				  "#wavelength_shift_n");
		wavelength_shift_n -= 40e-9;
		hard_limit("#wavelength_shift_n", &wavelength_shift_n);

		inp_search_double(&inp, &wavelength_shift_alpha,
				  "#wavelength_shift_alpha");
		wavelength_shift_alpha -= 40.0e-9;

		hard_limit("#wavelength_shift_alpha", &wavelength_shift_alpha);

		inp_search_int(&inp, &patch, "#patch");

		inp_search_int(&inp, &inter, "#inter");

		inp_search_int(&inp, &spectrum, "#spectrum");

		inp_free(&inp);

		join_path(3, file_path, physdir, in->material_dir_name[i],
			  "alpha.inp");
		inter_load(&(in->mat[i]), file_path);
		inter_sort(&(in->mat[i]));

		join_path(3, file_path, physdir, in->material_dir_name[i],
			  "n.inp");
		inter_load(&(in->mat_n[i]), file_path);
		inter_sort(&(in->mat_n[i]));

		inter_mul(&(in->mat[i]), alpha_mul);
		inter_add_x(&(in->mat[i]), wavelength_shift_alpha);

		inter_mul(&(in->mat_n[i]), n_mul);
		inter_add_x(&(in->mat_n[i]), wavelength_shift_n);

		if (patch == TRUE) {
			join_path(3, patch_file, physdir,
				  in->material_dir_name[i], "patch.inp");

			FILE *patch_in = fopen(patch_file, "r");
			if (in == NULL) {
				ewe("file %s not found\n", patch_file);
			}

			do {
				unused = fscanf(patch_in, "%s", token);
				if (strcmp(token, "#end") == 0) {
					break;
				}
				unused = fscanf(patch_in, "%s", type);

				unused = fscanf(patch_in, "%s", token);
				unused = fscanf(patch_in, "%le", &a);

				unused = fscanf(patch_in, "%s", token);
				unused = fscanf(patch_in, "%le", &b);

				unused = fscanf(patch_in, "%s", token);
				unused = fscanf(patch_in, "%le", &c);

				if (strcmp(type, "bar_n") == 0) {
					hard_limit(token, &c);
					c = fabs(c);
					for (ii = 0; ii < in->mat_n[i].len;
					     ii++) {
						if ((in->mat_n[i].x[ii] >= a)
						    && (in->mat_n[i].x[ii] <=
							b)) {
							in->mat_n[i].data[ii] =
							    c;
						}
					}
				} else if (strcmp(type, "bar_alpha") == 0) {
					hard_limit(token, &c);
					c = fabs(c);
					for (ii = 0; ii < in->mat[i].len; ii++) {
						if ((in->mat[i].x[ii] >= a)
						    && (in->mat[i].x[ii] <=
							b)) {
							in->mat[i].data[ii] = c;
						}
					}
				} else if (strcmp(type, "gaus") == 0) {
					hard_limit(token, &c);
					c = fabs(c);
					double add = 0.0;
					int max_pos =
					    inter_search_pos(&(in->mat_n[i]),
							     a);
					double subtract =
					    in->mat_n[i].data[max_pos];
					b = fabs(b);
					for (ii = 0; ii < in->mat_n[i].len;
					     ii++) {
						add =
						    (c -
						     subtract) *
						    exp(-pow
							(((in->mat_n[i].x[ii] -
							   a) / (sqrt(2.0) *
								 b)), 2.0));
						in->mat_n[i].data[ii] += add;

					}
				} else if (strcmp(type, "gaus_math") == 0) {
					printf("gaus math\n");
					double add = 0.0;
					b = fabs(b);
					for (ii = 0; ii < in->mat_n[i].len;
					     ii++) {
						add =
						    c *
						    exp(-pow
							(((in->mat_n[i].x[ii] -
							   a) / (sqrt(2.0) *
								 b)), 2.0));
						in->mat_n[i].data[ii] += add;

					}
				}
			} while (!feof(patch_in));

			if (strcmp(token, "#end") != 0) {
				printf("Error at end of patch file\n");
				exit(0);
			}

			fclose(patch_in);

			if (inter == TRUE) {

				join_path(3, patch_file, physdir,
					  in->material_dir_name[i],
					  "inter.inp");

				patch_in = fopen(patch_file, "r");
				if (in == NULL) {
					ewe("file %s not found\n", patch_file);
				}

				double from = 0.0;
				double to = 0.0;

				do {
					unused = fscanf(patch_in, "%s", token);
					if (strcmp(token, "#end") == 0) {
						break;
					}

					unused =
					    fscanf(patch_in, "%le %le", &from,
						   &to);

					int x0 =
					    inter_search_pos(&(in->mat_n[i]),
							     from);
					int x1 =
					    inter_search_pos(&(in->mat_n[i]),
							     to);
					double y0 = in->mat_n[i].data[x0];
					double y1 = in->mat_n[i].data[x1];
					double step =
					    (y1 - y0) / ((double)(x1 - x0));
					double pos = y0;
					for (ii = x0; ii < x1; ii++) {
						in->mat_n[i].data[ii] = pos;
						pos += step;
					}

					x0 = inter_search_pos(&(in->mat[i]),
							      from);
					x1 = inter_search_pos(&(in->mat[i]),
							      to);
					y0 = in->mat[i].data[x0];
					y1 = in->mat[i].data[x1];
					step = (y1 - y0) / ((double)(x1 - x0));
					pos = y0;
					for (ii = x0; ii < x1; ii++) {
						in->mat[i].data[ii] = pos;
						pos += step;
					}

				} while (!feof(patch_in));

				if (strcmp(token, "#end") != 0) {
					printf("Error at end of inter file\n");
					exit(0);
				}

				fclose(patch_in);
			}

			join_path(3, out_file, physdir,
				  in->material_dir_name[i], "n_out.dat");
			inter_save(&(in->mat_n[i]), out_file);

			join_path(3, out_file, physdir,
				  in->material_dir_name[i], "alpha_out.dat");
			inter_save(&(in->mat[i]), out_file);
		}

		if (spectrum == TRUE) {
			inter_free(&(in->mat_n[i]));

			join_path(3, patch_file, physdir,
				  in->material_dir_name[i], "n_spectrum.inp");

			FILE *f_in = fopen(patch_file, "r");

			if (f_in == NULL) {
				ewe("file %s not found\n", patch_file);
			}

			int n = 0;
			double value = 0.0;
			double start = 0.0;
			double stop = 0.0;
			unused = fscanf(f_in, "%s", token);
			unused = fscanf(f_in, "%le", &start);
			unused = fscanf(f_in, "%s", token);
			unused = fscanf(f_in, "%le", &stop);
			unused = fscanf(f_in, "%s", token);
			unused = fscanf(f_in, "%d", &n);
			unused = fscanf(f_in, "%s", token);
			inter_init_mesh(&(in->mat_n[i]), n, start, stop);
			for (ii = 0; ii < n; ii++) {
				unused = fscanf(f_in, "%le", &value);
				in->mat_n[i].data[ii] = value;
			}
			fclose(f_in);

			join_path(3, out_file, physdir,
				  in->material_dir_name[i], "n_out.dat");
			inter_save(&(in->mat_n[i]), out_file);

		}

	}

}

void light_free_materials(struct light *in)
{
	int i;
	for (i = 0; i < in->layers; i++) {
		inter_free(&(in->mat[i]));
		inter_free(&(in->mat_n[i]));
	}
	free(in->mat);
	free(in->mat_n);
}

void light_free_epitaxy(struct light *in)
{
	int i = 0;
	free(in->thick);
	free(in->G_percent);

	for (i = 0; i < in->layers; i++) {
		free(in->material_dir_name[i]);
	}

	free(in->material_dir_name);

}

void light_load_epitaxy(struct light *in, char *epi_file)
{

	FILE *epi = fopena(in->input_path, epi_file, "r");
	char temp[200];
	if (epi == NULL) {
		ewe("Optics epitaxy '%s' '%s' file not found\n", in->input_path,
		    epi_file);
	}
	int i = 0;

	unused = fscanf(epi, "%s", temp);
	unused = fscanf(epi, "%d", &in->layers);
	in->thick = (double *)malloc(in->layers * sizeof(double));
	in->G_percent = (double *)malloc(in->layers * sizeof(double));

	in->material_dir_name = (char **)malloc(in->layers * sizeof(char *));
	for (i = 0; i < in->layers; i++) {
		in->material_dir_name[i] = (char *)malloc(300.0 * sizeof(char));
	}

	in->ylen = 0.0;
	in->device_start = 0.0;
	int device_started = FALSE;
	in->device_ylen = 0.0;

	for (i = 0; i < in->layers; i++) {
		unused = fscanf(epi, "%s", temp);
		unused = fscanf(epi, "%le", &(in->thick[i]));

		in->thick[i] = fabs(in->thick[i]);
		hard_limit(temp, &(in->thick[i]));
		unused = fscanf(epi, "%s", temp);
		strcpy(in->material_dir_name[i], temp);

		int device;
		unused = fscanf(epi, "%d", &device);
		if (device == TRUE) {
			device_started = TRUE;
			in->device_ylen += in->thick[i];
		}

		if (device_started == FALSE) {
			in->device_start += in->thick[i];
			in->device_start_layer = i;
		}

		in->ylen += in->thick[i];
	}

	double ver;
	unused = fscanf(epi, "%s", temp);
	unused = fscanf(epi, "%lf", &(ver));

	if (ver != 1.11) {
		ewe("File compatability problem %s\n", "optics_epitaxy.inp");
	}

	unused = fscanf(epi, "%s", temp);
	if (strcmp("#end", temp) != 0) {
		ewe("Problem with device_epitaxy.inp file last item read %s!\n",
		    temp);
	}
	fclose(epi);
}

void light_calculate_complex_n(struct light *in)
{
	int i = 0;
	int ii = 0;
	double nc = 0.0;
	double kc = 0.0;

	double nr = 0.0;
	double kr = 0.0;
	double complex n0 = 0.0 + 0.0 * I;
	double complex n1 = 0.0 + 0.0 * I;

	for (i = 0; i < in->lpoints; i++) {
		for (ii = 0; ii < in->points; ii++) {
			if (ii == in->points - 1) {
				nr = in->n[i][ii];
				kr = in->alpha[i][ii] * (in->l[i] / (PI * 4.0));
			} else {
				nr = in->n[i][ii + 1];
				kr = in->alpha[i][ii +
						  1] * (in->l[i] / (PI * 4.0));
			}
			nc = in->n[i][ii];
			kc = in->alpha[i][ii] * (in->l[i] / (PI * 4.0));

			n0 = nc - kc * I;
			n1 = nr - kr * I;

			in->nbar[i][ii] = n0;

			in->r[i][ii] = (n0 - n1) / (n0 + n1);
			in->t[i][ii] = (2.0 * n0) / (n0 + n1);

		}
		memset(in->En[i], 0.0, in->points * sizeof(double));
		memset(in->Ep[i], 0.0, in->points * sizeof(double));
		memset(in->Enz[i], 0.0, in->points * sizeof(double));
		memset(in->Epz[i], 0.0, in->points * sizeof(double));

	}
}

void light_init_mesh(struct light *in)
{
	remove_dir(in->output_path, "light_dump", TRUE);
	int i;
	double ver = 0.0;
	double pos = 0.0;
	struct inp_file inp;

	sprintf(in->config_file, "%soptics.inp", in->output_path);
	printf("Load optics config %s\n", in->config_file);
	inp_init(&inp);
	inp_load_from_path(&inp, in->input_path, "optics.inp");
	inp_check(&inp, 1.84);

	inp_search_string(&inp, in->suns_spectrum_file, "#sun");

	light_load_materials(in);

	inp_search_int(&inp, &in->align_mesh, "#alignmesh");

	inp_search_int(&inp, &in->points, "#meshpoints");

	inp_search_int(&inp, &in->flip_field, "#flip_field");

	if (in->align_mesh == FALSE) {
		in->dx = in->ylen / ((double)in->points);
	} else {
		in->points = (int)(in->ylen / in->dx);
	}

	inp_search_int(&inp, &in->lpoints, "#lpoints");

	in->x = (double *)malloc(in->points * sizeof(double));
	in->H1d = (double *)malloc(in->points * sizeof(double));
	in->l = (double *)malloc(in->lpoints * sizeof(double));
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
	in->alpha0 = (double **)malloc(in->lpoints * sizeof(double *));
	in->photons = (double **)malloc(in->lpoints * sizeof(double *));
	in->pointing_vector = (double **)malloc(in->lpoints * sizeof(double *));
	in->E_tot_r = (double **)malloc(in->lpoints * sizeof(double *));
	in->E_tot_i = (double **)malloc(in->lpoints * sizeof(double *));
	in->n = (double **)malloc(in->lpoints * sizeof(double *));
	in->t =
	    (double complex **)malloc(in->lpoints * sizeof(double complex *));
	in->r =
	    (double complex **)malloc(in->lpoints * sizeof(double complex *));
	in->nbar =
	    (double complex **)malloc(in->lpoints * sizeof(double complex *));
	for (i = 0; i < in->lpoints; i++) {
		in->En[i] = (double *)malloc(in->points * sizeof(double));
		in->Enz[i] = (double *)malloc(in->points * sizeof(double));
		in->Ep[i] = (double *)malloc(in->points * sizeof(double));
		in->Epz[i] = (double *)malloc(in->points * sizeof(double));
		in->photons_asb[i] =
		    (double *)malloc(in->points * sizeof(double));
		in->alpha[i] = (double *)malloc(in->points * sizeof(double));
		in->alpha0[i] = (double *)malloc(in->points * sizeof(double));
		in->photons[i] = (double *)malloc(in->points * sizeof(double));
		in->pointing_vector[i] =
		    (double *)malloc(in->points * sizeof(double));
		in->E_tot_r[i] = (double *)malloc(in->points * sizeof(double));
		in->E_tot_i[i] = (double *)malloc(in->points * sizeof(double));
		in->n[i] = (double *)malloc(in->points * sizeof(double));
		in->t[i] =
		    (double complex *)malloc(in->points *
					     sizeof(double complex));
		in->r[i] =
		    (double complex *)malloc(in->points *
					     sizeof(double complex));
		in->nbar[i] =
		    (double complex *)malloc(in->points *
					     sizeof(double complex));
		in->H[i] = (double *)malloc(in->points * sizeof(double));

	}

	in->reflect = (double *)malloc(in->lpoints * sizeof(double));

	in->layer = (int *)malloc(in->points * sizeof(int));

	pos = in->dx;

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

	inp_search_double(&inp, &in->lstart, "#lstart");

	inp_search_double(&inp, &in->lstop, "#lstop");

	in->dl = (in->lstop - in->lstart) / ((double)in->lpoints);

	inp_search_double(&inp, &in->laser_wavelength, "#laserwavelength");
	in->laser_pos = (int)((in->laser_wavelength - in->lstart) / in->dl);

	inp_search_double(&inp, &in->spotx, "#spotx");

	inp_search_double(&inp, &in->spoty, "#spoty");

	inp_search_double(&inp, &in->pulseJ, "#pulseJ");

	pos = in->lstart;
	for (i = 0; i < in->lpoints; i++) {
		in->l[i] = pos;
		pos += in->dl;
	}

	int ii;

	for (i = 0; i < in->lpoints; i++) {
		for (ii = 0; ii < in->points; ii++) {
			in->alpha[i][ii] =
			    inter_get_noend(&(in->mat[in->layer[ii]]),
					    in->l[i]);
			in->alpha0[i][ii] = in->alpha[i][ii];
			in->n[i][ii] =
			    inter_get_noend(&(in->mat_n[in->layer[ii]]),
					    in->l[i]);
		}
	}

	light_calculate_complex_n(in);

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

	inp_search_int(&inp, &(in->gather), "#gather");

	inp_search_double(&inp, &(in->dlength), "#dlength");

	inp_search_double(&inp, &(in->Eg), "#Eg");

	inp_search_double(&inp, &(ver), "#ver");

	inp_free(&inp);

	in->N = 0.0;
	in->N += in->points;
	in->N += in->points - 1;
	in->N += in->points;

	in->N += in->points - 1;
	in->N += in->points;
	in->N += in->points - 1;

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
		in->sun[i] = in->sun_norm[i] * power * 1000.0;

		E = hp * cl / in->l[i];
		in->sun_photons[i] = in->sun[i] / E;
		if (i == in->laser_pos) {
			if (in->pulse_width != 0.0) {
				in->sun_photons[i] +=
				    laser_eff *
				    ((in->pulseJ / in->pulse_width / E) /
				     (in->spotx * in->spoty)) / in->dl;
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
