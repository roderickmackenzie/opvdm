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
#include <frame.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "sim.h"
#include "dump.h"

static int unused __attribute__ ((unused));

static struct istruct jout;
static struct istruct jout_avg;
static struct istruct jnout;
static struct istruct jpout;
static struct istruct iout;
static struct istruct iout_left;
static struct istruct iout_right;
static struct istruct gexout;
static struct istruct ntrapout;
static struct istruct ptrapout;
static struct istruct ntrap_delta_out;
static struct istruct ptrap_delta_out;
static struct istruct nfreeout;
static struct istruct pfreeout;
static struct istruct nfree_delta_out;
static struct istruct pfree_delta_out;
static struct istruct Rnpout;
static struct istruct Rnout;
static struct istruct Rpout;
static struct istruct nrelax_out;
static struct istruct prelax_out;
static struct istruct tpc_mue;
static struct istruct tpc_muh;
static struct istruct tpc_mu_avg;
static struct istruct tpc_filledn;
static struct istruct tpc_filledp;
static struct istruct dynamic_np;
static struct istruct only_n;
static struct istruct only_p;
static struct istruct E_field;
static struct istruct dynamic_pl;

static int frame_config_change;

void dump_build_2d_charge_frame(struct map *mapin_e, struct map *mapin_h,
				struct device *in)
{

	int i;
	int band;
	double offset = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		if (frame_config_change == TRUE) {
			frame_add_data(mapin_e, in->ymesh[i],
				       in->Ec[i] + offset,
				       in->n[i] - in->nfinit[i]);
			frame_add_data(mapin_h, in->ymesh[i], in->Ev[i],
				       in->p[i] - in->pfinit[i]);

			for (band = 0; band < in->srh_bands; band++) {
				frame_add_data(mapin_e, in->ymesh[i],
					       in->Ec[i] +
					       dos_get_band_energy_n(band,
								     in->
								     imat[i]) +
					       offset,
					       in->nt[i][band] -
					       in->ntbinit[i][band]);

				frame_add_data(mapin_h, in->ymesh[i],
					       in->Ev[i] -
					       dos_get_band_energy_p(band,
								     in->
								     imat[i]),
					       in->pt[i][band] -
					       in->ptbinit[i][band]);
			}
		} else {
			frame_add_data(mapin_e, in->ymesh[i], in->Ec[i],
				       in->n[i]);
			frame_add_data(mapin_h, in->ymesh[i], in->Ev[i],
				       in->p[i]);

			for (band = 0; band < in->srh_bands; band++) {
				frame_add_data(mapin_e, in->ymesh[i],
					       in->Ec[i] +
					       dos_get_band_energy_n(band,
								     in->
								     imat[i]) +
					       offset, in->nt[i][band]);

				frame_add_data(mapin_h, in->ymesh[i],
					       in->Ev[i] -
					       dos_get_band_energy_p(band,
								     in->
								     imat[i]),
					       in->pt[i][band]);
			}
		}
	}

}

void dump_build_2d_charge_single_frame(struct map *mapin, struct device *in)
{

	int i;
	int band;
	double offset = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		frame_add_data(mapin, in->ymesh[i], in->Ec[i], in->n[i]);
		frame_add_data(mapin, in->ymesh[i], in->Ev[i], in->p[i]);

		for (band = 0; band < in->srh_bands; band++) {
			frame_add_data(mapin, in->ymesh[i],
				       in->Ec[i] + dos_get_band_energy_n(band,
									 in->
									 imat
									 [i]) +
				       offset, in->nt[i][band]);

			frame_add_data(mapin, in->ymesh[i],
				       in->Ev[i] - dos_get_band_energy_p(band,
									 in->
									 imat
									 [i]),
				       in->pt[i][band]);
		}
	}

}

void dump_write_2d_charge_single_map(struct map *in, struct device *dev)
{
	char temp[400];
	FILE *out;
	out = fopen("./frame_single.plot", "w");
	fprintf(out, "set terminal jpeg large \n");
	fprintf(out, "set yrange \[%le:%le]\n", in->ystart, in->ystop);
	fprintf(out, "set title 'V=%.3lf (V)'\n", dev->Vapplied);
	fprintf(out, "set pm3d map\n");
	fprintf(out, "set xlabel 'Position (nm)'\n");
	fprintf(out, "set ylabel 'Energy (eV)'\n");
	fprintf(out, "set logscale cb\n");
	fprintf(out, "set cbrange[1e14:1e25]\n");
	fprintf(out, "set xrange[%lf:%lf]\n", 0.0, (1e9 * dev->ylen));
	fprintf(out, "set format cb \"10^\{%%L}\"\n");
	fprintf(out,
		"splot 'frame_single.dat' using ($1*1e9):($2):($3) title ''\n");

	fclose(out);

	frame_dump("./frame_single.dat", in);

	unused = system("gnuplot frame_single.plot >frame_single.jpg");

	sprintf(temp, "mv frame_single.jpg single_%d.jpg", in->count);
	printf("running '%s'\n", temp);
	unused = system(temp);

}

void dump_write_2d_charge_map(struct map *in_e, struct map *in_h,
			      struct device *dev)
{
	char temp[400];
	FILE *out;
	out = fopen("./frame.plot", "w");
	fprintf(out, "set terminal jpeg large \n");
	fprintf(out, "set multiplot\n");
	fprintf(out, "set size 1.0,0.5\n");
	fprintf(out, "set origin 0.0,0.0\n");
	fprintf(out, "set yrange \[%le:%le]\n", in_e->ystart, in_e->ystop);
	fprintf(out, "set title 'V=%.3lf (V)'\n", dev->Vapplied);
	fprintf(out, "set pm3d map\n");
	fprintf(out, "set xlabel 'Position (nm)'\n");
	fprintf(out, "set ylabel 'Energy (eV)'\n");
	fprintf(out, "set logscale cb\n");
	fprintf(out, "set cbrange[1e14:1e25]\n");
	fprintf(out, "set xrange[%lf:%lf]\n", 0.0, (1e9 * dev->ylen));
	fprintf(out, "set format cb \"10^\{%%L}\"\n");
	fprintf(out, "splot 'frame_h.dat' using ($1*1e9):($2):($3) title ''\n");
	fprintf(out, "set origin 0.0,0.5\n");
	fprintf(out, "splot 'frame_e.dat' using ($1*1e9):($2):($3) title ''\n");

	fclose(out);

	frame_dump("./frame_e.dat", in_e);
	frame_dump("./frame_h.dat", in_h);

	unused = system("gnuplot frame.plot >frame.jpg");

	sprintf(temp, "mv frame.jpg %d.jpg", in_e->count);
	printf("running '%s'\n", temp);
	unused = system(temp);

	in_e->count++;

}

void dump_device_map(char *out_dir, char *extra, struct device *in)
{
}

void frame_add_data(struct map *in, double x, double y, double data)
{

	int xpos = (x - in->xstart) / in->xdelta;
	int ypos = (y - in->ystart) / in->ydelta;
	if (xpos < 0)
		xpos = 0;
	if (ypos < 0)
		ypos = 0;
	if (xpos >= in->xpoints)
		xpos = in->xpoints - 1;
	if (ypos >= in->ypoints)
		ypos = in->ypoints - 1;

	if (in->data[xpos][ypos] == 0) {
		in->data[xpos][ypos] = data;
	} else {
		in->data[xpos][ypos] = 1e25;
	}

}

void dump_dynamic_init(struct device *in)
{
	if (get_dump_status(dump_iodump) == TRUE) {
		inter_init(&jout);
		inter_init(&jout_avg);
		inter_init(&jnout);
		inter_init(&jpout);
		inter_init(&iout);
		inter_init(&iout_left);
		inter_init(&iout_right);
		inter_init(&gexout);
		inter_init(&ntrapout);
		inter_init(&ptrapout);
		inter_init(&ntrap_delta_out);
		inter_init(&ptrap_delta_out);
		inter_init(&nfreeout);
		inter_init(&pfreeout);
		inter_init(&nfree_delta_out);
		inter_init(&pfree_delta_out);
		inter_init(&Rnpout);
		inter_init(&Rnout);
		inter_init(&Rpout);
		inter_init(&nrelax_out);
		inter_init(&prelax_out);
		inter_init(&tpc_mue);
		inter_init(&tpc_muh);
		inter_init(&tpc_mu_avg);
		inter_init(&tpc_filledn);
		inter_init(&tpc_filledp);
		inter_init(&dynamic_np);
		inter_init(&only_n);
		inter_init(&only_p);
		inter_init(&E_field);
		inter_init(&dynamic_pl);
	}
}

void dump_dynamic_save(struct device *in)
{

	if (get_dump_status(dump_iodump) == TRUE) {
		char out_dir[1000];
		sprintf(out_dir, "%s/dynamic/", in->outputpath);
		struct stat st = { 0 };

		if (stat(out_dir, &st) == -1) {
			mkdir(out_dir, 0700);
		}

		char outpath[200];

		sprintf(outpath, "%s%s", out_dir, "dynamic_jn.dat");
		inter_save(&jnout, outpath);

		struct istruct one;
		inter_copy(&one, &jnout, TRUE);
		inter_deriv(&one, &jnout);
		sprintf(outpath, "%s%s", out_dir, "dynamic_djn.dat");
		inter_save(&one, outpath);
		inter_free(&one);

		sprintf(outpath, "%s%s", out_dir, "dynamic_jp.dat");
		inter_save(&jpout, outpath);

		inter_copy(&one, &jpout, TRUE);
		inter_deriv(&one, &jpout);
		sprintf(outpath, "%s%s", out_dir, "dynamic_djp.dat");
		inter_save(&one, outpath);
		inter_free(&one);

		sprintf(outpath, "%s%s", out_dir, "dynamic_j.dat");
		inter_save(&jout, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_j_avg.dat");
		inter_save(&jout_avg, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_i.dat");
		inter_save(&iout, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_i_left.dat");
		inter_save(&iout_left, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_i_right.dat");
		inter_save(&iout_right, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_gex.dat");
		inter_save(&gexout, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_Rn.dat");
		inter_save(&Rnout, outpath);

		double sum = inter_intergrate(&Rnout);
		FILE *out = fopen("dynamic_Rn_int.dat", "w");
		fprintf(out, "%le", sum);
		fclose(out);

		sprintf(outpath, "%s%s", out_dir, "dynamic_Rp.dat");
		inter_save(&Rpout, outpath);

		sum = inter_intergrate(&Rpout);
		out = fopen("dynamic_Rp_int.dat", "w");
		fprintf(out, "%le", sum);
		fclose(out);

		inter_make_cumulative(&Rnout);

		sprintf(outpath, "%s%s", out_dir, "dynamic_Rn_cumulative.dat");
		inter_save(&Rnout, outpath);

		inter_make_cumulative(&Rpout);

		sprintf(outpath, "%s%s", out_dir, "dynamic_Rp_cumulative.dat");
		inter_save(&Rpout, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_prelax.dat");
		inter_save(&nrelax_out, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_nrelax.dat");
		inter_save(&prelax_out, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_ntrap.dat");
		inter_save(&ntrapout, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_ptrap.dat");
		inter_save(&ptrapout, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_nfree.dat");
		inter_save(&nfreeout, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_pfree.dat");
		inter_save(&pfreeout, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_nfree_delta.dat");
		inter_save(&nfree_delta_out, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_pfree_delta.dat");
		inter_save(&pfree_delta_out, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_ntrap_delta.dat");
		inter_save(&ntrap_delta_out, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_ptrap_delta.dat");
		inter_save(&ptrap_delta_out, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_filledn.dat");
		inter_save(&tpc_filledn, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_Rn-p.dat");
		inter_save(&Rnpout, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_filledp.dat");
		inter_save(&tpc_filledp, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_mue.dat");
		inter_save(&tpc_mue, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_muh.dat");
		inter_save(&tpc_muh, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_mu_avg.dat");
		inter_save(&tpc_mu_avg, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_n.dat");
		inter_save(&only_n, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_p.dat");
		inter_save(&only_p, outpath);

		inter_sub_double(&dynamic_np, dynamic_np.data[0]);
		sprintf(outpath, "%s%s", out_dir, "dynamic_np.dat");
		inter_save(&dynamic_np, outpath);

		inter_norm_to_one(&dynamic_np);
		sprintf(outpath, "%s%s", out_dir, "dynamic_np_norm.dat");
		inter_save(&dynamic_np, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_E_field.dat");
		inter_div_double(&E_field, E_field.data[0]);
		inter_save(&E_field, outpath);

		sprintf(outpath, "%s%s", out_dir, "dynamic_pl.dat");
		inter_save(&dynamic_pl, outpath);

		double max =
		    inter_array_get_max(dynamic_pl.data, dynamic_pl.len / 4);
		inter_div_double(&dynamic_pl, max);
		sprintf(outpath, "%s%s", out_dir, "dynamic_pl_norm.dat");
		inter_save(&dynamic_pl, outpath);
	}

}

void dump_dynamic_add_data(struct device *in)
{
}

void dump_dynamic_free(struct device *in)
{
	if (get_dump_status(dump_iodump) == TRUE) {
		inter_free(&jout);
		inter_free(&jout_avg);
		inter_free(&jnout);
		inter_free(&jpout);
		inter_free(&iout);
		inter_free(&iout_left);
		inter_free(&iout_right);
		inter_free(&gexout);
		inter_free(&Rnout);
		inter_free(&Rpout);
		inter_free(&nrelax_out);
		inter_free(&prelax_out);
		inter_free(&ntrapout);
		inter_free(&ptrapout);
		inter_free(&ntrap_delta_out);
		inter_free(&ptrap_delta_out);
		inter_free(&nfreeout);
		inter_free(&pfreeout);
		inter_free(&nfree_delta_out);
		inter_free(&pfree_delta_out);
		inter_free(&tpc_filledn);
		inter_free(&Rnpout);
		inter_free(&tpc_filledp);
		inter_free(&tpc_mue);
		inter_free(&tpc_muh);
		inter_free(&tpc_mu_avg);
		inter_free(&only_n);
		inter_free(&only_p);
		inter_free(&dynamic_np);
		inter_free(&E_field);
		inter_free(&dynamic_pl);
	}
}

void dump_for_plot(struct device *in, char *extra)
{
	FILE *out;
	char outpath[100];
	strcpy(outpath, "");
	char out_dir[200];
	if (strcmp(extra, "") != 0) {
		sprintf(out_dir, "%s/snapshots/", in->outputpath);
		struct stat st = { 0 };

		if (stat(out_dir, &st) == -1) {
			mkdir(out_dir, 0700);
		}
	} else {
		strcpy(out_dir, in->outputpath);
	}

	if (get_dump_status(dump_pl) == TRUE) {
		exp_cal_emission(extra, in);
	}

	if (get_dump_status(dump_newton) == TRUE) {
		int i;
		int band;

		sprintf(outpath, "%s%s%s%s", out_dir, "Ec", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Ec[i]);

		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "Ev", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Ev[i]);
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "Nad", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Nad[i]);

		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "Eg", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Eg[i]);
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "Fn", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Fn[i]);
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "Fp", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Fp[i]);
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "phi", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->phi[i]);
		}

		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "Jn", extra, ".dat");
		out = fopen(outpath, "w");

		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Jn[i]);
		}

		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "Jp", extra, ".dat");
		out = fopen(outpath, "w");

		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Jp[i]);
		}

		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "Jnp", extra, ".dat");
		out = fopen(outpath, "w");

		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i],
				(in->Jp[i] + in->Jn[i]));
		}

		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "Fi", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e ", in->ymesh[i]);
			fprintf(out, "%e ", in->Fi[i]);

			fprintf(out, "\n");

		}

		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "epsilon_r", extra,
			".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->epsilonr[i]);
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "mu_n", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->mun[i]);
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "mu_p", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->mup[i]);
		}

		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "mu_n_ft", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i],
				in->mun[i] * in->n[i] / in->nt_all[i]);
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "mu_p_ft", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i],
				in->mup[i] * in->p[i] / in->pt_all[i]);
		}

		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "n", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le", in->n[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "p", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le ", in->p[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "nt", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le ", in->nt_all[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "pt", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le ", in->pt_all[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "dn", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le", in->n[i] - in->nfinit[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "charge", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le",
				in->p[i] - in->n[i] + in->pt_all[i] -
				in->nt_all[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "dcharge", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le",
				in->p[i] - in->n[i] + in->pt_all[i] -
				in->nt_all[i]);
			fprintf(out, "\n");
		}
		fclose(out);
		sprintf(outpath, "%s%s%s%s", out_dir, "dp", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le ", in->p[i] - in->pfinit[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "dnt", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le ", in->nt_all[i] - in->ntinit[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "dpt", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le ", in->pt_all[i] - in->ptinit[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "ntot", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le ", in->n[i] + in->nt_all[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "ptot", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%le ", in->p[i] + in->pt_all[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		double Gn_max = inter_array_get_max(in->Gn, in->ymeshpoints);
		if (Gn_max > 0.0) {
			out = fopena(out_dir, "./Gn.dat", "w");
			FILE *out_norm = fopena(out_dir, "./Gn_norm.dat", "w");
			for (i = 0; i < in->ymeshpoints; i++) {
				fprintf(out, "%e %le\n", in->ymesh[i],
					in->Gn[i]);
				fprintf(out_norm, "%e %le\n", in->ymesh[i],
					in->Gn[i] / Gn_max);
			}
			fclose(out);
			fclose(out_norm);
		}

		if (Gn_max > 0.0) {
			out = fopena(out_dir, "./Gp.dat", "w");
			for (i = 0; i < in->ymeshpoints; i++) {
				fprintf(out, "%e %le\n", in->ymesh[i],
					in->Gp[i]);
			}
			fclose(out);
		}

		sprintf(outpath, "%s%s%s%s", out_dir, "Rn", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Rn[i]);
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "Rp", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e\n", in->ymesh[i], in->Rp[i]);
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "relax", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e %e %e\n", in->ymesh[i], in->nrelax[i],
				in->prelax[i]);
		}
		fclose(out);

		out = fopena(out_dir, "./fsrhn.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e ", in->ymesh[i]);
			for (band = 0; band < in->srh_bands; band++) {
				fprintf(out, "%e %e ", in->Fnt[i][band],
					-in->phi[i] - in->Xi[i] +
					dos_srh_get_fermi_n(in->n[i], in->p[i],
							    band, in->imat[i]));
			}
			fprintf(out, "\n");
		}
		fclose(out);

		out = fopena(out_dir, "./fsrhh.dat", "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%e ", in->ymesh[i]);
			for (band = 0; band < in->srh_bands; band++) {
				fprintf(out, "%e %e ", in->Fpt[i][band],
					-in->phi[i] - in->Xi[i] - in->Eg[i] -
					dos_srh_get_fermi_p(in->n[i], in->p[i],
							    band, in->imat[i]));
			}
			fprintf(out, "\n");
		}
		fclose(out);

		sprintf(outpath, "%s%s%s%s", out_dir, "imat", extra, ".dat");
		out = fopen(outpath, "w");
		for (i = 0; i < in->ymeshpoints; i++) {
			fprintf(out, "%le ", in->ymesh[i]);
			fprintf(out, "%d ", in->imat[i]);
			fprintf(out, "\n");
		}
		fclose(out);

		dump_device_map(out_dir, extra, in);

	}

}

void dump_energy_slice(struct device *in, int num, int i)
{
	int band = 0;
	char outpath[200];
	struct istruct dump1;
	inter_init(&dump1);

	struct istruct dump2;
	inter_init(&dump2);

	struct istruct dump3;
	inter_init(&dump3);

	struct istruct dump4;
	inter_init(&dump4);

	struct istruct dump5;
	inter_init(&dump5);

	struct istruct dump6;
	inter_init(&dump6);

	struct istruct dump7;
	inter_init(&dump7);

	struct istruct dump8;
	inter_init(&dump8);

	struct istruct dump9;
	inter_init(&dump9);

	struct istruct dump10;
	inter_init(&dump10);

	int mat = in->imat[in->ymeshpoints / 2];
	for (band = 0; band < in->srh_bands; band++) {
		inter_append(&dump1, get_dos_E_n(band, mat),
			     in->n[i] * in->srh_n_r1[i][band] -
			     in->srh_n_r2[i][band]);
		inter_append(&dump2, get_dos_E_p(band, mat),
			     in->p[i] * in->srh_p_r1[i][band] -
			     in->srh_p_r2[i][band]);
		inter_append(&dump3, get_dos_E_n(band, mat),
			     in->nt[i][band] - in->ntbinit[i][band]);
		inter_append(&dump4, get_dos_E_p(band, mat),
			     in->pt[i][band] - in->ptbinit[i][band]);
		inter_append(&dump5, get_dos_E_n(band, mat),
			     in->p[i] * in->srh_n_r3[i][band] -
			     in->srh_n_r4[i][band]);
		inter_append(&dump6, get_dos_E_p(band, mat),
			     in->n[i] * in->srh_p_r3[i][band] -
			     in->srh_p_r4[i][band]);
		inter_append(&dump7, get_dos_E_n(band, mat), in->nt[i][band]);
		inter_append(&dump8, get_dos_E_p(band, mat), in->pt[i][band]);
		inter_append(&dump9, get_dos_E_n(band, mat), in->Fnt[i][band]);
		inter_append(&dump10, get_dos_E_p(band, mat), in->Fpt[i][band]);

	}

	sprintf(outpath, "%senergy_slice_%d_%s", in->outputpath, num,
		"nt_cap.dat");
	inter_save(&dump1, outpath);
	inter_free(&dump1);

	sprintf(outpath, "%senergy_slice_%d_%s", in->outputpath, num,
		"pt_cap.dat");
	inter_save(&dump2, outpath);
	inter_free(&dump2);

	sprintf(outpath, "%senergy_slice_%d_%s", in->outputpath, num, "nt.dat");
	inter_save(&dump3, outpath);
	inter_free(&dump3);

	sprintf(outpath, "%senergy_slice_%d_%s", in->outputpath, num, "pt.dat");
	inter_save(&dump4, outpath);
	inter_free(&dump4);

	sprintf(outpath, "%senergy_slice_%d_%s", in->outputpath, num,
		"nt_recom.dat");
	inter_save(&dump5, outpath);
	inter_free(&dump5);

	sprintf(outpath, "%senergy_slice_%d_%s", in->outputpath, num,
		"pt_recom.dat");
	inter_save(&dump6, outpath);
	inter_free(&dump6);

	sprintf(outpath, "%senergy_slice_%d_%s", in->outputpath, num,
		"nt_abs.dat");
	inter_save(&dump7, outpath);
	inter_free(&dump7);

	sprintf(outpath, "%senergy_slice_%d_%s", in->outputpath, num,
		"pt_abs.dat");
	inter_save(&dump8, outpath);
	inter_free(&dump8);

	sprintf(outpath, "%senergy_slice_%d_%s", in->outputpath, num, "fn.dat");
	inter_save(&dump9, outpath);
	inter_free(&dump9);

	sprintf(outpath, "%senergy_slice_%d_%s", in->outputpath, num, "fp.dat");
	inter_save(&dump10, outpath);
	inter_free(&dump10);
}
