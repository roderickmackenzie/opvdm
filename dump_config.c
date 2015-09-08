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
#include "dump.h"
#include "inp.h"

void dump_load_config(struct device *in)
{
	int dump;
	struct inp_file inp;
	inp_init(&inp);
	inp_load_from_path(&inp, in->inputpath, "dump.inp");
	inp_check(&inp, 1.32);

	inp_search_int(&inp, &(dump), "#plot");
	set_dump_status(dump_plot, dump);

	inp_search_int(&inp, &(dump), "#newton_dump");
	set_dump_status(dump_newton, dump);

	inp_search_int(&inp, &(dump), "#dump_dynamic");
	set_dump_status(dump_dynamic, dump);

	inp_search_int(&inp, &(in->stop_start), "#startstop");

	inp_search_int(&inp, &(in->dumpitdos), "#dumpitdos");

	inp_search_string(&inp, in->plot_file, "#plotfile");

	inp_search_double(&inp, &(in->start_stop_time), "#start_stop_time");

	inp_search_int(&inp, &(dump), "#dump_iodump");
	set_dump_status(dump_iodump, dump);

	inp_search_int(&inp, &(in->dump_movie), "#dump_movie");

	inp_search_int(&inp, &(dump), "#dump_optics");
	set_dump_status(dump_optics, dump);

	inp_search_int(&inp, &(dump), "#dump_optics_verbose");
	set_dump_status(dump_optics_verbose, dump);

	inp_search_int(&inp, &(dump), "#dump_norm_time_to_one");
	set_dump_status(dump_norm_time_to_one, dump);

	inp_search_int(&inp, &(dump), "#dump_norm_y_axis");
	set_dump_status(dump_norm_y_axis, dump);

	inp_search_int(&inp, &(dump), "#dump_1d_slices");
	set_dump_status(dump_1d_slices, dump);

	inp_search_int(&inp, &(dump), "#dump_energy_slice_switch");
	set_dump_status(dump_energy_slice_switch, dump);

	inp_search_int(&inp, &(in->dump_slicepos), "#dump_energy_slice_pos");
	if (in->dump_slicepos >= in->ymeshpoints)
		in->dump_slicepos = 0;

	inp_search_int(&inp, &(dump), "#dump_print_newtonerror");
	set_dump_status(dump_print_newtonerror, dump);

	inp_search_int(&inp, &(dump), "#dump_write_converge");
	set_dump_status(dump_write_converge, dump);

	inp_search_int(&inp, &(dump), "#dump_print_converge");
	set_dump_status(dump_print_converge, dump);

	inp_search_int(&inp, &(dump), "#dump_print_pos_error");
	set_dump_status(dump_print_pos_error, dump);

	inp_search_int(&inp, &(dump), "#dump_pl");
	set_dump_status(dump_pl, dump);

	inp_search_int(&inp, &(dump), "#dump_zip_files");
	set_dump_status(dump_zip_files, dump);

	inp_search_int(&inp, &(dump), "#dump_print_hardware_info");
	set_dump_status(dump_print_hardware_info, dump);

	inp_search_int(&inp, &(dump), "#dump_write_out_band_structure");
	set_dump_status(dump_write_out_band_structure, dump);

	inp_free(&inp);

	if (get_dump_status(dump_iodump) == FALSE) {
		in->dumpitdos = FALSE;
		set_dump_status(dump_optics, FALSE);
	}

}
