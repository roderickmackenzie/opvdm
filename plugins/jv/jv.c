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
#include "../../sim.h"
#include "jv.h"
#include "frame.h"
#include "dump.h"
#include <math.h>
#include "../../elec_plugins.h"
#include "ntricks.h"

static int unused __attribute__ ((unused));

void sim_jv(struct device *in)
{
	struct jv config;
	jv_load_config(&config, in);
	double V = 0.0;
	double Vstop = config.Vstop;
	double Vstep = config.Vstep;
	FILE *iv;
	int ittr = 0;
	double J;
	double Pden;
	int first = TRUE;
	double Vlast;
	double Jlast;
	double Pdenlast;
	double Vexternal;

	struct istruct ivexternal;
	inter_init(&ivexternal);

	struct istruct jvexternal;
	inter_init(&jvexternal);

	struct istruct charge;
	inter_init(&charge);

	struct istruct charge_tot;
	inter_init(&charge_tot);

	struct istruct klist;
	inter_init(&klist);

	in->Vapplied = 0.0;
	if (fabs(config.Vstart - in->Vapplied) > 0.2) {
		ramp_externalv(in, 0.0, config.Vstart);
	}
	in->Vapplied = config.Vstart;
	sim_externalv(in, in->Vapplied);

	remesh_reset(in, in->Vapplied);

	V = in->Vapplied;
	newton_set_min_ittr(30);
	in->Vapplied = config.Vstart;
	newton_sim_jv(in);
	newton_set_min_ittr(0);
	struct map framen;
	struct map framep;
	struct map frame;
	if (in->dump_movie == TRUE) {
		frame_init(&framen, in->ymeshpoints, 1000, in->ymesh[0],
			   in->ymesh[in->ymeshpoints - 1], -7.5, -3.5);
		frame_init(&framep, in->ymeshpoints, 1000, in->ymesh[0],
			   in->ymesh[in->ymeshpoints - 1], -7.5, -3.5);
		frame_init(&frame, in->ymeshpoints, 1000, in->ymesh[0],
			   in->ymesh[in->ymeshpoints - 1], -7.5, -3.5);
	}

	double k_voc = 0.0;
	double n_voc = 0.0;
	double r_voc = 0.0;
	double nsc = 0.0;
	do {

		if (in->fgen != 0)
			light_solve_and_update(in, &(in->mylight), in->Psun,
					       0.0);
		in->Vapplied = V;

		newton_sim_jv(in);

		J = get_equiv_J(in);
		Vexternal = get_equiv_V(in);

		if (in->dump_movie == TRUE) {
			frame_reset(&framen);
			frame_reset(&framep);
			frame_reset(&frame);
			dump_build_2d_charge_frame(&framen, &framep, in);
			dump_write_2d_charge_map(&framen, &framep, in);

			dump_build_2d_charge_single_frame(&frame, in);
			dump_write_2d_charge_single_map(&frame, in);
		}

		if (ittr > 0) {

			inter_append(&jvexternal, get_equiv_V(in),
				     get_equiv_J(in));
			inter_append(&ivexternal, get_equiv_V(in),
				     get_equiv_I(in));

		}

		ittr++;

		inter_append(&charge, get_equiv_V(in), get_extracted_np(in));
		inter_append(&charge_tot, get_equiv_V(in), get_np_tot(in));

		Pden = fabs(J * Vexternal);

		plot_now(in, "jv_vars.plot");

		if (first == FALSE) {

			if ((Vlast <= 0) && (Vexternal >= 0.0)) {
				in->Jsc =
				    Jlast + (J - Jlast) * (0 - Vlast) / (V -
									 Vlast);
				nsc = get_extracted_np(in);
				printf("nsc=%le\n", nsc);
				printf("Jsc = %e\n", in->Jsc);
			}

			if ((Jlast <= 0) && (J >= 0.0)) {
				in->Voc =
				    Vlast + (Vexternal - Vlast) * (0 -
								   Jlast) / (J -
									     Jlast);
				printf("Voc = %e\n", in->Voc);
				k_voc =
				    get_avg_recom(in) /
				    pow(get_extracted_np(in), 2.0);
				r_voc = get_avg_recom(in);
				n_voc = get_extracted_np(in);

			}

			if ((Pden > Pdenlast) && (Vexternal > 0.0) && (J < 0.0)) {
				in->Pmax = Pden;
				in->Pmax_voltage = Vexternal;
			}

			if (Vexternal > Vstop)
				break;

		}

		if (get_dump_status(dump_iodump) == TRUE) {
			printf("V=%lf %lf current = %e mA (%e A/m^2) %le\n", V,
			       Vexternal, get_I(in) / 1e-3, J, get_J(in));
		}

		Jlast = J;
		Vlast = Vexternal;
		Pdenlast = Pden;
		first = FALSE;

		if (get_dump_status(dump_energy_slice_switch) == TRUE) {
			dump_energy_slice(in, ittr, in->dump_slicepos);
		}

		V += Vstep;

		if ((Vstep >= 0) && (V > Vstop)) {
			in->stop = TRUE;
			printf("--------------1\n");
		}

		if ((Vstep < 0) && (V < Vstop)) {
			in->stop = TRUE;
			printf("--------------2\n");
		}

		if (in->stop == TRUE) {
			break;
		}
		inter_append(&klist, get_extracted_np(in),
			     get_avg_recom(in) /
			     (pow(get_extracted_np(in), 2.0)));

		stop_start(in);

	} while (1);

	in->FF = fabs(in->Pmax / (in->Jsc * in->Voc));

	if (get_dump_status(dump_print_text) == TRUE) {
		printf("Voc= %lf (V)\n", in->Voc);
		printf("Jsc= %lf (A/m^2)\n", in->Jsc);
		printf("Pmax= %lf (W/m^2)\n", in->Pmax);
		printf("Voltage to get Pmax= %lf (V)\n", in->Pmax_voltage);
		printf("FF= %lf\n", in->FF * 100.0);
		printf("Efficiency= %lf percent\n",
		       mod(in->Pmax / in->Psun) * 100.0);
	}

	if (get_dump_status(dump_iodump) == TRUE) {
		iv = fopena(in->outputpath, "./ff.dat", "w");
		fprintf(iv, "%le\n", in->FF);
		fclose(iv);

		FILE *outfile = fopena(in->outputpath, "./eff.dat", "w");
		fprintf(outfile, "%le", mod(in->Pmax / (in->Psun)) * 100.0);
		fclose(outfile);

		outfile = fopena(in->outputpath, "./voc.dat", "w");
		fprintf(outfile, "%le", in->Voc);
		fclose(outfile);

		outfile = fopena(in->outputpath, "./jsc.dat", "w");
		fprintf(outfile, "%le", in->Jsc);
		fclose(outfile);

		outfile = fopena(in->outputpath, "./nsc.dat", "w");
		fprintf(outfile, "%le", nsc);
		fclose(outfile);

		outfile = fopena(in->outputpath, "./vbi.dat", "w");
		fprintf(outfile, "%le", in->vbi);
		fclose(outfile);

		outfile = fopena(in->outputpath, "./jv_gen.dat", "w");
		fprintf(outfile, "%le", get_avg_gen(in));
		fclose(outfile);

		inter_save_a(&klist, in->outputpath, "k.dat");
		inter_free(&klist);
	}

	FILE *kvocfile = fopena(in->outputpath, "./kvoc.dat", "w");
	fprintf(kvocfile, "%le %le\n", n_voc, k_voc);
	fclose(kvocfile);

	kvocfile = fopena(in->outputpath, "./voc_tau.dat", "w");
	fprintf(kvocfile, "%le %le\n", in->Voc, n_voc / r_voc);
	fclose(kvocfile);

	kvocfile = fopena(in->outputpath, "./n_tau.dat", "w");
	fprintf(kvocfile, "%le %le\n", n_voc, 1.0 / (k_voc * n_voc));
	fclose(kvocfile);

	inter_save_a(&charge, in->outputpath, "charge.dat");
	inter_free(&charge);

	inter_save_a(&charge_tot, in->outputpath, "charge_tot.dat");
	inter_free(&charge_tot);

	inter_save_a(&ivexternal, in->outputpath, "ivexternal.dat");
	inter_free(&ivexternal);

	inter_save_a(&jvexternal, in->outputpath, "jvexternal.dat");
	inter_free(&jvexternal);

	if (in->dump_movie == TRUE) {
		frame_free(&framen);
		frame_free(&framep);
		frame_free(&frame);
	}

}

void jv_load_config(struct jv *in, struct device *dev)
{
	double ver = 0.0;
	char name[200];
	FILE *config = fopena(dev->outputpath, "jv.inp", "r");

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%le", &(in->Vstart));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%le", &(in->Vstop));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%le", &(in->Vstep));

	unused = fscanf(config, "%s", name);
	unused = fscanf(config, "%lf", &(ver));
	if (ver != 1.0) {
		printf("File compatability problem %s\n", "jv.inp");
		exit(0);
	}

	unused = fscanf(config, "%s", name);

	if (strcmp("#end", name) != 0) {
		printf("Problem with jv.inp file last item read %s!\n", name);
		exit(0);
	}
	fclose(config);
}
