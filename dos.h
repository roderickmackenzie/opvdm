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

#ifndef dos_h
#define dos_h
#include "dos_types.h"

struct dosconfig {
	double edge;
	double m;
	int dostype;
	double Nt;
	double Et;
	double Nt2;
	double Et2;
	double Eshift;
	double nstart;
	double nstop;
	double base1;
	double base2;
	int npoints;
	int expan_len;
	double expan_N[20];
	double expan_E[20];
	double mu;
	double tau0;
	double tau1;
	double Tstart;
	double Tstop;
	double Ngaus;
	int Tsteps;
	int traps;
	double dband;
	double detrap;
	int srh_bands;
	double srh_start;

	double srh_sigman;
	double srh_sigmap;
	double srh_vth;
	double Nc;
	double Nv;
	double srh_cut;

	double del_start;
	double del_stop;
	double Eg;
	double epsilonr;
	double doping;
	double Xi;
	double gaus_mull;
	int Esteps;
};

struct dos {
	int used;
	double *x;
	int xlen;
	int tlen;
	int srh_bands;
	double *t;
	double *srh_E;
	double *srh_den;
	double **c;
	double **w;
	double ***srh_r1;
	double ***srh_r2;
	double ***srh_r3;
	double ***srh_r4;
	double ***srh_c;
	struct dosconfig config;
};

void dos_init();
void dos_free();
double get_dos_epsilonr(int mat);
double get_dos_doping(int mat);
double get_dos_E_n(int band, int mat);
double get_dos_E_p(int band, int mat);
void dos_free_now(struct dos *mydos);

double get_n_pop_srh(double top, double T, int trap, int mat);
double get_p_pop_srh(double top, double T, int trap, int mat);
double get_dn_pop_srh(double top, double T, int trap, int mat);
double get_dp_pop_srh(double top, double T, int trap, int mat);

void load_dos(struct device *dev, char *namen, char *namep, int mat);
double get_dn_trap_den(double top, double T, int type, int band, int mat);
double get_dp_trap_den(double top, double T, int type, int mat);
int search(double *x, int N, double find);
double get_n_srh(double top, double T, int trap, int r, int mat);
double get_dn_srh(double top, double T, int trap, int r, int mat);
double get_p_srh(double top, double T, int trap, int r, int mat);
double get_dp_srh(double top, double T, int trap, int r, int mat);
double dos_get_band_energy_n(int band, int mat);
double dos_get_band_energy_p(int band, int mat);
double dos_srh_get_fermi_p(double n, double p, int band, int mat);
double dos_srh_get_fermi_n(double n, double p, int band, int mat);
double get_Nc_free(int mat);
double get_Nv_free(int mat);
double get_n_mu(int mat);
double get_p_mu(int mat);
double get_dos_Eg(int mat);
double get_dos_Xi(int mat);

double get_dos_E_n(int band, int mat);
double get_dos_E_p(int band, int mat);
double get_n_w(double top, double T, int mat);
double get_p_w(double top, double T, int mat);
double get_top_from_n(double n, double T, int mat);
double get_top_from_p(double p, double T, int mat);
double get_n_den(double top, double T, int mat);
double get_p_den(double top, double T, int mat);
double get_n_mu(int mat);
double get_p_mu(int mat);
double get_dn_den(double top, double T, int mat);
double get_dp_den(double top, double T, int mat);
double get_dpdT_den(double top, double T, int mat);
double get_dndT_den(double top, double T, int mat);
double get_dos_filled_n(struct device *in);
double get_dos_filled_p(struct device *in);
void gen_dos_fd_gaus_n();
void gen_dos_fd_gaus_p();
int hashget(double *x, int N, double find);
void draw_gaus(struct device *in);

double get_dos_filled_n(struct device *in);
double get_dos_filled_p(struct device *in);
void safe_file(char *name);
#endif
