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
#ifndef exp_h
#define exp_h
void exp_cal_emission(char *extra, struct device *in);
double get_avg_field(struct device *in);
double get_np_tot(struct device *in);
void reset_npequlib(struct device *in);
void get_avg_np_pos(struct device *in, double *nx, double *px);
double get_background_charge(struct device *in);
void reset_npinit(struct device *in);
double get_n_trapped_charge(struct device *in);
double get_p_trapped_charge(struct device *in);
double get_avg_recom(struct device *in);
double get_avg_recom_n(struct device *in);
double get_avg_recom_p(struct device *in);
double get_J_recom_n(struct device *in);
double get_J_recom_p(struct device *in);
double get_avg_k(struct device *in);
double get_avg_mue(struct device *in);
double get_avg_muh(struct device *in);
double get_free_n_charge(struct device *in);
double get_free_p_charge(struct device *in);
double get_free_n_charge_delta(struct device *in);
double get_free_p_charge_delta(struct device *in);
double get_total_n_trapped_charge(struct device *in);
double get_total_p_trapped_charge(struct device *in);
double get_n_trapped_charge_delta(struct device *in);
double get_p_trapped_charge_delta(struct device *in);
double get_avg_relax_n(struct device *in);
double get_avg_relax_p(struct device *in);
double get_avg_J(struct device *in);
double get_free_np_avg(struct device *in);
double get_extracted_np(struct device *in);
double get_extracted_k(struct device *in);
double get_charge_delta(struct device *in);
double get_I_recomb(struct device *in);
double get_J_left(struct device *in);
double get_J_right(struct device *in);
double get_J_recom(struct device *in);
double get_I_ce(struct device *in);
double get_equiv_I(struct device *in);
double carrier_count_get_rn(struct device *in);
double carrier_count_get_rp(struct device *in);

double carrier_count_get_n(struct device *in);
double carrier_count_get_p(struct device *in);
void carrier_count_reset(struct device *in);
void carrier_count_add(struct device *in);
double get_extracted_n(struct device *in);
double get_extracted_p(struct device *in);
double get_equiv_V(struct device *in);
double get_equiv_J(struct device *in);
double get_I(struct device *in);
double get_J(struct device *in);
double get_charge(struct device *in);
double get_avg_gen(struct device *in);
void set_orig_charge_den(struct device *in);
double get_total_np(struct device *in);
#endif
