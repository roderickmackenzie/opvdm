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
#include <zip.h>
#include "dump_ctrl.h"
#include "light.h"
#include "buffer.h"

void light_dump(struct light *in)
{
	char out_dir[1024];
	if ((get_dump_status(dump_optics_verbose) == TRUE)
	    && (in->Gn[0] != 0.0)) {
		sprintf(out_dir, "%s/light_dump/", in->output_path);
		struct stat st = { 0 };

		if (stat(out_dir, &st) == -1) {
			mkdir(out_dir, 0700);
		}

		FILE *out;
		int i;
		int ii;

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

		out = fopena(out_dir, "light_2d_photons.dat", "w");
		for (i = 0; i < in->lpoints; i++) {
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le %le\n", in->l[i],
					in->x[ii], in->photons[i][ii]);
			}

			fprintf(out, "\n");
		}
		fclose(out);

		out = fopena(out_dir, "light_2d_photons_asb.dat", "w");
		for (i = 0; i < in->lpoints; i++) {
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le %le\n", in->l[i],
					in->x[ii], in->photons_asb[i][ii]);
			}

			fprintf(out, "\n");
		}
		fclose(out);

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

		out = fopena(out_dir, "light_lambda_alpha.dat", "w");
		for (i = 0; i < in->lpoints; i++) {
			for (ii = 0; ii < in->points; ii++) {
				fprintf(out, "%le %le %le\n", in->l[i],
					in->x[ii], in->alpha[i][ii]);
			}

			fprintf(out, "\n");
		}
		fclose(out);
	}
}

void light_dump_1d(struct light *in, int i, char *ext)
{
	char out_dir[1024];
	char line[1024];

	char zip_file[1024];
	struct zip_source *s;
	struct buffer data_photons;
	struct buffer data_photons_norm;
	struct buffer data_light_1d_Ep;
	struct buffer data_light_1d_En;
	struct buffer data_pointing;
	struct buffer data_E_tot;
	struct buffer data_1d_photons_tot;
	struct buffer data_1d_photons_tot_norm;

	if ((get_dump_status(dump_optics) == TRUE) && (in->sun_E[i] != 0.0)) {

		sprintf(out_dir, "%s/light_dump/", in->output_path);
		struct stat st = { 0 };

		if (stat(out_dir, &st) == -1) {

			mkdir(out_dir, 0700);
		}

		int err = 0;
		sprintf(zip_file, "%s/light.zip", out_dir);
		struct zip *z;

		z = zip_open(zip_file, ZIP_CREATE, &err);

		FILE *out;
		FILE *out2;
		int ii;
		char name[400];
		double max = 0.0;

		if (i == 0) {
			char name_1d_photons_tot[200];
			char name_1d_photons_tot_norm[200];

			buffer_init(&data_1d_photons_tot);
			buffer_init(&data_1d_photons_tot_norm);

			sprintf(name_1d_photons_tot,
				"light_1d_photons_tot%s.dat", ext);
			sprintf(name_1d_photons_tot_norm,
				"light_1d_photons_tot_norm%s.dat", ext);

			max = inter_array_get_max(in->photons_tot, in->points);
			for (ii = 0; ii < in->points; ii++) {
				sprintf(line, "%le %le\n",
					in->x[ii] - in->device_start,
					in->photons_tot[ii]);
				buffer_add_string(&data_1d_photons_tot, line);

				sprintf(line, "%le %le\n",
					in->x[ii] - in->device_start,
					in->photons_tot[ii] / max);
				buffer_add_string(&data_1d_photons_tot_norm,
						  line);
			}

			s = zip_source_buffer(z, data_1d_photons_tot.buf,
					      data_1d_photons_tot.len, 0);
			zip_add(z, name_1d_photons_tot, s);

			s = zip_source_buffer(z, data_1d_photons_tot_norm.buf,
					      data_1d_photons_tot_norm.len, 0);
			zip_add(z, name_1d_photons_tot_norm, s);

		}

		buffer_init(&data_photons);
		buffer_init(&data_photons_norm);
		buffer_init(&data_light_1d_Ep);
		buffer_init(&data_light_1d_En);
		buffer_init(&data_pointing);
		buffer_init(&data_E_tot);

		char name_photons[200];
		char name_photons_norm[200];
		char name_light_1d_Ep[200];
		char name_light_1d_En[200];
		char name_pointing[200];
		char name_E_tot[200];

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
		}

		s = zip_source_buffer(z, data_photons.buf, data_photons.len, 0);
		zip_add(z, name_photons, s);

		s = zip_source_buffer(z, data_photons_norm.buf,
				      data_photons_norm.len, 0);
		zip_add(z, name_photons_norm, s);

		s = zip_source_buffer(z, data_light_1d_Ep.buf,
				      data_light_1d_Ep.len, 0);
		zip_add(z, name_light_1d_Ep, s);

		s = zip_source_buffer(z, data_light_1d_En.buf,
				      data_light_1d_En.len, 0);
		zip_add(z, name_light_1d_En, s);

		s = zip_source_buffer(z, data_pointing.buf, data_pointing.len,
				      0);
		zip_add(z, name_pointing, s);

		s = zip_source_buffer(z, data_E_tot.buf, data_E_tot.len, 0);
		zip_add(z, name_E_tot, s);

		zip_close(z);

		buffer_free(&data_photons);
		buffer_free(&data_photons_norm);
		buffer_free(&data_light_1d_Ep);
		buffer_free(&data_light_1d_En);
		buffer_free(&data_pointing);
		buffer_free(&data_E_tot);

		if (i == 0) {
			buffer_free(&data_1d_photons_tot);
			buffer_free(&data_1d_photons_tot_norm);
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
