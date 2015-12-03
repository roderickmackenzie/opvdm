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
#include "dump.h"
#include <math.h>
#include "../../elec_plugins.h"
#include "../../dynamic_store.h"
#include "ntricks.h"
#include "../../inp.h"
#include "../../buffer.h"
#include "../../gui_hooks.h"
#include "../../pl.h"

static int unused __attribute__ ((unused));

void sim_jv(struct device *in)
{
	printf("entered jv\n");
	struct buffer buf;
	buffer_init(&buf);

	struct dynamic_store store;
	dump_dynamic_init(&store, in);

	struct jv config;
	jv_load_config(&config, in);
	double V = 0.0;
	double Vstop = config.Vstop;
	double Vstep = config.Vstep;
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

	struct istruct lv;
	inter_init(&lv);

	in->Vapplied = 0.0;
	if (fabs(config.Vstart - in->Vapplied) > 0.2) {
		ramp_externalv(in, 0.0, config.Vstart);
	}

	in->Vapplied = config.Vstart;

	sim_externalv(in, in->Vapplied);

	remesh_reset(in, in->Vapplied);
	if (in->remesh == TRUE) {
		light_solve_and_update(in, &(in->mylight),
				       in->Psun * config.jv_light_efficiency,
				       0.0);
	}

	V = in->Vapplied;
	newton_set_min_ittr(30);
	in->Vapplied = config.Vstart;
	newton_sim_jv(in);
	newton_set_min_ittr(0);

	double k_voc = 0.0;
	double n_voc = 0.0;
	double r_voc = 0.0;
	double nsc = 0.0;
	double n_trap_voc = 0.0;
	double p_trap_voc = 0.0;
	double n_free_voc = 0.0;
	double p_free_voc = 0.0;
	double np_voc_tot = 0.0;
	double r_pmax = 0.0;
	double n_pmax = 0.0;
	do {

		in->Vapplied = V;

		newton_sim_jv(in);

		J = get_equiv_J(in);
		Vexternal = get_equiv_V(in);

		gui_send_data("pulse");

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

		plot_now(in, "jv.plot");
		dump_dynamic_add_data(&store, in, get_equiv_V(in));

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
				np_voc_tot = get_total_np(in);
				n_trap_voc = get_n_trapped_charge(in);
				n_free_voc = get_free_n_charge(in);
				p_trap_voc = get_p_trapped_charge(in);
				p_free_voc = get_free_p_charge(in);

			}

			if ((Pden > Pdenlast) && (Vexternal > 0.0) && (J < 0.0)) {
				in->Pmax = Pden;
				in->Pmax_voltage = Vexternal;
				r_pmax = get_avg_recom(in);
				n_pmax = get_extracted_np(in);
			}

			if (Vexternal > Vstop)
				break;

		}

		if (get_dump_status(dump_print_converge) == TRUE) {
			printf("V=%lf %lf current = %e mA (%e A/m^2) %le\n", V,
			       Vexternal, get_I(in) / 1e-3, J, in->last_error);
		}

		Jlast = J;
		Vlast = Vexternal;
		Pdenlast = Pden;
		first = FALSE;

		dump_write_to_disk(in);

		inter_append(&lv, get_equiv_V(in), pl_get_light_energy());

		V += Vstep;
		Vstep *= config.jv_step_mul;

		if ((Vstep >= 0) && (V > Vstop)) {
			in->stop = TRUE;
			printf("--------------1\n");
		}

		if ((Vstep < 0) && (V < Vstop)) {
			in->stop = TRUE;
			printf("--------------2\n");
		}

		if (get_equiv_J(in) > config.jv_max_j) {
			in->stop = TRUE;
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
		       fabs(in->Pmax / in->Psun / 1000) * 100.0);
	}

	FILE *out;
	out = fopena(in->inputpath, "sim_info.dat", "w");
	fprintf(out, "#ff\n%le\n", in->FF);
	fprintf(out, "#pce\n%le\n", fabs(in->Pmax / (in->Psun / 1000)) * 100.0);
	fprintf(out, "#voc\n%le\n", in->Voc);
	fprintf(out, "#voc_tau\n%le\n", n_voc / r_voc);
	fprintf(out, "#voc_R\n%le\n", r_voc);
	fprintf(out, "#jv_voc_k\n%le\n", r_voc / n_voc);
	fprintf(out, "#jv_pmax_n\n%le\n", n_pmax);
	fprintf(out, "#jv_pmax_tau\n%le\n", n_pmax / r_pmax);
	fprintf(out, "#voc_nt\n%le\n", n_trap_voc);
	fprintf(out, "#voc_pt\n%le\n", p_trap_voc);
	fprintf(out, "#voc_nf\n%le\n", n_free_voc);
	fprintf(out, "#voc_pf\n%le\n", p_free_voc);
	fprintf(out, "#jsc\n%le\n", in->Jsc);
	fprintf(out, "#jv_jsc_n\n%le\n", nsc);
	fprintf(out, "#jv_vbi\n%le\n", in->vbi);
	fprintf(out, "#jv_gen\n%le\n", get_avg_gen(in));
	fprintf(out, "#voc_np_tot\n%le\n", np_voc_tot);
	fprintf(out, "#end");
	fclose(out);

	if (get_dump_status(dump_iodump) == TRUE) {

		inter_save_a(&klist, in->outputpath, "k.dat");
		inter_free(&klist);
	}

	buffer_malloc(&buf);
	buf.y_mul = 1.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Charge density - Appleid voltage");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Applied Voltage");
	strcpy(buf.y_label, "Charge density");
	strcpy(buf.x_units, "Volts");
	strcpy(buf.y_units, "m^{-3}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, charge.x, charge.data, charge.len);
	buffer_dump_path(in->outputpath, "charge.dat", &buf);
	buffer_free(&buf);

	inter_save_a(&charge_tot, in->outputpath, "charge_tot.dat");
	inter_free(&charge_tot);

	buffer_malloc(&buf);
	buf.y_mul = 1.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Current density - Appleid voltage");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Applied Voltage");
	strcpy(buf.y_label, "Current density");
	strcpy(buf.x_units, "Volts");
	strcpy(buf.y_units, "A m^{-2}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, jvexternal.x, jvexternal.data, jvexternal.len);
	buffer_dump_path(in->outputpath, "jvexternal.dat", &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul = 1.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Current - Appleid voltage");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Applied Voltage");
	strcpy(buf.y_label, "Current");
	strcpy(buf.x_units, "Volts");
	strcpy(buf.y_units, "A");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, jvexternal.x, jvexternal.data, jvexternal.len);
	buffer_dump_path(in->outputpath, "ivexternal.dat", &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul = 1000.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Voltage - Light generated");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Applied Voltage");
	strcpy(buf.y_label, "Light power");
	strcpy(buf.x_units, "Volts");
	strcpy(buf.y_units, "mW");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, lv.x, lv.data, lv.len);
	buffer_dump_path(in->outputpath, "lv.dat", &buf);
	buffer_free(&buf);

	inter_free(&jvexternal);
	inter_free(&charge);
	inter_free(&ivexternal);
	inter_free(&lv);

	dump_dynamic_save(in->outputpath, &store);
	dump_dynamic_free(&store);
}

void jv_load_config(struct jv *in, struct device *dev)
{
	struct inp_file inp;
	inp_init(&inp);
	inp_load_from_path(&inp, dev->inputpath, "jv.inp");
	inp_check(&inp, 1.21);
	inp_search_double(&inp, &(in->Vstart), "#Vstart");
	inp_search_double(&inp, &(in->Vstop), "#Vstop");
	inp_search_double(&inp, &(in->Vstep), "#Vstep");
	inp_search_double(&inp, &(in->jv_step_mul), "#jv_step_mul");
	inp_search_double(&inp, &(in->jv_light_efficiency),
			  "#jv_light_efficiency");
	inp_search_double(&inp, &(in->jv_max_j), "#jv_max_j");
	in->jv_light_efficiency = fabs(in->jv_light_efficiency);
	inp_free(&inp);

}
