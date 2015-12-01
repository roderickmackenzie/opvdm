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
#include "i.h"
#include "buffer.h"
#include "pl.h"

static double light_energy = 0.0;

double calculate_photon_energy(struct istruct *in)
{
	int i;
	double tot = 0.0;

	for (i = 0; i < in->len; i++) {
		tot += in->data[i] * in->x[i] * Q;
	}

	return tot;
}

void exp_cal_emission(int number, struct device *in)
{

	double Re_h = 0.0;
	double Re_e = 0.0;
	double Rh_e = 0.0;
	double Rh_h = 0.0;

	double dEe_e = 0.0;
	double dEe_h = 0.0;
	double dEh_e = 0.0;
	double dEh_h = 0.0;
	char name[100];
	char out_dir[400];
	int i;
	int band;
	struct buffer buf;
	char temp[200];
	int mat = 0;
	double pl_fe_fh = 0.0;
	double pl_fe_te = 0.0;
	double pl_te_fh = 0.0;
	double pl_th_fe = 0.0;
	double pl_ft_th = 0.0;
	int pl_enabled = 0;

	double Vexternal = get_equiv_V(in);

	buffer_init(&buf);

	struct istruct fe_to_fh;
	struct istruct fe_to_te;
	struct istruct te_to_fh;
	struct istruct fh_to_th;
	struct istruct th_to_fe;

	double max_Eg = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		if (in->Eg[i] > max_Eg) {
			max_Eg = in->Eg[i];
		}
	}

	inter_init(&fe_to_fh);
	inter_init(&fe_to_te);
	inter_init(&te_to_fh);
	inter_init(&fh_to_th);
	inter_init(&th_to_fe);

	for (i = 0; i < in->ymeshpoints; i++) {
		mat = in->imat[i];
		pl_fe_fh = get_pl_fe_fh(mat);
		pl_fe_te = get_pl_fe_te(mat);
		pl_te_fh = get_pl_te_fh(mat);
		pl_th_fe = get_pl_th_fe(mat);
		pl_ft_th = get_pl_ft_th(mat);
		pl_enabled = get_pl_enabled(mat);

		if (pl_enabled == TRUE) {

			inter_append(&fe_to_fh, in->Eg[i],
				     in->Rfree[i] * pl_fe_fh);

			for (band = 0; band < in->srh_bands; band++) {

				dEe_e = -dos_get_band_energy_n(band, mat);
				Re_e =
				    (in->nt_r1[i][band] -
				     in->nt_r2[i][band]) * pl_fe_te;
				inter_append(&fe_to_te, dEe_e, Re_e);

				dEe_h = get_dos_Eg(mat) - dEe_e;
				Re_h =
				    (in->nt_r3[i][band] -
				     in->nt_r4[i][band]) * pl_te_fh;
				inter_append(&te_to_fh, dEe_h, Re_h);

				dEh_e = get_dos_Eg(mat) - dEh_h;
				Rh_e =
				    (in->pt_r3[i][band] -
				     in->pt_r4[i][band]) * pl_th_fe;
				inter_append(&th_to_fe, dEh_e, Rh_e);

				dEh_h = -dos_get_band_energy_p(band, mat);
				Rh_h =
				    (in->pt_r1[i][band] -
				     in->pt_r2[i][band]) * pl_ft_th;
				inter_append(&fh_to_th, dEh_h, Rh_h);

			}

		}
	}

	inter_mul(&fe_to_fh, in->ylen / ((double)in->ymeshpoints));
	inter_mul(&fe_to_te, in->ylen / ((double)in->ymeshpoints));
	inter_mul(&te_to_fh, in->ylen / ((double)in->ymeshpoints));
	inter_mul(&th_to_fe, in->ylen / ((double)in->ymeshpoints));
	inter_mul(&fh_to_th, in->ylen / ((double)in->ymeshpoints));

	sprintf(temp, "%d", number);
	join_path(3, out_dir, in->outputpath, "snapshots", temp);

	inter_sort(&fe_to_te);
	inter_sort(&te_to_fh);
	inter_sort(&th_to_fe);
	inter_sort(&fh_to_th);

	inter_join_bins(&fe_to_fh, 0.01);
	inter_join_bins(&fe_to_te, 0.01);
	inter_join_bins(&te_to_fh, 0.01);
	inter_join_bins(&th_to_fe, 0.01);
	inter_join_bins(&fh_to_th, 0.01);

	light_energy = 0.0;

	light_energy += calculate_photon_energy(&fe_to_fh) * in->area;
	light_energy += calculate_photon_energy(&fe_to_te) * in->area;
	light_energy += calculate_photon_energy(&te_to_fh) * in->area;
	light_energy += calculate_photon_energy(&th_to_fe) * in->area;
	light_energy += calculate_photon_energy(&fh_to_th) * in->area;

	buffer_malloc(&buf);
	sprintf(name, "%s", "fe_to_fh.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "PL Spectra Free electron to free hole");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Energy");
	strcpy(buf.y_label, "Intensity");
	strcpy(buf.x_units, "eV");
	strcpy(buf.y_units, "m^{-3}s^{-1}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, fe_to_fh.x, fe_to_fh.data, fe_to_fh.len);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "te_to_fh.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "PL Spectra Free hole to trapped electron");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Energy");
	strcpy(buf.y_label, "Intensity");
	strcpy(buf.x_units, "eV");
	strcpy(buf.y_units, "m^{-3}s^{-1}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, te_to_fh.x, te_to_fh.data, te_to_fh.len);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "fe_to_te.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "PL Spectra free electron to trapped electron");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Energy");
	strcpy(buf.y_label, "Intensity");
	strcpy(buf.x_units, "eV");
	strcpy(buf.y_units, "m^{-3}s^{-1}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, fe_to_te.x, fe_to_te.data, fe_to_te.len);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "th_to_fe.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "PL Spectra Free electron to trapped hole");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Energy");
	strcpy(buf.y_label, "Intensity");
	strcpy(buf.x_units, "eV");
	strcpy(buf.y_units, "m^{-3}s^{-1}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, th_to_fe.x, th_to_fe.data, th_to_fe.len);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "fh_to_th.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "PL Spectra free hole to trapped hole");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Energy");
	strcpy(buf.y_label, "Intensity");
	strcpy(buf.x_units, "eV");
	strcpy(buf.y_units, "m^{-3}s^{-1}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, fh_to_th.x, fh_to_th.data, fh_to_th.len);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	inter_free(&fe_to_fh);
	inter_free(&fe_to_te);
	inter_free(&te_to_fh);
	inter_free(&th_to_fe);
	inter_free(&fh_to_th);

	return;
}

double pl_get_light_energy()
{
	return light_energy;
}
