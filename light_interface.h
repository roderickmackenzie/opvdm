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
#ifndef h_light_interface
#define h_light_interface
#include "device.h"

#ifdef windows
#ifdef BUILDING_EXAMPLE_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
#else
#define EXPORT
#endif

EXPORT void light_dll_init();
EXPORT void light_dll_free(struct light *in);
EXPORT void light_dll_solve_and_update(struct device *cell, struct light *in,
				       double Psun_in, double laser_eff_in,
				       double pulse_width);
EXPORT int light_dll_solve_lam_slice(struct light *in, int lam);
double light_convert_density(struct device *in, double start, double width);
EXPORT void light_dll_ver();
EXPORT void light_fixup(char *name, void (*in));
#endif
