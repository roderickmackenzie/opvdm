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
#include "newton.h"
#include "dump.h"
#include <math.h>

void update_arrays(struct device *in)
{

	int i;
	int band = 0;

	for (i = 0; i < in->ymeshpoints; i++) {
		in->Fn[i] = in->x[i] - in->phi[i];
		in->Fp[i] = -in->xp[i] - in->phi[i];

		in->Ec[i] = -in->phi[i] - in->Xi[i];
		in->Ev[i] = -in->phi[i] - in->Xi[i] - in->Eg[i];

		in->dn[i] =
		    get_dn_den(in->x[i] + in->t[i], in->Te[i], in->imat[i]);
		in->n[i] =
		    get_n_den(in->x[i] + in->t[i], in->Te[i], in->imat[i]);
		in->dndphi[i] =
		    get_dn_den(in->x[i] + in->t[i], in->Te[i], in->imat[i]);
		in->dp[i] =
		    get_dp_den(in->xp[i] - in->tp[i], in->Th[i], in->imat[i]);
		in->p[i] =
		    get_p_den(in->xp[i] - in->tp[i], in->Th[i], in->imat[i]);
		in->dpdphi[i] =
		    -get_dp_den(in->xp[i] - in->tp[i], in->Th[i], in->imat[i]);

		in->wn[i] =
		    get_n_w(in->x[i] + in->t[i], in->Te[i], in->imat[i]);
		in->wp[i] =
		    get_p_w(in->xp[i] - in->tp[i], in->Th[i], in->imat[i]);

		in->mun[i] = get_n_mu(in->imat[i]);
		in->mup[i] = get_p_mu(in->imat[i]);
		if (in->ntrapnewton) {
			in->nt_all[i] = 0.0;
			for (band = 0; band < in->srh_bands; band++) {
				in->Fnt[i][band] = in->xt[i][band] - in->phi[i];

				in->srh_n_r1[i][band] =
				    get_n_srh(in->xt[i][band] + in->tt[i],
					      in->Te[i], band, srh_1,
					      in->imat[i]);
				in->srh_n_r2[i][band] =
				    get_n_srh(in->xt[i][band] + in->tt[i],
					      in->Te[i], band, srh_2,
					      in->imat[i]);
				in->srh_n_r3[i][band] =
				    get_n_srh(in->xt[i][band] + in->tt[i],
					      in->Te[i], band, srh_3,
					      in->imat[i]);
				in->srh_n_r4[i][band] =
				    get_n_srh(in->xt[i][band] + in->tt[i],
					      in->Te[i], band, srh_4,
					      in->imat[i]);
				in->dsrh_n_r1[i][band] =
				    get_dn_srh(in->xt[i][band] + in->tt[i],
					       in->Te[i], band, srh_1,
					       in->imat[i]);
				in->dsrh_n_r2[i][band] =
				    get_dn_srh(in->xt[i][band] + in->tt[i],
					       in->Te[i], band, srh_2,
					       in->imat[i]);
				in->dsrh_n_r3[i][band] =
				    get_dn_srh(in->xt[i][band] + in->tt[i],
					       in->Te[i], band, srh_3,
					       in->imat[i]);
				in->dsrh_n_r4[i][band] =
				    get_dn_srh(in->xt[i][band] + in->tt[i],
					       in->Te[i], band, srh_4,
					       in->imat[i]);

				in->nt[i][band] =
				    get_n_pop_srh(in->xt[i][band] + in->tt[i],
						  in->Te[i], band, in->imat[i]);
				in->dnt[i][band] =
				    get_dn_pop_srh(in->xt[i][band] + in->tt[i],
						   in->Te[i], band,
						   in->imat[i]);
				in->nt_all[i] += in->nt[i][band];

			}
		}

		if (in->ptrapnewton) {
			in->pt_all[i] = 0.0;
			for (band = 0; band < in->srh_bands; band++) {
				in->Fpt[i][band] =
				    -in->xpt[i][band] - in->phi[i];

				in->srh_p_r1[i][band] =
				    get_p_srh(in->xpt[i][band] - in->tpt[i],
					      in->Th[i], band, srh_1,
					      in->imat[i]);
				in->srh_p_r2[i][band] =
				    get_p_srh(in->xpt[i][band] - in->tpt[i],
					      in->Th[i], band, srh_2,
					      in->imat[i]);
				in->srh_p_r3[i][band] =
				    get_p_srh(in->xpt[i][band] - in->tpt[i],
					      in->Th[i], band, srh_3,
					      in->imat[i]);
				in->srh_p_r4[i][band] =
				    get_p_srh(in->xpt[i][band] - in->tpt[i],
					      in->Th[i], band, srh_4,
					      in->imat[i]);
				in->dsrh_p_r1[i][band] =
				    get_dp_srh(in->xpt[i][band] - in->tpt[i],
					       in->Th[i], band, srh_1,
					       in->imat[i]);
				in->dsrh_p_r2[i][band] =
				    get_dp_srh(in->xpt[i][band] - in->tpt[i],
					       in->Th[i], band, srh_2,
					       in->imat[i]);
				in->dsrh_p_r3[i][band] =
				    get_dp_srh(in->xpt[i][band] - in->tpt[i],
					       in->Th[i], band, srh_3,
					       in->imat[i]);
				in->dsrh_p_r4[i][band] =
				    get_dp_srh(in->xpt[i][band] - in->tpt[i],
					       in->Th[i], band, srh_4,
					       in->imat[i]);

				in->pt[i][band] =
				    get_p_pop_srh(in->xpt[i][band] - in->tpt[i],
						  in->Th[i], band, in->imat[i]);
				in->dpt[i][band] =
				    get_dp_pop_srh(in->xpt[i][band] -
						   in->tpt[i], in->Th[i], band,
						   in->imat[i]);
				in->pt_all[i] += in->pt[i][band];

			}
		}

	}

}

void init_mat_arrays(struct device *in)
{
	int i;
	for (i = 0; i < in->ymeshpoints; i++) {
		in->Tl[i] =
		    in->Tll + in->ymesh[i] * (in->Tlr - in->Tll) / in->ylen;
		in->Te[i] =
		    in->Tll + in->ymesh[i] * (in->Tlr - in->Tll) / in->ylen;
		in->Th[i] =
		    in->Tll + in->ymesh[i] * (in->Tlr - in->Tll) / in->ylen;
		in->ex[i] = 0.0;
		in->Hex[i] = 0.0;

		in->epsilonr[i] = get_dos_epsilonr(in->imat[i]);
		in->dostype[i] = 0;

		in->Eg[i] = get_dos_Eg(in->imat[i]);
		in->Dex[i] = 0.0;

		in->Xi[i] = get_dos_Xi(in->imat[i]);

		in->Ec[i] = -in->Xi[i];

		in->Ev[i] = -in->Xi[i] - in->Eg[i];

		in->Nc[i] = get_Nc_free(in->imat[i]);

		in->Nv[i] = get_Nv_free(in->imat[i]);

		in->mun[i] = get_n_mu(in->imat[i]);
		in->mup[i] = get_p_mu(in->imat[i]);

		in->kf[i] = 0.0;
		in->kl[i] = 0.0;
		in->ke[i] = get_n_mu(in->imat[i]);
		in->kh[i] = get_p_mu(in->imat[i]);

		in->Hl[i] = 0.0;
		in->He[i] = 0.0;
		in->Hh[i] = 0.0;
		in->Habs[i] = 0.0;

		in->t[i] = in->Xi[i];
		in->tp[i] = in->Xi[i] + in->Eg[i];

		in->tt[i] = in->Xi[i];
		in->tpt[i] = in->Xi[i] + in->Eg[i];

	}
}
