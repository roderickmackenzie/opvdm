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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "sim.h"
#include "dump.h"
#include "buffer.h"
#include "dynamic_store.h"
static int unused __attribute__ ((unused));

void dump_dynamic_init(struct dynamic_store *store, struct device *in)
{
	int i = 0;

	if (get_dump_status(dump_dynamic) == TRUE) {
		inter_init(&(store->charge_change));
		inter_init(&(store->jout));
		inter_init(&(store->jn_avg));
		inter_init(&(store->jp_avg));
		inter_init(&(store->dynamic_jn));
		inter_init(&(store->dynamic_jp));
		inter_init(&(store->jnout_mid));
		inter_init(&(store->jpout_mid));
		inter_init(&(store->iout));
		inter_init(&(store->iout_left));
		inter_init(&(store->iout_right));
		inter_init(&(store->gexout));
		inter_init(&(store->ntrap));
		inter_init(&(store->ptrap));
		inter_init(&(store->ntrap_delta_out));
		inter_init(&(store->ptrap_delta_out));
		inter_init(&(store->nfree));
		inter_init(&(store->pfree));
		inter_init(&(store->nfree_delta_out));
		inter_init(&(store->pfree_delta_out));
		inter_init(&(store->Rnpout));
		inter_init(&(store->nfree_to_ptrap));
		inter_init(&(store->pfree_to_ntrap));
		inter_init(&(store->Rnout));
		inter_init(&(store->Rpout));
		inter_init(&(store->nrelax_out));
		inter_init(&(store->prelax_out));
		inter_init(&(store->tpc_mue));
		inter_init(&(store->tpc_muh));
		inter_init(&(store->tpc_mu_avg));
		inter_init(&(store->tpc_filledn));
		inter_init(&(store->tpc_filledp));
		inter_init(&(store->dynamic_np));
		inter_init(&(store->only_n));
		inter_init(&(store->only_p));
		inter_init(&(store->E_field));
		inter_init(&(store->dynamic_Vapplied));
		inter_init(&(store->dynamic_charge_tot));
		inter_init(&(store->dynamic_pl));
		inter_init(&(store->dynamic_jn_drift));
		inter_init(&(store->dynamic_jn_diffusion));

		inter_init(&(store->dynamic_jp_drift));
		inter_init(&(store->dynamic_jp_diffusion));

		inter_init(&(store->dynamic_qe));

		inter_init(&(store->srh_n_r1));
		inter_init(&(store->srh_n_r2));
		inter_init(&(store->srh_n_r3));
		inter_init(&(store->srh_n_r4));

		inter_init(&(store->srh_p_r1));
		inter_init(&(store->srh_p_r2));
		inter_init(&(store->srh_p_r3));
		inter_init(&(store->srh_p_r4));

		inter_init(&(store->band_bend));

		store->band_snapshot = malloc(sizeof(double) * in->ymeshpoints);
		for (i = 0; i < in->ymeshpoints; i++) {
			store->band_snapshot[i] = in->phi[i];
		}
	}
}

void dump_dynamic_save(char *outputpath, struct dynamic_store *store)
{
	int i;
	int sub = TRUE;
	char temp[200];
	struct buffer buf;
	buffer_init(&buf);

	if (get_dump_status(dump_dynamic) == TRUE) {

		if (get_dump_status(dump_norm_time_to_one) == TRUE) {
			buf.norm_x_axis = TRUE;
		}

		if (get_dump_status(dump_norm_y_axis) == TRUE) {
			buf.norm_y_axis = TRUE;
		}

		char out_dir[1000];
		sprintf(out_dir, "%s/dynamic/", outputpath);
		struct stat st = { 0 };

		if (stat(out_dir, &st) == -1) {
#ifdef windows
			mkdir(out_dir);
#else
			mkdir(out_dir, 0700);
#endif
		}

		char outpath[200];

		sprintf(outpath, "%s%s", out_dir, "dynamic_jn_mid.dat");
		inter_save(&(store->jnout_mid), outpath);

		struct istruct one;
		inter_copy(&one, &(store->jnout_mid), TRUE);
		inter_deriv(&one, &(store->jnout_mid));
		sprintf(outpath, "%s%s", out_dir, "dynamic_djn.dat");
		inter_save(&one, outpath);
		inter_free(&one);

		sprintf(outpath, "%s%s", out_dir, "dynamic_jp_mid.dat");
		inter_save(&(store->jpout_mid), outpath);

		inter_copy(&one, &(store->jpout_mid), TRUE);
		inter_deriv(&one, &(store->jpout_mid));
		sprintf(outpath, "%s%s", out_dir, "dynamic_djp.dat");
		inter_save(&one, outpath);
		inter_free(&one);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Hole drift plus diffusion current");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Hole current density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$A m^{-2}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);

		for (i = 0; i < (store->dynamic_jp_drift).len; i++) {
			sprintf(temp, "%e %e\n", (store->dynamic_jp_drift).x[i],
				(store->dynamic_jp_drift).data[i] +
				(store->dynamic_jp_diffusion).data[i]);
			buffer_add_string(&buf, temp);
		}
		buffer_dump(out_dir, "dynamic_jp_drift_plus_diffusion.dat",
			    &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Electron drift plus diffusion current");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Hole current density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$A m^{-2}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		for (i = 0; i < (store->dynamic_jn_drift).len; i++) {
			sprintf(temp, "%e %e\n", (store->dynamic_jn_drift).x[i],
				(store->dynamic_jn_drift).data[i] +
				(store->dynamic_jn_diffusion).data[i]);
			buffer_add_string(&buf, temp);
		}
		buffer_dump(out_dir, "dynamic_jn_drift_plus_diffusion.dat",
			    &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Current density at contacts");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Current density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$A m^{-2}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		if (sub == TRUE) {
			inter_sub_double(&(store->jout), (store->jout).data[0]);
			inter_mul(&(store->jout), -1.0);
		}
		buffer_add_xy_data(&buf, (store->jout).x, (store->jout).data,
				   (store->jout).len);
		buffer_dump(out_dir, "dynamic_j.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Change in charge distribution");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "percent");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$\\%$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->charge_change).x,
				   (store->charge_change).data,
				   (store->charge_change).len);
		buffer_dump(out_dir, "dynamic_charge_change.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Drift current");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Electron current density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$A m^{-2}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->dynamic_jn_drift).x,
				   (store->dynamic_jn_drift).data,
				   (store->dynamic_jn_drift).len);
		buffer_dump(out_dir, "dynamic_jn_drift.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Diffusion current");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Electron current density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$A m^{-2}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->dynamic_jn_diffusion).x,
				   (store->dynamic_jn_diffusion).data,
				   (store->dynamic_jn_diffusion).len);
		buffer_dump(out_dir, "dynamic_jn_diffusion.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Drift current");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Hole current density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$A m^{-2}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->dynamic_jp_drift).x,
				   (store->dynamic_jp_drift).data,
				   (store->dynamic_jp_drift).len);
		buffer_dump(out_dir, "dynamic_jp_drift.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Diffusion current");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Hole current density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$A m^{-2}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->dynamic_jp_diffusion).x,
				   (store->dynamic_jp_diffusion).data,
				   (store->dynamic_jp_diffusion).len);
		buffer_dump(out_dir, "dynamic_jp_diffusion.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Jn contacts");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Electron current density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$A m^{-2}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->dynamic_jn).x,
				   (store->dynamic_jn).data,
				   (store->dynamic_jn).len);
		buffer_dump(out_dir, "dynamic_jn_contacts.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Jp contacts");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Hole current density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$A m^{-2}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->dynamic_jp).x,
				   (store->dynamic_jp).data,
				   (store->dynamic_jp).len);
		buffer_dump(out_dir, "dynamic_jp_contacts.dat", &buf);
		buffer_free(&buf);

		sprintf(outpath, "%s%s", out_dir, "dynamic_jn_avg.dat");
		inter_save(&(store->jn_avg), outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_jp_avg.dat");
		inter_save(&(store->jp_avg), outpath);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "External Current");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Current");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$Amps$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->iout).x, (store->iout).data,
				   (store->iout).len);
		buffer_dump(out_dir, "dynamic_i.dat", &buf);
		buffer_free(&buf);

		sprintf(outpath, "%s%s", out_dir, "dynamic_i_left.dat");
		inter_save(&(store->iout_left), outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_i_right.dat");
		inter_save(&(store->iout_right), outpath);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Free carrier generation rate");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Generation rate");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}s^{-1}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->gexout).x,
				   (store->gexout).data, (store->gexout).len);
		buffer_dump(out_dir, "dynamic_gex.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Dynamic quantum efficency");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Percent");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$\%$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->dynamic_qe).x,
				   (store->dynamic_qe).data,
				   (store->dynamic_qe).len);
		buffer_dump(out_dir, "dynamic_qe.dat", &buf);
		buffer_free(&buf);

		double sum = inter_intergrate(&(store->nfree_to_ptrap));
		FILE *out = fopen("dynamic_Rn_int.dat", "w");
		fprintf(out, "%le", sum);
		fclose(out);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Free hole recombination");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Recombination");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}s^{-1}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->pfree_to_ntrap).x,
				   (store->pfree_to_ntrap).data,
				   (store->pfree_to_ntrap).len);
		buffer_dump(out_dir, "dynamic_pf_to_nt.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Free electron recombination");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Recombination");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}s^{-1}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->nfree_to_ptrap).x,
				   (store->nfree_to_ptrap).data,
				   (store->nfree_to_ptrap).len);
		buffer_dump(out_dir, "dynamic_nf_to_pt.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Free electron loss - time");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Free electron loss");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}s^{-1}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->Rnout).x, (store->Rnout).data,
				   (store->Rnout).len);
		buffer_dump(out_dir, "dynamic_Rn.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Free hole loss - time");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Free hole loss");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}s^{-1}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->Rpout).x, (store->Rpout).data,
				   (store->Rpout).len);
		buffer_dump(out_dir, "dynamic_Rp.dat", &buf);
		buffer_free(&buf);

		sum = inter_intergrate(&(store->pfree_to_ntrap));
		out = fopen("dynamic_Rp_int.dat", "w");
		fprintf(out, "%le", sum);
		fclose(out);

		inter_make_cumulative(&(store->nfree_to_ptrap));

		sprintf(outpath, "%s%s", out_dir, "dynamic_Rn_cumulative.dat");
		inter_save(&(store->nfree_to_ptrap), outpath);

		inter_make_cumulative(&(store->pfree_to_ntrap));

		sprintf(outpath, "%s%s", out_dir, "dynamic_Rp_cumulative.dat");
		inter_save(&(store->pfree_to_ntrap), outpath);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Electron relaxation");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Relaxation");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}s^{-1}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->nrelax_out).x,
				   (store->nrelax_out).data,
				   (store->nrelax_out).len);
		buffer_dump(out_dir, "dynamic_nrelax.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "Hole relaxation");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Relaxation");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "$m^{-3}s^{-1}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->prelax_out).x,
				   (store->prelax_out).data,
				   (store->prelax_out).len);
		buffer_dump(out_dir, "dynamic_prelax.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Trapped electron density");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Electron density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->ntrap).x, (store->ntrap).data,
				   (store->ntrap).len);
		buffer_dump(out_dir, "dynamic_nt.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Trapped hole density");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Hole density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->ptrap).x, (store->ptrap).data,
				   (store->ptrap).len);
		buffer_dump(out_dir, "dynamic_pt.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Free electron density");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Electron density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->nfree).x, (store->nfree).data,
				   (store->nfree).len);
		buffer_dump(out_dir, "dynamic_nf.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Free hole density");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Hole density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->pfree).x, (store->pfree).data,
				   (store->pfree).len);
		buffer_dump(out_dir, "dynamic_pf.dat", &buf);
		buffer_free(&buf);

		sprintf(outpath, "%s%s", out_dir, "dynamic_nfree_delta.dat");
		inter_save(&(store->nfree_delta_out), outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_pfree_delta.dat");
		inter_save(&(store->pfree_delta_out), outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_ntrap_delta.dat");
		inter_save(&(store->ntrap_delta_out), outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_ptrap_delta.dat");
		inter_save(&(store->ptrap_delta_out), outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_filledn.dat");
		inter_save(&(store->tpc_filledn), outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_Rn-p.dat");
		inter_save(&(store->Rnpout), outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_filledp.dat");
		inter_save(&(store->tpc_filledp), outpath);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Electron mobility");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Mobility");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{2}V^{-1}s^{-1}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->tpc_mue).x,
				   (store->tpc_mue).data, (store->tpc_mue).len);
		buffer_dump(out_dir, "dynamic_mue.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Hole mobility");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Mobility");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{2}V^{-1}s^{-1}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->tpc_muh).x,
				   (store->tpc_muh).data, (store->tpc_muh).len);
		buffer_dump(out_dir, "dynamic_muh.dat", &buf);
		buffer_free(&buf);

		sprintf(outpath, "%s%s", out_dir, "dynamic_mu_avg.dat");
		inter_save(&(store->tpc_mu_avg), outpath);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Total electron density");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Electron density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->only_n).x,
				   (store->only_n).data, (store->only_n).len);
		buffer_dump(out_dir, "dynamic_n.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Total hole density");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Hole density");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$m^{-3}$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->only_p).x,
				   (store->only_p).data, (store->only_p).len);
		buffer_dump(out_dir, "dynamic_p.dat", &buf);
		buffer_free(&buf);

		sprintf(outpath, "%s%s", out_dir, "dynamic_np.dat");
		inter_save(&(store->dynamic_np), outpath);

		inter_norm(&(store->dynamic_np), 1.0);
		sprintf(outpath, "%s%s", out_dir, "dynamic_np_norm.dat");
		inter_save(&(store->dynamic_np), outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_E_field.dat");
		inter_div_double(&(store->E_field), (store->E_field).data[0]);
		inter_save(&(store->E_field), outpath);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, "Voltage applied to diode");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "Voltage");
		strcpy(buf.x_units, "$\\mu s$");
		strcpy(buf.y_units, "$V$");
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->dynamic_Vapplied).x,
				   (store->dynamic_Vapplied).data,
				   (store->dynamic_Vapplied).len);
		buffer_dump(out_dir, "dynamic_Vapplied.dat", &buf);
		buffer_free(&buf);

		sprintf(outpath, "%s%s", out_dir, "dynamic_charge_tot.dat");
		inter_sub_double(&(store->dynamic_charge_tot),
				 (store->dynamic_charge_tot).data[0]);
		inter_save(&(store->dynamic_charge_tot), outpath);

		inter_chop(&(store->dynamic_pl), 1.0e-9, 1.0);
		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "PL intensity");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "PL Intensity");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "au");
		buf.logscale_x = 1;
		buf.logscale_y = 1;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->dynamic_pl).x,
				   (store->dynamic_pl).data,
				   (store->dynamic_pl).len);
		buffer_dump(out_dir, "dynamic_pl.dat", &buf);
		buffer_free(&buf);

		double max = inter_get_max(&(store->dynamic_pl));
		inter_div_double(&(store->dynamic_pl), max);
		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "PL intensity normalized");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "PL Intensity");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "au");
		buf.logscale_x = 1;
		buf.logscale_y = 1;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->dynamic_pl).x,
				   (store->dynamic_pl).data,
				   (store->dynamic_pl).len);
		buffer_dump(out_dir, "dynamic_pl_norm.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "time v.s. srh_n_r1");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "srh_n_r1");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "m^{-3} s^{-1}");
		buf.logscale_x = 1;
		buf.logscale_y = 1;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->srh_n_r1).x,
				   (store->srh_n_r1).data,
				   (store->srh_n_r1).len);
		buffer_dump(out_dir, "dynamic_srh_n_r1.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "time v.s. srh_n_r2");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "srh_n_r2");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "m^{-3}s^{-1}");
		buf.logscale_x = 1;
		buf.logscale_y = 1;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->srh_n_r2).x,
				   (store->srh_n_r2).data,
				   (store->srh_n_r2).len);
		buffer_dump(out_dir, "dynamic_srh_n_r2.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "time v.s. srh_n_r3");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "srh_n_r3");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "m^{-3}s^{-1}");
		buf.logscale_x = 1;
		buf.logscale_y = 1;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->srh_n_r3).x,
				   (store->srh_n_r3).data,
				   (store->srh_n_r3).len);
		buffer_dump(out_dir, "dynamic_srh_n_r3.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "time v.s. srh_n_r4");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "srh_n_r4");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "m^{-3}s^{-1}");
		buf.logscale_x = 1;
		buf.logscale_y = 1;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->srh_n_r4).x,
				   (store->srh_n_r4).data,
				   (store->srh_n_r4).len);
		buffer_dump(out_dir, "dynamic_srh_n_r4.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "time v.s. srh_p_r1");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "srh_p_r1");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "m^{-3}s^{-1}");
		buf.logscale_x = 1;
		buf.logscale_y = 1;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->srh_p_r1).x,
				   (store->srh_p_r1).data,
				   (store->srh_p_r1).len);
		buffer_dump(out_dir, "dynamic_srh_p_r1.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "time v.s. srh_p_r2");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "srh_p_r2");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "m^{-3}s^{-1}");
		buf.logscale_x = 1;
		buf.logscale_y = 1;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->srh_p_r2).x,
				   (store->srh_p_r2).data,
				   (store->srh_p_r2).len);
		buffer_dump(out_dir, "dynamic_srh_p_r2.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "time v.s. srh_p_r3");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "srh_p_r3");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "m^{-3}s^{-1}");
		buf.logscale_x = 1;
		buf.logscale_y = 1;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->srh_p_r3).x,
				   (store->srh_p_r3).data,
				   (store->srh_p_r3).len);
		buffer_dump(out_dir, "dynamic_srh_p_r3.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "time v.s. srh_p_r4");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "srh_p_r4");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "m^{-3}s^{-1}");
		buf.logscale_x = 1;
		buf.logscale_y = 1;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->srh_p_r4).x,
				   (store->srh_p_r4).data,
				   (store->srh_p_r4).len);
		buffer_dump(out_dir, "dynamic_srh_p_r4.dat", &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1.0;
		strcpy(buf.title, "time v.s. band bend (percent)");
		strcpy(buf.type, "xy");
		strcpy(buf.x_label, "Time");
		strcpy(buf.y_label, "band bend");
		strcpy(buf.x_units, "s");
		strcpy(buf.y_units, "percent");
		buf.logscale_x = 1;
		buf.logscale_y = 1;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, (store->band_bend).x,
				   (store->band_bend).data,
				   (store->band_bend).len);
		buffer_dump(out_dir, "dynamic_band_bend.dat", &buf);
		buffer_free(&buf);
	}

}

void dump_dynamic_add_data(struct dynamic_store *store, struct device *in,
			   double x_value)
{
}

void dump_dynamic_free(struct dynamic_store *store)
{
	if (get_dump_status(dump_dynamic) == TRUE) {
		inter_free(&(store->charge_change));
		inter_free(&(store->jout));
		inter_free(&(store->jn_avg));
		inter_free(&(store->jp_avg));
		inter_free(&(store->dynamic_jn));
		inter_free(&(store->dynamic_jp));
		inter_free(&(store->jnout_mid));
		inter_free(&(store->jpout_mid));
		inter_free(&(store->iout));
		inter_free(&(store->iout_left));
		inter_free(&(store->iout_right));
		inter_free(&(store->gexout));
		inter_free(&(store->nfree_to_ptrap));
		inter_free(&(store->pfree_to_ntrap));
		inter_free(&(store->Rnout));
		inter_free(&(store->Rpout));
		inter_free(&(store->nrelax_out));
		inter_free(&(store->prelax_out));
		inter_free(&(store->ntrap));
		inter_free(&(store->ptrap));
		inter_free(&(store->ntrap_delta_out));
		inter_free(&(store->ptrap_delta_out));
		inter_free(&(store->nfree));
		inter_free(&(store->pfree));
		inter_free(&(store->nfree_delta_out));
		inter_free(&(store->pfree_delta_out));
		inter_free(&(store->tpc_filledn));
		inter_free(&(store->Rnpout));
		inter_free(&(store->tpc_filledp));
		inter_free(&(store->tpc_mue));
		inter_free(&(store->tpc_muh));
		inter_free(&(store->tpc_mu_avg));
		inter_free(&(store->only_n));
		inter_free(&(store->only_p));
		inter_free(&(store->dynamic_np));
		inter_free(&(store->E_field));
		inter_free(&(store->dynamic_Vapplied));
		inter_free(&(store->dynamic_charge_tot));
		inter_free(&(store->dynamic_pl));
		inter_free(&(store->dynamic_jn_drift));
		inter_free(&(store->dynamic_jn_diffusion));
		inter_free(&(store->dynamic_jp_drift));
		inter_free(&(store->dynamic_jp_diffusion));
		inter_free(&(store->dynamic_qe));

		inter_free(&(store->srh_n_r1));
		inter_free(&(store->srh_n_r2));
		inter_free(&(store->srh_n_r3));
		inter_free(&(store->srh_n_r4));

		inter_free(&(store->srh_p_r1));
		inter_free(&(store->srh_p_r2));
		inter_free(&(store->srh_p_r3));
		inter_free(&(store->srh_p_r4));

		inter_free(&(store->band_bend));

		free(store->band_snapshot);
	}
}
