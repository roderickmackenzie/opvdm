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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <true_false.h>
#include "dump_ctrl.h"
#include "light.h"
#include "buffer.h"

void light_dump(struct light *in)
{
	FILE *out;
	int i;
	int ii;
	struct buffer buf;
	char out_dir[1024];
	char line[1024];
	if ((get_dump_status(dump_optics_verbose) == TRUE)
	    && (in->Gn[0] != 0.0)) {
		sprintf(out_dir, "%s/light_dump/", in->output_path);
		struct stat st = { 0 };

		if (stat(out_dir, &st) == -1) {
#ifdef windows
			mkdir(out_dir);
#else
			mkdir(out_dir, 0700);
#endif
		}

		out = fopena(out_dir, "light_2d_Ep.dat", "w");
		for (i = 0; i < in->lpoints; i++) {
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le %le\n", in->l[i],
					in->x[ii],
					pow(pow(in->Ep[i][ii], 2.0) +
					    pow(in->Epz[i][ii], 2.0), 0.5));

			}

			fprintf(out, "\n");
		}
		fclose(out);

		out = fopena(out_dir, "light_2d_En.dat", "w");
		for (i = 0; i < in->lpoints; i++) {
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le %le\n", in->l[i],
					in->x[ii],
					pow(pow(in->En[i][ii], 2.0) +
					    pow(in->Enz[i][ii], 2.0), 0.5));
			}

			fprintf(out, "\n");
		}
		fclose(out);

		out = fopena(out_dir, "light_2d_E_mod.dat", "w");
		for (i = 0; i < in->lpoints; i++) {
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le %le\n", in->l[i],
					in->x[ii],
					pow(pow
					    (in->Ep[i][ii] + in->En[i][ii],
					     2.0) + pow(in->Enz[i][ii] +
							in->Epz[i][ii], 2.0),
					    1.0));
			}

			fprintf(out, "\n");
		}
		fclose(out);

		buffer_init(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1e9;
		buf.x_mul = 1e9;
		strcpy(buf.title, "Photon density");
		strcpy(buf.type, "3d");
		strcpy(buf.x_label, "Position");
		strcpy(buf.y_label, "Wavelength");
		strcpy(buf.x_units, "$nm$");
		strcpy(buf.y_units, "$nm$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);

		for (i = 0; i < in->lpoints; i++) {
			for (ii = 0; ii < in->points; ii++) {
				sprintf(line, "%le %le %le\n", in->l[i],
					in->x[ii], in->photons[i][ii]);
				buffer_add_string(&buf, line);
			}

			buffer_add_string(&buf, "\n");
		}

		buffer_dump(out_dir, "light_2d_photons.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1e9;
		buf.x_mul = 1e9;
		strcpy(buf.title, "Adsorbed Photon density");
		strcpy(buf.type, "3d");
		strcpy(buf.x_label, "Position");
		strcpy(buf.y_label, "Wavelength");
		strcpy(buf.x_units, "$nm$");
		strcpy(buf.y_units, "$nm$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);

		for (i = 0; i < in->lpoints; i++) {
			for (ii = 0; ii < in->points; ii++) {
				sprintf(line, "%le %le %le\n", in->l[i],
					in->x[ii], in->photons_asb[i][ii]);
				buffer_add_string(&buf, line);
			}

			buffer_add_string(&buf, "\n");
		}

		buffer_dump(out_dir, "light_2d_photons_asb.dat", &buf);
		buffer_free(&buf);

		out = fopena(out_dir, "light_2d_n.dat", "w");
		for (i = 0; i < in->lpoints; i++) {
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le %le\n", in->l[i],
					in->x[ii], in->n[i][ii]);
			}

			fprintf(out, "\n");
		}
		fclose(out);

		out = fopena(out_dir, "light_lambda_sun.dat", "w");
		for (i = 0; i < in->lpoints; i++) {
			fprintf(out, "%le %le\n", in->l[i], in->sun[i]);
		}
		fclose(out);

		out = fopena(out_dir, "light_lambda_sun_norm.dat", "w");
		for (i = 0; i < in->lpoints; i++) {
			fprintf(out, "%le %le\n", in->l[i], in->sun_norm[i]);
		}
		fclose(out);

		out = fopena(out_dir, "light_lambda_sun_photons.dat", "w");
		for (i = 0; i < in->lpoints; i++) {
			fprintf(out, "%le %le\n", in->l[i], in->sun_photons[i]);
		}
		fclose(out);

		buffer_malloc(&buf);
		buf.y_mul = 1e9;
		buf.x_mul = 1e9;
		strcpy(buf.title, "Optical absorption coefficient");
		strcpy(buf.type, "3d");
		strcpy(buf.x_label, "Position");
		strcpy(buf.y_label, "Wavelength");
		strcpy(buf.x_units, "$nm$");
		strcpy(buf.y_units, "$nm$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);

		for (i = 0; i < in->lpoints; i++) {
			for (ii = 0; ii < in->points; ii++) {
				sprintf(line, "%le %le %le\n", in->l[i],
					in->x[ii], in->alpha[i][ii]);
				buffer_add_string(&buf, line);
			}

			buffer_add_string(&buf, "\n");
		}

		buffer_dump(out_dir, "light_lambda_alpha.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e9;
		strcpy(buf.title, "Wavelength - Reflected light");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Wavelength");
		strcpy(buf.y_label, "Reflected light");
		strcpy(buf.x_units, "nm");
		strcpy(buf.y_units, "a.u.");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, in->l, in->reflect, in->lpoints);
		buffer_dump(out_dir, "reflect.dat", &buf);
		buffer_free(&buf);

	}

}

void light_dump_1d(struct light *in, int i, char *ext)
{
	char out_dir[1024];
	char line[1024];
	char out_name[200];
	struct buffer data_photons;
	struct buffer data_photons_norm;
	struct buffer data_light_1d_Ep;
	struct buffer data_light_1d_En;
	struct buffer data_pointing;
	struct buffer data_E_tot;
	struct buffer data_1d_photons_tot;
	struct buffer data_r;
	struct buffer data_t;
	struct buffer buf;

	buffer_init(&data_photons);
	buffer_init(&data_photons_norm);
	buffer_init(&data_light_1d_Ep);
	buffer_init(&data_light_1d_En);
	buffer_init(&data_pointing);
	buffer_init(&data_E_tot);
	buffer_init(&data_1d_photons_tot);
	buffer_init(&data_r);
	buffer_init(&data_t);
	buffer_init(&buf);

	if ((get_dump_status(dump_optics) == TRUE) && (in->sun_E[i] != 0.0)) {

		sprintf(out_dir, "%s/light_dump/", in->output_path);
		struct stat st = { 0 };

		if (stat(out_dir, &st) == -1) {
#ifdef windows
			mkdir(out_dir);
#else
			mkdir(out_dir, 0700);
#endif
		}

		FILE *out;
		FILE *out2;
		int ii;
		char name[400];
		double max = 0.0;

		if (i == 0) {
			max = inter_array_get_max(in->photons_tot, in->points);

			buffer_malloc(&data_1d_photons_tot);
			buffer_malloc(&buf);
			buf.y_mul = 1.0;
			buf.x_mul = 1e9;
			strcpy(buf.title, "Normalized photon density");
			strcpy(buf.type, "xy");
			strcpy(buf.x_label, "Position");
			strcpy(buf.y_label, "Hole current density");
			strcpy(buf.x_units, "$nm$");
			strcpy(buf.y_units, "$a.u.$");
			buf.logscale_x = 0;
			buf.logscale_y = 0;
			buffer_add_info(&buf);

			for (ii = 0; ii < in->points; ii++) {
				sprintf(line, "%le %le\n",
					in->x[ii] - in->device_start,
					in->photons_tot[ii] / max);
				buffer_add_string(&buf, line);
			}

			sprintf(out_name, "light_1d_photons_tot_norm%s.dat",
				ext);
			buffer_dump(out_dir, out_name, &buf);
			buffer_free(&buf);

			char name_1d_photons_tot[200];
			sprintf(name_1d_photons_tot,
				"light_1d_photons_tot%s.dat", ext);

			for (ii = 0; ii < in->points; ii++) {
				sprintf(line, "%le %le\n",
					in->x[ii] - in->device_start,
					in->photons_tot[ii]);
				buffer_add_string(&data_1d_photons_tot, line);

			}

			buffer_dump(out_dir, name_1d_photons_tot,
				    &data_1d_photons_tot);
		}

		buffer_malloc(&data_photons);
		buffer_malloc(&data_photons_norm);
		buffer_malloc(&data_light_1d_Ep);
		buffer_malloc(&data_light_1d_En);
		buffer_malloc(&data_pointing);
		buffer_malloc(&data_E_tot);
		buffer_malloc(&data_r);
		buffer_malloc(&data_t);

		char name_photons[200];
		char name_photons_norm[200];
		char name_light_1d_Ep[200];
		char name_light_1d_En[200];
		char name_pointing[200];
		char name_E_tot[200];
		char name_r[200];
		char name_t[200];

		sprintf(name_photons, "light_1d_%.0f_photons%s.dat",
			in->l[i] * 1e9, ext);
		sprintf(name_photons_norm, "light_1d_%.0f_photons%s_norm.dat",
			in->l[i] * 1e9, ext);
		sprintf(name_light_1d_Ep, "light_1d_%.0f_Ep%s.dat",
			in->l[i] * 1e9, ext);
		sprintf(name_light_1d_En, "light_1d_%.0f_En%s.dat",
			in->l[i] * 1e9, ext);
		sprintf(name_pointing, "light_1d_%.0f_pointing%s.dat",
			in->l[i] * 1e9, ext);
		sprintf(name_E_tot, "light_1d_%.0f_E_tot%s.dat", in->l[i] * 1e9,
			ext);
		sprintf(name_r, "light_1d_%.0f_r%s.dat", in->l[i] * 1e9, ext);
		sprintf(name_t, "light_1d_%.0f_t%s.dat", in->l[i] * 1e9, ext);

		max = inter_array_get_max(in->photons[i], in->points);
		for (ii = 0; ii < in->points; ii++) {
			sprintf(line, "%le %le\n", in->x[ii],
				in->photons[i][ii]);
			buffer_add_string(&data_photons, line);

			sprintf(line, "%le %le\n", in->x[ii] - in->device_start,
				in->photons[i][ii] / max);
			buffer_add_string(&data_photons_norm, line);

			sprintf(line, "%le %le %le %le\n", in->x[ii],
				pow(pow(in->Ep[i][ii], 2.0) +
				    pow(in->Epz[i][ii], 2.0), 0.5),
				in->Ep[i][ii], in->Epz[i][ii]);
			buffer_add_string(&data_light_1d_Ep, line);

			sprintf(line, "%le %le %le %le\n", in->x[ii],
				pow(pow(in->En[i][ii], 2.0) +
				    pow(in->Enz[i][ii], 2.0), 0.5),
				in->En[i][ii], in->Enz[i][ii]);
			buffer_add_string(&data_light_1d_En, line);

			sprintf(line, "%le %le\n", in->x[ii],
				in->pointing_vector[i][ii]);
			buffer_add_string(&data_pointing, line);

			sprintf(line, "%le %le %le\n", in->x[ii],
				in->E_tot_r[i][ii], in->E_tot_i[i][ii]);
			buffer_add_string(&data_E_tot, line);

			sprintf(line, "%le %le %le %le\n", in->x[ii],
				cabs(in->r[i][ii]), creal(in->r[i][ii]),
				cimag(in->r[i][ii]));
			buffer_add_string(&data_r, line);

			sprintf(line, "%le %le %le %le\n", in->x[ii],
				cabs(in->t[i][ii]), creal(in->t[i][ii]),
				cimag(in->t[i][ii]));
			buffer_add_string(&data_t, line);
		}

		buffer_dump(out_dir, name_photons, &data_photons);
		buffer_dump(out_dir, name_photons_norm, &data_photons_norm);
		buffer_dump(out_dir, name_light_1d_Ep, &data_light_1d_Ep);
		buffer_dump(out_dir, name_light_1d_En, &data_light_1d_En);
		buffer_dump(out_dir, name_pointing, &data_pointing);
		buffer_dump(out_dir, name_E_tot, &data_E_tot);
		buffer_dump(out_dir, name_r, &data_r);
		buffer_dump(out_dir, name_t, &data_t);

		buffer_free(&data_photons);
		buffer_free(&data_photons_norm);
		buffer_free(&data_light_1d_Ep);
		buffer_free(&data_light_1d_En);
		buffer_free(&data_pointing);
		buffer_free(&data_E_tot);
		buffer_free(&data_r);
		buffer_free(&data_t);

		if (i == 0) {
			buffer_free(&data_1d_photons_tot);
		}

		if (get_dump_status(dump_optics_verbose) == TRUE) {
			sprintf(name, "%s/light_1d_%.0f_layer%s.dat", out_dir,
				in->l[i] * 1e9, ext);
			out = fopen(name, "w");
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %d\n", in->x[ii],
					in->layer[ii]);
			}
			fclose(out);

			sprintf(name, "%s/light_1d_%.0f_Gn%s.dat", out_dir,
				in->l[i] * 1e9, ext);
			out = fopen(name, "w");
			sprintf(name, "%s/light_1d_%.0f_Gn%s_norm.dat", out_dir,
				in->l[i] * 1e9, ext);
			out2 = fopen(name, "w");
			max = inter_array_get_max(in->Gn, in->points);
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le\n",
					in->x[ii] - in->device_start,
					in->Gn[ii]);
				fprintf(out2, "%le %le\n",
					in->x[ii] - in->device_start,
					in->Gn[ii] / max);
			}
			fclose(out);
			fclose(out2);

			sprintf(name, "%s/light_1d_%.0f_Gp%s.dat", out_dir,
				in->l[i] * 1e9, ext);
			out = fopen(name, "w");
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le\n", in->x[ii],
					in->Gp[ii]);
			}
			fclose(out);

			sprintf(name, "%s/light_1d_%.0f_E%s.dat", out_dir,
				in->l[i] * 1e9, ext);
			out = fopen(name, "w");
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le\n", in->x[ii],
					pow(pow
					    (in->Ep[i][ii] + in->En[i][ii],
					     2.0) + pow(in->Enz[i][ii] +
							in->Epz[i][ii], 2.0),
					    0.5));
			}
			fclose(out);

			sprintf(name, "%s/light_1d_%.0f_t%s.dat", out_dir,
				in->l[i] * 1e9, ext);
			out = fopen(name, "w");
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le\n", in->x[ii],
					cabs(in->t[i][ii]));
			}
			fclose(out);

			sprintf(name, "%s/light_1d_%.0f_r%s.dat", out_dir,
				in->l[i] * 1e9, ext);
			out = fopen(name, "w");
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le\n", in->x[ii],
					cabs(in->r[i][ii]));
			}
			fclose(out);

			sprintf(name, "%s/light_1d_%.0f_photons_abs%s.dat",
				out_dir, in->l[i] * 1e9, ext);
			out = fopen(name, "w");
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le\n", in->x[ii],
					in->photons_asb[i][ii]);
			}
			fclose(out);

			sprintf(name, "%s/light_1d_%.0f_n%s.dat", out_dir,
				in->l[i] * 1e9, ext);
			out = fopen(name, "w");

			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le\n", in->x[ii],
					in->n[i][ii]);
			}

			fclose(out);

			sprintf(name, "%s/light_1d_%.0f_alpha%s.dat", out_dir,
				in->l[i] * 1e9, ext);
			out = fopen(name, "w");

			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le\n", in->x[ii],
					in->alpha[i][ii]);
			}

			fclose(out);

			out =
			    fopena(out_dir, "light_sun_wavelength_E.dat", "w");
			for (ii = 0; ii < in->lpoints; ii++) {
				fprintf(out, "%le %le\n", in->l[ii],
					in->sun_E[ii]);
			}
			fclose(out);

		}

	}

}
