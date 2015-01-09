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
#ifndef h_light
#define h_light
#include <i.h>
#include <complex.h>

struct light {
	char config_file[300];
	int points;
	int lpoints;
	double *x;
	double dx;
	double **Ep;
	double **Epz;
	double **En;
	double **Enz;
	double **n;
	double **alpha;
	double **photons;
	double **photons_asb;
	double *photons_tot;
	double **H;
	int *layer;
	double *sun_E;
	double *H1d;
	int layers;
	double ylen;
	struct istruct *mat;
	struct istruct *mat_n;
	struct istruct sun_read;
	double *sun;
	double *sun_norm;
	double *sun_photons;
	double *thick;
	int M;
	int N;
	int *Ti;
	int *Tj;
	double *Tx;
	double *Txz;
	double *b;
	double *bz;
	double lstart;
	double lstop;
	double *l;
	double *Gn;
	double *Gp;
	double dl;
	double laser_wavelength;
	int laser_pos;
	double ND;
	double spotx;
	double spoty;
	double pulseJ;
	double pulseL;
	double complex **t;
	double complex **r;
	int gather;
	int *enable_gen;
	double *layer_end;
	double device_start;
	double *G_percent;
	double dlength;
	int align_mesh;
	int device_start_layer;
	int device_start_i;
	double Eg;
	double laser_photon_density;
	char output_path[300];
	double electron_eff;
	double hole_eff;
	int function;
	double Psun;
	double laser_eff;

};

void light_get_mode(struct istruct *mode, double lam, struct light *in);
double light_cal_photon_density(struct light *in);
void light_set_unity_power(struct light *in);
void light_solve_optical_problem(struct light *in);
void light_solve_all(struct light *in);
void light_set_dump(struct light *in, int dump);
void light_load_config(struct light *in, char *config_file);
void light_free(struct light *in);
void light_dump(struct light *in);
int light_solve_lam_slice(struct light *in, int lam);
void light_set_dx(struct light *in, double dx);
void light_dump_1d(struct light *in, int i);
#endif
