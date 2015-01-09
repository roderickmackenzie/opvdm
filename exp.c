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
#include "sim.h"
#include "i.h"
#include <math.h>

static double n_count = 0.0;
static double p_count = 0.0;
static double rn_count = 0.0;
static double rp_count = 0.0;

void exp_cal_emission(char *extra, struct device *in)
{
	return;
}

double get_avg_recom(struct device *in)
{
	int i;
	double sum = 0.0;
	double add = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		add = in->ptrap_to_n[i] + in->ntrap_to_p[i];
		if ((in->interfaceleft == TRUE) && (i == 0))
			add /= 2.0;
		if ((in->interfaceright == TRUE) && (i == in->ymeshpoints - 1))
			add /= 2.0;
		sum += add;
	}

	return sum / (((double)in->ymeshpoints));
}

double get_avg_relax_n(struct device *in)
{
	int i = 0;
	double Rtot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		Rtot += (in->nrelax[i]);
	}
	return Rtot / (((double)in->ymeshpoints));
}

double get_avg_relax_p(struct device *in)
{
	int i = 0;
	double Rtot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		Rtot += (in->prelax[i]);
	}
	return Rtot / (((double)in->ymeshpoints));
}

double get_avg_recom_n(struct device *in)
{
	int i = 0;
	double Rtot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		Rtot += (in->ptrap_to_n[i]);
	}
	return Rtot / (((double)in->ymeshpoints));
}

double get_avg_recom_p(struct device *in)
{
	int i = 0;
	double Rtot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		Rtot += (in->ntrap_to_p[i]);
	}
	return Rtot / (((double)in->ymeshpoints));
}

double get_avg_k(struct device *in)
{
	double n = (get_extracted_n(in) * get_extracted_p(in));
	double R = (get_avg_recom_n(in) + get_avg_recom_p(in)) / 2.0;
	double k = R / n;
	return k;
}

double carrier_count_get_n(struct device *in)
{
	return n_count;
}

double carrier_count_get_p(struct device *in)
{
	return p_count;
}

double carrier_count_get_rn(struct device *in)
{
	return rn_count;
}

double carrier_count_get_rp(struct device *in)
{
	return rp_count;
}

void carrier_count_reset(struct device *in)
{
	n_count = 0.0;
	p_count = 0.0;
	rn_count = 0.0;
	rp_count = 0.0;
}

double get_J_recom(struct device *in)
{
	int i = 0;
	double Rtot = 0.0;
	double add = 0.0;
	double dx = in->ymesh[1] - in->ymesh[0];
	for (i = 0; i < in->ymeshpoints; i++) {
		add =
		    (in->Rn[i] - in->Gn[i] - in->Rp[i] - in->Gn[i]) * dx / 2.0;
		if ((in->interfaceleft == TRUE) && (i == 0))
			add /= 2.0;
		if ((in->interfaceright == TRUE) && (i == in->ymeshpoints - 1))
			add /= 2.0;
		Rtot += add;
	}

	return Rtot * q;
}

double get_J_recom_n(struct device *in)
{
	int i = 0;
	double Rtot = 0.0;
	double dx = in->ymesh[1] - in->ymesh[0];
	for (i = 0; i < in->ymeshpoints; i++) {
		Rtot += (in->Rn[i]) * dx;
	}
	return Rtot;
}

double get_J_recom_p(struct device *in)
{
	int i = 0;
	double Rtot = 0.0;
	double dx = in->ymesh[1] - in->ymesh[0];
	for (i = 0; i < in->ymeshpoints; i++) {
		Rtot += (in->Rp[i]) * dx;
	}
	return Rtot;
}

double get_avg_J(struct device *in)
{
	int i = 0;
	double Jtot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		Jtot += fabs(in->Jn[i] + in->Jp[i]);
	}
	return Jtot / (double)in->ymeshpoints;
}

void carrier_count_add(struct device *in)
{
	int i;
	double locat_n_tot = 0.0;
	double locat_p_tot = 0.0;

	n_count += (in->Jn[0] + in->Jn[in->ymeshpoints - 1]) * in->dt / q;
	p_count += (in->Jp[0] + in->Jp[in->ymeshpoints - 1]) * in->dt / q;

	double dx = in->ymesh[1] - in->ymesh[0];

	for (i = 0; i < in->ymeshpoints; i++) {
		locat_n_tot += in->Rbi[i] * dx;
		locat_p_tot += in->Rbi[i] * dx;
	}

	rn_count += locat_n_tot * in->dt;
	rp_count += locat_p_tot * in->dt;

}

double get_extracted_np(struct device *in)
{
	return (get_extracted_n(in) + get_extracted_p(in)) / 2.0;
}

double get_extracted_n(struct device *in)
{
	int i;
	double sum_n = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		sum_n += (in->n[i] - in->nfinit[i]);
		sum_n += (in->nt_all[i] - in->ntinit[i]);

	}

	return sum_n / ((double)(in->ymeshpoints));
}

double get_total_np(struct device *in)
{
	int i;
	double sum = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		sum += (in->n[i] + in->p[i]);
		sum += (in->nt_all[i] + in->pt_all[i]);
	}

	return sum / ((double)(in->ymeshpoints) * 2.0);
}

double get_extracted_p(struct device *in)
{
	int i;

	double sum_p = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		sum_p += (in->p[i] - in->pfinit[i]);
		sum_p += (in->pt_all[i] - in->ptinit[i]);
	}

	return sum_p / ((double)(in->ymeshpoints));
}

double get_background_charge(struct device *in)
{
	int i;
	double sum = 0.0;

	for (i = 0; i < in->ymeshpoints; i++) {
		sum +=
		    (in->nfinit[i] + in->ntinit[i] + in->pfinit[i] +
		     in->ptinit[i]) / 2.0;
	}
	return sum / ((double)(in->ymeshpoints));
}

double get_extracted_k(struct device *in)
{
	int i;

	double tot = 0.0;
	double n = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += in->Rbi[i];
		n += in->n[i] * in->p[i];
	}

	return tot / n;
}

double get_avg_gen(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += (in->Gn[i] + in->Gp[i]) / 2.0;
	}
	return tot / (((double)in->ymeshpoints));
}

double get_avg_mue(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += in->mun[i] * in->n[i] / (in->nt_all[i] + in->n[i]);
	}
	return tot / (((double)in->ymeshpoints));
}

double get_avg_muh(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += in->mup[i] * in->p[i] / (in->pt_all[i] + in->p[i]);
	}
	return tot / (((double)in->ymeshpoints));
}

double get_np_tot(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += (in->n[i] + in->p[i] + in->pt_all[i] + in->nt_all[i]);
	}

	return tot / (((double)in->ymeshpoints)) / 2.0;
}

double get_free_np_avg(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += (in->n[i] + in->p[i] - in->nfinit[i] - in->pfinit[i]);
	}

	return tot / (((double)in->ymeshpoints)) / 2.0;
}

double get_free_n_charge(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += (in->n[i] - in->nfinit[i]);
	}

	return tot / (((double)in->ymeshpoints));
}

double get_free_p_charge(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += (in->p[i] - in->pfinit[i]);
	}

	return tot / (((double)in->ymeshpoints));
}

void set_orig_charge_den(struct device *in)
{
	int i;
	for (i = 0; i < in->ymeshpoints; i++) {
		in->n_orig[i] = in->n[i] + in->nt_all[i];
		in->p_orig[i] = in->p[i] + in->pt_all[i];

		in->n_orig_f[i] = in->n[i];
		in->p_orig_f[i] = in->p[i];

		in->n_orig_t[i] = in->nt_all[i];
		in->p_orig_t[i] = in->pt_all[i];
	}
}

double get_free_n_charge_delta(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += (in->n[i] - in->n_orig_f[i]);
	}

	return tot / (((double)in->ymeshpoints));
}

double get_free_p_charge_delta(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += (in->p[i] - in->p_orig_f[i]);
	}

	return tot / (((double)in->ymeshpoints));
}

void reset_npinit(struct device *in)
{
	int i;
	int band;
	for (i = 0; i < in->ymeshpoints; i++) {
		in->nfinit[i] = in->n[i];
		in->pfinit[i] = in->p[i];
		in->ntinit[i] = in->nt_all[i];
		in->ptinit[i] = in->pt_all[i];
		for (band = 0; band < in->srh_bands; band++) {
			in->ntbinit[i][band] = in->nt[i][band];
			in->ptbinit[i][band] = in->pt[i][band];
		}
	}
}

void reset_npequlib(struct device *in)
{
	int i;
	for (i = 0; i < in->ymeshpoints; i++) {
		in->nfequlib[i] = in->n[i];
		in->pfequlib[i] = in->p[i];
		in->ntequlib[i] = in->nt_all[i];
		in->ptequlib[i] = in->pt_all[i];
	}
}

double get_n_trapped_charge(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += (in->nt_all[i] - in->ntinit[i]);
	}

	return tot / in->ymeshpoints;
}

double get_p_trapped_charge(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += (in->pt_all[i] - in->ptinit[i]);
	}

	return tot / in->ymeshpoints;
}

double get_charge_delta(struct device *in)
{
	int i;
	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot +=
		    (((in->n[i] + in->nt_all[i]) - in->n_orig[i]) +
		     ((in->p[i] + in->pt_all[i]) - in->p_orig[i])) / 2.0;
	}

	return tot / in->ymeshpoints;
}

double get_n_trapped_charge_delta(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += (in->nt_all[i] - in->n_orig_t[i]);
	}

	return tot / in->ymeshpoints;
}

double get_p_trapped_charge_delta(struct device *in)
{
	int i;

	double tot = 0.0;
	for (i = 0; i < in->ymeshpoints; i++) {
		tot += (in->pt_all[i] - in->p_orig_t[i]);
	}

	return tot / in->ymeshpoints;
}

double get_I_recomb(struct device *in)
{
	double ret = 0.0;

	ret = (get_J_recom(in)) * (in->xlen * in->zlen) / 2.0;

	return ret;
}

double get_I(struct device *in)
{
	double ret = 0.0;

	ret += (get_J_left(in) + get_J_right(in)) * (in->xlen * in->zlen) / 2.0;

	return ret;
}

double get_equiv_I(struct device *in)
{
	double Iout = 0.0;
	Iout = get_equiv_J(in) * (in->xlen * in->zlen);
	return Iout;
}

double get_equiv_J(struct device *in)
{
	double J = 0.0;
	J = get_J(in);
	if (in->lr_pcontact == RIGHT)
		J *= -1.0;
	J += in->Vapplied / in->Rshunt / in->area;

	return J;
}

double get_I_ce(struct device *in)
{
	double ret = in->Vapplied / (in->Rcontact + in->Rshort);
	return ret;
}

double get_avg_field(struct device *in)
{
	return (in->phi[in->ymeshpoints - 1] - in->phi[0]);
}

double get_equiv_V(struct device *in)
{
	double J = 0.0;

	J = get_equiv_J(in);

	double V = J * in->Rcontact * in->area + in->Vapplied;
	return V;
}

double get_J(struct device *in)
{

	double ret = 0.0;

	ret = (get_J_left(in) + get_J_right(in)) / 2.0;

	return ret;
}

double get_J_left(struct device *in)
{

	double ret = 0.0;
	ret = in->Jpleft + in->Jnleft;

	return ret * q;
}

double get_J_right(struct device *in)
{

	double ret = 0.0;
	ret = in->Jnright + in->Jpright;

	return ret * q;
}
