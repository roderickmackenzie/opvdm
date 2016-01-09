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

#ifndef device_h
#define device_h
#include "code_ctrl.h"
#include "light.h"
#include "epitaxy.h"

struct mesh {
	double number;
	double len;
	double den;
};

struct device {
	struct epitaxy my_epitaxy;
	int remesh;
	int newmeshsize;
	double Jnleft;
	double Jnright;
	double Jpleft;
	double Jpright;
	double *phi;
	double *Nad;
	double *G;
	double *Gn;
	double *Gp;
	double *n;
	double *p;
	double *dn;
	double *dndphi;
	double *dp;
	double *dpdphi;
	double *Eg;
	double *Xi;
	double *Ev;
	double *Ec;
	double *Rfree;

	double *mun;
	double *mup;

	double *Dn;
	double *Dp;

	double *epsilonr;

	double *Fn;
	double *Fp;
	double *Nc;
	double *Nv;
	double *Tl;
	double *Te;
	double *Th;
	double *ymesh;
	double *R;
	double *Fi;
	int *imat;
	double *Jn;
	double *Jp;

	double *Jn_diffusion;
	double *Jn_drift;

	double *Jp_diffusion;
	double *Jp_drift;

	double Vapplied;
	int ymeshpoints;
	double Vl;
	double Vr;
	double *x;
	double *t;
	double *xp;
	double *tp;
	double *b;
	double *kf;
	double *kd;
	double *kr;

	double *Rn;
	double *Rp;
	double *kl;
	double *ke;
	double *kh;
	double *Hl;
	double *He;
	double *Hh;
	double *Habs;
	double Psun;
	int excite_conv;
	int thermal_conv;
	int newton_enable_external_thermal;

	double deltaFln;
	double deltaFlp;
	double deltaFrn;
	double deltaFrp;

	double *Rbi_k;

	double *ex;
	double *Dex;
	double *Hex;

	double *nf_save;
	double *pf_save;
	double *nt_save;
	double *pt_save;

	double *nfequlib;
	double *pfequlib;
	double *ntequlib;
	double *ptequlib;

	double **ntb_save;
	double **ptb_save;

	double *phi_save;

	double xlen;
	double ylen;
	double zlen;

	int N;
	int M;
	int *Ti;		//row
	int *Tj;		//col
	double *Tx;		//data

	int lr_pcontact;
	int invert_applied_bias;

//plotting
	FILE *gnuplot;
	FILE *gnuplot_time;
	FILE *converge;
	FILE *tconverge;
	int max_electrical_itt;
	double electrical_clamp;
	int max_electrical_itt0;
	double electrical_clamp0;
	double electrical_error0;
	int math_enable_pos_solver;
	double min_cur_error;
	double Pmax_voltage;
	int pos_max_ittr;
//Device characterisation
	double Voc;
	double Jsc;
	double FF;
	double Pmax;
	double Tll;
	double Tlr;
	int Tliso;
	double dt;
	int srh_sim;
	int go_time;
	double time;
	double *nlast;
	double *plast;
	int ntrapnewton;
	int ptrapnewton;

	int stop;
	double Rshort;
	struct mesh *meshdata;
	int ymeshlayers;
	int onlypos;
	int odes;
	double last_error;
	double posclamp;
	int srh_bands;
	double *wn;
	double *wp;

//n traps
	double *nt_all;
	double **nt;
	double **ntlast;
	double **dnt;
	double **srh_n_r1;
	double **srh_n_r2;
	double **srh_n_r3;
	double **srh_n_r4;
	double **dsrh_n_r1;
	double **dsrh_n_r2;
	double **dsrh_n_r3;
	double **dsrh_n_r4;
	double **Fnt;
	double **xt;
	double *tt;

	double **nt_r1;
	double **nt_r2;
	double **nt_r3;
	double **nt_r4;
//p traps
	double *pt_all;
	double **pt;
	double **ptlast;
	double **dpt;
	double **srh_p_r1;
	double **srh_p_r2;
	double **srh_p_r3;
	double **srh_p_r4;
	double **dsrh_p_r1;
	double **dsrh_p_r2;
	double **dsrh_p_r3;
	double **dsrh_p_r4;
	double **Fpt;
	double **xpt;
	double *tpt;

	double **pt_r1;
	double **pt_r2;
	double **pt_r3;
	double **pt_r4;

	double A;
	double Vol;

	double Rshunt;
	double Rcontact;

	int lr_bias;

	int *dostype;

	int interfaceleft;
	int interfaceright;
	double phibleft;
	double phibright;
	double vl_e;
	double vl_h;
	double vr_e;
	double vr_h;
	int stop_start;
	double externalv;
	double Vapplied_last;
	double Ilast;
	int timedumpcount;
	char outputpath[200];
	char inputpath[200];
	char simmode[200];
	double area;

	double *nrelax;
	double *ntrap_to_p;
	double *prelax;
	double *ptrap_to_n;

	double lcharge;
	double rcharge;

	double l_electrons;
	double l_holes;
	double r_electrons;
	double r_holes;

	int dumpitdos;

	double t_big_offset;

	double C;
	double other_layers;
	int last_ittr;

	int kl_in_newton;
	int config_kl_in_newton;
	void (*newton_aux) (struct device *, double, double *, double *,
			    double *, double *, double *, double *, double *,
			    double *);
	double *B;
	double xnl_left;
	double xpl_left;
	int stoppoint;
	double ilast;

	int newton_clever_exit;
	char plot_file[100];

	double start_stop_time;

	double Is;
	double n_id;
	double Igen;
	struct light mylight;
	int dump_movie;

	double *n_orig;
	double *p_orig;
	double *n_orig_f;
	double *p_orig_f;
	double *n_orig_t;
	double *p_orig_t;
	int nofluxl;

	double Vbi;
	int newton_min_itt;
	double vbi;
	double avg_gen;
	int dump_slicepos;
	double pl_intensity;

	double Rext;
	double Cext;
	double VCext_last;
	double VCext;
	int newton_last_ittr;
};
#endif
