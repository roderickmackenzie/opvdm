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
#ifndef epitaxy_h
#define epitaxy_h

struct epitaxy {
	int layers;
	int electrical_layers;
	double width[20];
	char mat_file[20][100];
	char dos_file[20][100];
	char pl_file[20][100];
	char electrical_layer[20];
};

void epitaxy_load(struct epitaxy *in, char *file);
double epitaxy_get_electrical_length(struct epitaxy *in);
double epitaxy_get_optical_length(struct epitaxy *in);
int epitaxy_get_optical_material_layer(struct epitaxy *in, double pos);
int epitaxy_get_electrical_material_layer(struct epitaxy *in, double pos);
double epitaxy_get_device_start(struct epitaxy *in);
double epitaxy_get_device_start_i(struct epitaxy *in);
#endif
