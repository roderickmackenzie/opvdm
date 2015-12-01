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

	dump = inp_search_english(&inp, "#plot");
	set_dump_status(dump_plot, dump);

	dump = inp_search_english(&inp, "#newton_dump");
	set_dump_status(dump_newton, dump);

	dump = inp_search_english(&inp, "#dump_dynamic");
	set_dump_status(dump_dynamic, dump);

	in->stop_start = inp_search_english(&inp, "#startstop");

	in->dumpitdos = inp_search_english(&inp, "#dumpitdos");

	inp_search_string(&inp, in->plot_file, "#plotfile");

	inp_search_double(&inp, &(in->start_stop_time), "#start_stop_time");

	inp_search_int(&inp, &(dump), "#dump_iodump");
	set_dump_status(dump_iodump, dump);

	in->dump_movie = inp_search_english(&inp, "#dump_movie");

	dump = inp_search_english(&inp, "#dump_optics");
	set_dump_status(dump_optics, dump);

	dump = inp_search_english(&inp, "#dump_optics_verbose");
	set_dump_status(dump_optics_verbose, dump);

	dump = inp_search_english(&inp, "#dump_norm_time_to_one");
	set_dump_status(dump_norm_time_to_one, dump);

	dump = inp_search_english(&inp, "#dump_norm_y_axis");
	set_dump_status(dump_norm_y_axis, dump);

	dump = inp_search_english(&inp, "#dump_1d_slices");
	set_dump_status(dump_1d_slices, dump);

	dump = inp_search_english(&inp, "#dump_energy_slice_switch");
	set_dump_status(dump_energy_slice_switch, dump);

	inp_search_int(&inp, &(in->dump_slicepos), "#dump_energy_slice_pos");
	if (in->dump_slicepos >= in->ymeshpoints)
		in->dump_slicepos = 0;

	dump = inp_search_english(&inp, "#dump_print_newtonerror");
	set_dump_status(dump_print_newtonerror, dump);

	dump = inp_search_english(&inp, "#dump_write_converge");
	set_dump_status(dump_write_converge, dump);

	dump = inp_search_english(&inp, "#dump_print_converge");
	set_dump_status(dump_print_converge, dump);

	dump = inp_search_english(&inp, "#dump_print_pos_error");
	set_dump_status(dump_print_pos_error, dump);

	dump = inp_search_english(&inp, "#dump_pl");
	set_dump_status(dump_pl, dump);

	dump = inp_search_english(&inp, "#dump_zip_files");
	set_dump_status(dump_zip_files, dump);

	dump = inp_search_english(&inp, "#dump_print_hardware_info");
	set_dump_status(dump_print_hardware_info, dump);

	dump = inp_search_english(&inp, "#dump_write_out_band_structure");
	set_dump_status(dump_write_out_band_structure, dump);

	inp_free(&inp);

	if (get_dump_status(dump_iodump) == FALSE) {
		in->dumpitdos = FALSE;
		set_dump_status(dump_optics, FALSE);
	}

}
