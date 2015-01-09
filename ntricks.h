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
#ifndef ntricks_h
#define ntricks_h

struct newton_math_state {
	int max_electrical_itt;
	double min_cur_error;
	int newton_min_itt;
	double electrical_clamp;
	int newton_clever_exit;
};

void newton_push_state(struct device *in);
void newton_pop_state(struct device *in);
double sim_externalv(struct device *in, double wantedv);
double sim_i(struct device *in, double wantedi);
void save_state(struct device *in, double to);
int load_state(struct device *in, double voltage);
void ramp(struct device *in, double from, double to, double steps);
void ramp_externalv(struct device *in, double from, double to);
void set_ntricks_fast(int val);
double sim_voc(struct device *in);
#endif
