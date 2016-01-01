//    Organic Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for organic solar cells. 
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; version 2 of the License
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
#include "../../sim.h"
#include "../../dump.h"
#include "../../ntricks.h"
#include "../../dynamic_store.h"
#include "pulse.h"
#include "../../inp.h"
#include "../../buffer.h"
#include "../../gui_hooks.h"
#include "../../lang.h"

static int unused __attribute__ ((unused));

struct pulse pulse_config;

int pulse_find_config(char *ret, char *dir_name, char *search_name)
{
	int i = 0;
	int found = FALSE;
	char filepath[256];
	char sim_name[256];
	struct inp_file inp;
	struct inp_list a;
	inp_listdir(&a);

	for (i = 0; i < a.len; i++) {
		if ((strcmp(a.names[i], ".") != 0)
		    && (strcmp(a.names[i], "..") != 0)) {
			if ((cmpstr_min(a.names[i], "pulse") == 0)
			    && (strcmp_end(a.names[i], ".inp") == 0)) {
				inp_init(&inp);
				inp_load_from_path(&inp, dir_name, a.names[i]);
				inp_search_string(&inp, sim_name,
						  "#sim_menu_name");
				inp_free(&inp);

				if (strcmp(sim_name, search_name) == 0) {
					strcpy(ret, a.names[i]);
					found = TRUE;
					break;
				}

			}
		}
	}

	inp_list_free(&a);

	if (found == TRUE) {
		return 0;
	} else {
		strcpy(ret, "");
		return -1;
	}

	return -1;
}

void sim_pulse(struct device *in)
{
	struct buffer buf;
	buffer_init(&buf);

	struct dynamic_store store;
	dump_dynamic_init(&store, in);

	struct istruct out_i;
	inter_init(&out_i);

	struct istruct out_v;
	inter_init(&out_v);

	struct istruct out_G;
	inter_init(&out_G);

	struct istruct lost_charge;
	inter_init(&lost_charge);

	char config_file_name[200];
	char search_name[200];

	if (pulse_find_config(config_file_name, in->inputpath, in->simmode) !=
	    0) {
		ewe("%s %s %s\n", _("no pulse config file found"),
		    in->inputpath, in->simmode);
	}

	pulse_load_config(&pulse_config, in, config_file_name);
	int number = strextract_int(config_file_name);
	in->go_time = FALSE;

	in->time = 0.0;

	time_init(in);
//time_load_mesh(in,number);
	time_load_mesh_build(in, number);
//time_mesh_save();
//getchar();
//struct istruct pulseout;
//inter_init(&pulseout);

	int ittr = 0;

	int step = 0;
	in->Psun = time_get_sun();
	light_solve_and_update(in, &(in->mylight), time_get_sun(),
			       time_get_laser());

	double V = 0.0;
	double V0 = 0.0;

	if (pulse_config.pulse_sim_mode == pulse_load) {
		sim_externalv(in, time_get_voltage());
		newton_pulse(in, time_get_voltage(), FALSE);
	} else if (pulse_config.pulse_sim_mode == pulse_open_circuit) {
		in->Vapplied = in->Vbi;
		pulse_newton_sim_voc(in);
		pulse_newton_sim_voc_fast(in, FALSE);
	} else {
		ewe(_("pulse mode not known\n"));
	}

	device_timestep(in);

	in->go_time = TRUE;

	double extracted_through_contacts = 0.0;
	double laser_width = 0.0;
	double laser_power = 0.0;
	double i0 = 0;
	carrier_count_reset(in);
	reset_np_save(in);
	do {
		in->Psun = time_get_sun();
		light_solve_and_update(in, &(in->mylight), time_get_sun(),
				       time_get_laser() + time_get_fs_laser());
		dump_dynamic_add_data(&store, in, in->time);

		if (pulse_config.pulse_sim_mode == pulse_load) {
			i0 = newton_pulse(in, time_get_voltage(), TRUE);
		} else if (pulse_config.pulse_sim_mode == pulse_open_circuit) {
			V = in->Vapplied;
			pulse_newton_sim_voc_fast(in, TRUE);
		} else {
			ewe(_("pulse mode not known\n"));
		}

		if (get_dump_status(dump_print_text) == TRUE) {
			printf("%s=%e %s=%d %.1e ", _("pulse time"), in->time,
			       _("step"), step, in->last_error);
			printf("Vtot=%lf %s = %e mA (%e A/m^2)\n", V,
			       _("current"), get_I(in) / 1e-3, get_J(in));
		}

		ittr++;

		gui_send_data("pulse");
		dump_write_to_disk(in);

		plot_now(in, "pulse.plot");

		inter_append(&out_i, in->time, i0);
		inter_append(&out_v, in->time, V);
		inter_append(&out_G, in->time, in->Gn[0]);
		inter_append(&lost_charge, in->time,
			     extracted_through_contacts -
			     fabs(get_extracted_n(in) +
				  get_extracted_p(in)) / 2.0);

		device_timestep(in);
		step++;

		if (time_run() == FALSE)
			break;
		//getchar();

	} while (1);
	char outpath[1000];
	struct istruct out_flip;

	dump_dynamic_save(in->outputpath, &store);
	dump_dynamic_free(&store);

	buffer_malloc(&buf);
	buf.y_mul = 1e3;
	buf.x_mul = 1e6;
	strcpy(buf.title, _("Time - current"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Time"));
	strcpy(buf.y_label, _("Current"));
	strcpy(buf.x_units, _("us"));
	strcpy(buf.y_units, _("m"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, out_i.x, out_i.data, out_i.len);
	buffer_dump_path(in->outputpath, "pulse_i.dat", &buf);
	buffer_free(&buf);

	inter_copy(&out_flip, &out_i, TRUE);
	inter_mul(&out_flip, -1.0);

	buffer_malloc(&buf);
	buf.y_mul = 1e3;
	buf.x_mul = 1e6;
	strcpy(buf.title, _("Time - -current"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Time"));
	strcpy(buf.y_label, _("-Current"));
	strcpy(buf.x_units, _("us"));
	strcpy(buf.y_units, _("mA"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, out_flip.x, out_flip.data, out_flip.len);
	buffer_dump_path(in->outputpath, "pulse_i_pos.dat", &buf);
	buffer_free(&buf);

	inter_free(&out_flip);

	buffer_malloc(&buf);
	buf.y_mul = 1.0;
	buf.x_mul = 1e6;
	strcpy(buf.title, _("Time - Voltage"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Time"));
	strcpy(buf.y_label, _("Volts"));
	strcpy(buf.x_units, _("us"));
	strcpy(buf.y_units, _("Voltage"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, out_v.x, out_v.data, out_v.len);
	buffer_dump_path(in->outputpath, "pulse_v.dat", &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul = 1.0;
	buf.x_mul = 1e6;
	strcpy(buf.title, _("Time - Photogeneration rate"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Time"));
	strcpy(buf.y_label, _("Generation rate"));
	strcpy(buf.x_units, _("s"));
	strcpy(buf.y_units, _("m^{-3} s^{-1}"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, out_G.x, out_G.data, out_G.len);
	buffer_dump_path(in->outputpath, "pulse_G.dat", &buf);
	buffer_free(&buf);

//sprintf(outpath,"%s%s",in->outputpath,"pulse_lost_charge.dat");
//inter_save(&lost_charge,outpath);

	in->go_time = FALSE;

	inter_free(&out_G);
	inter_free(&out_i);
	inter_free(&out_v);
	time_memory_free();
}

void pulse_load_config(struct pulse *in, struct device *dev,
		       char *config_file_name)
{

	char name[200];
	struct inp_file inp;
	inp_init(&inp);
	inp_load_from_path(&inp, dev->inputpath, config_file_name);
	inp_check(&inp, 1.25);

	inp_search_double(&inp, &(in->Rshort_pulse), "#Rshort_pulse");
	inp_search_double(&inp, &(in->pulse_laser_power), "#pulse_laser_power");
	inp_search_double(&inp, &(in->pulse_shift), "#pulse_shift");
	inp_search_string(&inp, name, "#pulse_sim_mode");
	inp_search_double(&inp, &(in->pulse_L), "#pulse_L");
	inp_search_double(&inp, &(in->pulse_Rload), "#Rload");

	in->pulse_sim_mode = english_to_bin(name);

	inp_free(&inp);

}
