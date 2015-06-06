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
#ifndef dynamic_store_h
#define dynamic_store_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "sim.h"
#include "dump.h"
#include "buffer.h"

struct dynamic_store {
	struct istruct charge_change;
	struct istruct jout;
	struct istruct jn_avg;
	struct istruct jp_avg;
	struct istruct dynamic_jn;
	struct istruct dynamic_jp;
	struct istruct jnout_mid;
	struct istruct jpout_mid;
	struct istruct iout;
	struct istruct iout_left;
	struct istruct iout_right;
	struct istruct gexout;
	struct istruct ntrap;
	struct istruct ptrap;
	struct istruct ntrap_delta_out;
	struct istruct ptrap_delta_out;
	struct istruct nfree;
	struct istruct pfree;
	struct istruct nfree_delta_out;
	struct istruct pfree_delta_out;
	struct istruct Rnpout;
	struct istruct nfree_to_ptrap;
	struct istruct pfree_to_ntrap;
	struct istruct Rnout;
	struct istruct Rpout;

	struct istruct nrelax_out;
	struct istruct prelax_out;
	struct istruct tpc_mue;
	struct istruct tpc_muh;
	struct istruct tpc_mu_avg;
	struct istruct tpc_filledn;
	struct istruct tpc_filledp;
	struct istruct dynamic_np;
	struct istruct only_n;
	struct istruct only_p;
	struct istruct E_field;
	struct istruct dynamic_pl;
	struct istruct dynamic_Vapplied;
	struct istruct dynamic_charge_tot;
	struct istruct dynamic_jn_drift;
	struct istruct dynamic_jn_diffusion;

	struct istruct dynamic_jp_drift;
	struct istruct dynamic_jp_diffusion;
	struct istruct dynamic_qe;

	struct istruct srh_n_r1;
	struct istruct srh_n_r2;
	struct istruct srh_n_r3;
	struct istruct srh_n_r4;

	struct istruct srh_p_r1;
	struct istruct srh_p_r2;
	struct istruct srh_p_r3;
	struct istruct srh_p_r4;

	struct istruct band_bend;

	double *band_snapshot;
};
#endif
