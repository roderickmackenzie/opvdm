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
//    the Free Software Foundation; either version 2 of the License.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#ifndef h_dump
#define h_dump
#include <frame.h>
#include "sim.h"
#include "dump_ctrl.h"
#include "dynamic_store.h"

void dump_init(struct device *in);
void dump_load_config(struct device *in);
void dump_remove_snapshots(struct device *in);
void dump_dynamic_init(struct dynamic_store *store, struct device *in);
void dump_dynamic_save(char *outputpath, struct dynamic_store *store);
void dump_dynamic_add_data(struct dynamic_store *store, struct device *in,
			   double x_value);
void dump_dynamic_free(struct dynamic_store *store);
void dump_build_2d_charge_frame(struct map *mapin_e, struct map *mapin_h,
				struct device *in);
void dump_write_2d_charge_map(struct map *in_e, struct map *in_h,
			      struct device *dev);
void frame_add_data(struct map *in, double x, double y, double data);
void dump_slice(struct device *in, char *prefix);
void dump_energy_slice(char *out_dir, struct device *in);
void dump_write_2d_charge_single_map(struct map *in, struct device *dev);
void dump_build_2d_charge_single_frame(struct map *mapin, struct device *in);
void dump_device_map(char *out_dir, struct device *in);
void dump_1d_slice(struct device *in, char *out_dir);
void dump_write_to_disk(struct device *in);
#endif
