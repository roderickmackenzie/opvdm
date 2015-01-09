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
#ifndef dump_ctrl_h
#define dump_ctrl_h

#define dump_iodump 0
#define dump_optics 1
#define dump_newton 2
#define dump_plot 3
#define dump_stop_plot 4
#define dump_opt_for_fit 5
#define dump_print_text 7
#define dump_exit_on_dos_error 8
#define dump_energy_slice_switch 9
#define dump_lock 11
#define dump_lock_enable 12
#define dump_band_structure 14
#define dump_print_newtonerror 15
#define dump_print_converge 16
#define dump_slices_by_time 17
#define dump_all_slices 18
#define dump_print_pos_error 19
#define dump_optics_verbose 20
#define dump_pl 21
void set_io_dump(int in);
int get_io_dump();
int get_dump_status(int a);
void set_dump_status(int name, int a);

#endif
