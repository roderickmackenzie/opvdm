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
#include "dump.h"
#include "mesh.h"
#include <math.h>
#include "log.h"

void free_srh_bands(struct device *in, double **var)
{
	int i;
	for (i = 0; i < in->ymeshpoints; i++) {
		free(var[i]);
	}

	free(var);
}

void malloc_srh_bands(struct device *in, double *(**var))
{
	*var = (double **)malloc(in->ymeshpoints * sizeof(double *));

	int i;
	for (i = 0; i < in->ymeshpoints; i++) {
		(*var)[i] = (double *)malloc(in->srh_bands * sizeof(double));
	}
}

void device_free(struct device *in)
{
	free(in->phi);
	free(in->B);
	free(in->Nad);
	free(in->n);
	free(in->p);
	free(in->dn);
	free(in->dp);
	free(in->dndphi);
	free(in->dpdphi);
	free(in->Eg);
	free(in->Xi);
	free(in->Ev);
	free(in->Ec);
	free(in->mun);
	free(in->mup);
	free(in->Dn);
	free(in->Dp);
	free(in->ymesh);
	free(in->Fn);
	free(in->Fp);

	free(in->Nc);
	free(in->Nv);
	free(in->G);
	free(in->Gn);
	free(in->Gp);
	free(in->Tl);
	free(in->Te);
	free(in->Th);
	free(in->R);
	free(in->Fi);
	free(in->Jn);
	free(in->Jp);
	free(in->Jn_drift);
	free(in->Jn_diffusion);
	free(in->Jp_drift);
	free(in->Jp_diffusion);
	free(in->x);
	free(in->t);
	free(in->xp);
	free(in->tp);
	free(in->ex);
	free(in->Dex);
	free(in->Hex);
	free(in->epsilonr);

	free(in->kf);
	free(in->kd);
	free(in->kr);
	free(in->Rfree);
	free(in->Rn);
	free(in->Rp);
	free(in->kl);
	free(in->ke);
	free(in->kh);
	free(in->Hl);
	free(in->He);
	free(in->Hh);
	free(in->Habs);
	free(in->nlast);
	free(in->plast);

	free(in->wn);
	free(in->wp);

	free(in->nt_all);
	free_srh_bands(in, in->nt);
	free_srh_bands(in, in->xt);
	free_srh_bands(in, in->dnt);
	free_srh_bands(in, in->srh_n_r1);
	free_srh_bands(in, in->srh_n_r2);
	free_srh_bands(in, in->srh_n_r3);
	free_srh_bands(in, in->srh_n_r4);
	free_srh_bands(in, in->dsrh_n_r1);
	free_srh_bands(in, in->dsrh_n_r2);
	free_srh_bands(in, in->dsrh_n_r3);
	free_srh_bands(in, in->dsrh_n_r4);
	free_srh_bands(in, in->Fnt);
	free_srh_bands(in, in->ntb_save);

	free_srh_bands(in, in->nt_r1);
	free_srh_bands(in, in->nt_r2);
	free_srh_bands(in, in->nt_r3);
	free_srh_bands(in, in->nt_r4);

	free_srh_bands(in, in->ntlast);

	free(in->tt);
	free(in->dostype);
	free(in->Rbi_k);

	free(in->pt_all);
	free_srh_bands(in, in->pt);
	free_srh_bands(in, in->dpt);
	free_srh_bands(in, in->xpt);
	free_srh_bands(in, in->srh_p_r1);
	free_srh_bands(in, in->srh_p_r2);
	free_srh_bands(in, in->srh_p_r3);
	free_srh_bands(in, in->srh_p_r4);
	free_srh_bands(in, in->dsrh_p_r1);
	free_srh_bands(in, in->dsrh_p_r2);
	free_srh_bands(in, in->dsrh_p_r3);
	free_srh_bands(in, in->dsrh_p_r4);
	free_srh_bands(in, in->Fpt);
	free_srh_bands(in, in->ptb_save);

	free_srh_bands(in, in->pt_r1);
	free_srh_bands(in, in->pt_r2);
	free_srh_bands(in, in->pt_r3);
	free_srh_bands(in, in->pt_r4);

	free_srh_bands(in, in->ptlast);

	free(in->tpt);

	free(in->nf_save);
	free(in->pf_save);
	free(in->nt_save);
	free(in->pt_save);

	free(in->nfequlib);
	free(in->pfequlib);
	free(in->ntequlib);
	free(in->ptequlib);

	mesh_free(in);

	solver_free();
	complex_solver_free();

	free(in->nrelax);
	free(in->ntrap_to_p);
	free(in->prelax);
	free(in->ptrap_to_n);

	free(in->n_orig);
	free(in->p_orig);
	free(in->n_orig_f);
	free(in->p_orig_f);
	free(in->n_orig_t);
	free(in->p_orig_t);

	free(in->phi_save);

	printf_log("%s %i %s\n", _("Solved"), in->odes, _("Equations"));
}

void device_get_memory(struct device *in)
{
	in->odes = 0;

	in->nf_save = malloc(in->ymeshpoints * sizeof(double));
	memset(in->nf_save, 0, in->ymeshpoints * sizeof(double));

	in->pf_save = malloc(in->ymeshpoints * sizeof(double));
	memset(in->pf_save, 0, in->ymeshpoints * sizeof(double));

	in->nt_save = malloc(in->ymeshpoints * sizeof(double));
	memset(in->nt_save, 0, in->ymeshpoints * sizeof(double));

	in->pt_save = malloc(in->ymeshpoints * sizeof(double));
	memset(in->pt_save, 0, in->ymeshpoints * sizeof(double));

	in->nfequlib = malloc(in->ymeshpoints * sizeof(double));
	memset(in->nfequlib, 0, in->ymeshpoints * sizeof(double));

	in->pfequlib = malloc(in->ymeshpoints * sizeof(double));
	memset(in->pfequlib, 0, in->ymeshpoints * sizeof(double));

	in->ntequlib = malloc(in->ymeshpoints * sizeof(double));
	memset(in->ntequlib, 0, in->ymeshpoints * sizeof(double));

	in->ptequlib = malloc(in->ymeshpoints * sizeof(double));
	memset(in->ptequlib, 0, in->ymeshpoints * sizeof(double));

	in->Habs = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Habs, 0, in->ymeshpoints * sizeof(double));

	in->phi = malloc(in->ymeshpoints * sizeof(double));
	memset(in->phi, 0, in->ymeshpoints * sizeof(double));

	in->B = malloc(in->ymeshpoints * sizeof(double));
	memset(in->B, 0, in->ymeshpoints * sizeof(double));

	in->Nad = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Nad, 0, in->ymeshpoints * sizeof(double));

	in->n = malloc(in->ymeshpoints * sizeof(double));
	memset(in->n, 0, in->ymeshpoints * sizeof(double));

	in->p = malloc(in->ymeshpoints * sizeof(double));
	memset(in->p, 0, in->ymeshpoints * sizeof(double));
	//malloc_srh_bands(in,&(in->n));
	//malloc_srh_bands(in,&(in->p));
	in->dn = malloc(in->ymeshpoints * sizeof(double));
	memset(in->dn, 0, in->ymeshpoints * sizeof(double));

	in->dp = malloc(in->ymeshpoints * sizeof(double));
	memset(in->dp, 0, in->ymeshpoints * sizeof(double));

	in->dndphi = malloc(in->ymeshpoints * sizeof(double));
	memset(in->dndphi, 0, in->ymeshpoints * sizeof(double));

	in->dpdphi = malloc(in->ymeshpoints * sizeof(double));
	memset(in->dpdphi, 0, in->ymeshpoints * sizeof(double));

	in->Eg = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Eg, 0, in->ymeshpoints * sizeof(double));

	in->Fn = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Fn, 0, in->ymeshpoints * sizeof(double));

	in->Fp = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Fp, 0, in->ymeshpoints * sizeof(double));

	in->Xi = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Xi, 0, in->ymeshpoints * sizeof(double));

	in->Ev = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Ev, 0, in->ymeshpoints * sizeof(double));

	in->Ec = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Ec, 0, in->ymeshpoints * sizeof(double));

	in->mun = malloc(in->ymeshpoints * sizeof(double));
	memset(in->mun, 0, in->ymeshpoints * sizeof(double));

	in->mup = malloc(in->ymeshpoints * sizeof(double));
	memset(in->mup, 0, in->ymeshpoints * sizeof(double));

	in->Dn = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Dn, 0, in->ymeshpoints * sizeof(double));

	in->Dp = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Dp, 0, in->ymeshpoints * sizeof(double));

	in->Nc = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Nc, 0, in->ymeshpoints * sizeof(double));

	in->Nv = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Nv, 0, in->ymeshpoints * sizeof(double));

	in->G = malloc(in->ymeshpoints * sizeof(double));
	memset(in->G, 0, in->ymeshpoints * sizeof(double));

	in->Gn = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Gn, 0, in->ymeshpoints * sizeof(double));

	in->Gp = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Gp, 0, in->ymeshpoints * sizeof(double));

	in->Tl = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Tl, 0, in->ymeshpoints * sizeof(double));

	in->Te = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Te, 0, in->ymeshpoints * sizeof(double));

	in->Th = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Th, 0, in->ymeshpoints * sizeof(double));

	in->R = malloc(in->ymeshpoints * sizeof(double));
	memset(in->R, 0, in->ymeshpoints * sizeof(double));

	in->Fi = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Fi, 0, in->ymeshpoints * sizeof(double));

	in->Jn = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Jn, 0, in->ymeshpoints * sizeof(double));

	in->Jp = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Jp, 0, in->ymeshpoints * sizeof(double));

	in->Jn_drift = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Jn_drift, 0, in->ymeshpoints * sizeof(double));

	in->Jn_diffusion = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Jn_diffusion, 0, in->ymeshpoints * sizeof(double));

	in->Jp_drift = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Jp_drift, 0, in->ymeshpoints * sizeof(double));

	in->Jp_diffusion = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Jp_diffusion, 0, in->ymeshpoints * sizeof(double));

	in->x = malloc(in->ymeshpoints * sizeof(double));
	memset(in->x, 0, in->ymeshpoints * sizeof(double));

	in->t = malloc(in->ymeshpoints * sizeof(double));
	memset(in->t, 0, in->ymeshpoints * sizeof(double));

	in->xp = malloc(in->ymeshpoints * sizeof(double));
	memset(in->xp, 0, in->ymeshpoints * sizeof(double));

	in->tp = malloc(in->ymeshpoints * sizeof(double));
	memset(in->tp, 0, in->ymeshpoints * sizeof(double));

	in->kf = malloc(in->ymeshpoints * sizeof(double));
	memset(in->kf, 0, in->ymeshpoints * sizeof(double));

	in->kd = malloc(in->ymeshpoints * sizeof(double));
	memset(in->kd, 0, in->ymeshpoints * sizeof(double));

	in->kr = malloc(in->ymeshpoints * sizeof(double));
	memset(in->kr, 0, in->ymeshpoints * sizeof(double));

	in->Rfree = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Rfree, 0, in->ymeshpoints * sizeof(double));

	in->Rn = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Rn, 0, in->ymeshpoints * sizeof(double));

	in->Rp = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Rp, 0, in->ymeshpoints * sizeof(double));

	in->ex = malloc(in->ymeshpoints * sizeof(double));
	memset(in->ex, 0, in->ymeshpoints * sizeof(double));

	in->Dex = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Dex, 0, in->ymeshpoints * sizeof(double));

	in->Hex = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Hex, 0, in->ymeshpoints * sizeof(double));

	in->epsilonr = malloc(in->ymeshpoints * sizeof(double));
	memset(in->epsilonr, 0, in->ymeshpoints * sizeof(double));

	in->kl = malloc(in->ymeshpoints * sizeof(double));
	memset(in->kl, 0, in->ymeshpoints * sizeof(double));

	in->ke = malloc(in->ymeshpoints * sizeof(double));
	memset(in->ke, 0, in->ymeshpoints * sizeof(double));

	in->kh = malloc(in->ymeshpoints * sizeof(double));
	memset(in->kh, 0, in->ymeshpoints * sizeof(double));

	in->Hl = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Hl, 0, in->ymeshpoints * sizeof(double));

	in->He = malloc(in->ymeshpoints * sizeof(double));
	memset(in->He, 0, in->ymeshpoints * sizeof(double));

	in->Hh = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Hh, 0, in->ymeshpoints * sizeof(double));

	in->nlast = malloc(in->ymeshpoints * sizeof(double));
	memset(in->nlast, 0, in->ymeshpoints * sizeof(double));

	in->plast = malloc(in->ymeshpoints * sizeof(double));
	memset(in->plast, 0, in->ymeshpoints * sizeof(double));

	in->wn = malloc(in->ymeshpoints * sizeof(double));
	memset(in->wn, 0, in->ymeshpoints * sizeof(double));

	in->wp = malloc(in->ymeshpoints * sizeof(double));
	memset(in->wp, 0, in->ymeshpoints * sizeof(double));

	in->nt_all = malloc(in->ymeshpoints * sizeof(double));
	memset(in->nt_all, 0, in->ymeshpoints * sizeof(double));

	in->phi_save = malloc(in->ymeshpoints * sizeof(double));
	memset(in->phi_save, 0, in->ymeshpoints * sizeof(double));

	malloc_srh_bands(in, &(in->nt));
	malloc_srh_bands(in, &(in->ntlast));

	malloc_srh_bands(in, &(in->xt));
	malloc_srh_bands(in, &(in->dnt));
	malloc_srh_bands(in, &(in->srh_n_r1));
	malloc_srh_bands(in, &(in->srh_n_r2));
	malloc_srh_bands(in, &(in->srh_n_r3));
	malloc_srh_bands(in, &(in->srh_n_r4));
	malloc_srh_bands(in, &(in->dsrh_n_r1));
	malloc_srh_bands(in, &(in->dsrh_n_r2));
	malloc_srh_bands(in, &(in->dsrh_n_r3));
	malloc_srh_bands(in, &(in->dsrh_n_r4));
	malloc_srh_bands(in, &(in->Fnt));
	malloc_srh_bands(in, &(in->ntb_save));

	malloc_srh_bands(in, &(in->nt_r1));
	malloc_srh_bands(in, &(in->nt_r2));
	malloc_srh_bands(in, &(in->nt_r3));
	malloc_srh_bands(in, &(in->nt_r4));

	in->tt = malloc(in->ymeshpoints * sizeof(double));
	memset(in->tt, 0, in->ymeshpoints * sizeof(double));

	in->dostype = malloc(in->ymeshpoints * sizeof(double));
	memset(in->dostype, 0, in->ymeshpoints * sizeof(double));

	in->pt_all = malloc(in->ymeshpoints * sizeof(double));
	memset(in->pt_all, 0, in->ymeshpoints * sizeof(double));

	malloc_srh_bands(in, &(in->pt));
	malloc_srh_bands(in, &(in->ptlast));

	malloc_srh_bands(in, &(in->xpt));
	malloc_srh_bands(in, &(in->dpt));
	malloc_srh_bands(in, &(in->srh_p_r1));
	malloc_srh_bands(in, &(in->srh_p_r2));
	malloc_srh_bands(in, &(in->srh_p_r3));
	malloc_srh_bands(in, &(in->srh_p_r4));
	malloc_srh_bands(in, &(in->dsrh_p_r1));
	malloc_srh_bands(in, &(in->dsrh_p_r2));
	malloc_srh_bands(in, &(in->dsrh_p_r3));
	malloc_srh_bands(in, &(in->dsrh_p_r4));
	malloc_srh_bands(in, &(in->ptb_save));
	malloc_srh_bands(in, &(in->Fpt));

	malloc_srh_bands(in, &(in->pt_r1));
	malloc_srh_bands(in, &(in->pt_r2));
	malloc_srh_bands(in, &(in->pt_r3));
	malloc_srh_bands(in, &(in->pt_r4));

	in->tpt = malloc(in->ymeshpoints * sizeof(double));
	memset(in->tpt, 0, in->ymeshpoints * sizeof(double));

	in->Rbi_k = malloc(in->ymeshpoints * sizeof(double));
	memset(in->Rbi_k, 0, in->ymeshpoints * sizeof(double));

	in->nrelax = malloc(in->ymeshpoints * sizeof(double));
	memset(in->nrelax, 0, in->ymeshpoints * sizeof(double));

	in->ntrap_to_p = malloc(in->ymeshpoints * sizeof(double));
	memset(in->ntrap_to_p, 0, in->ymeshpoints * sizeof(double));

	in->prelax = malloc(in->ymeshpoints * sizeof(double));
	memset(in->prelax, 0, in->ymeshpoints * sizeof(double));

	in->ptrap_to_n = malloc(in->ymeshpoints * sizeof(double));
	memset(in->ptrap_to_n, 0, in->ymeshpoints * sizeof(double));

	in->n_orig = (double *)malloc(sizeof(double) * in->ymeshpoints);
	memset(in->n_orig, 0, in->ymeshpoints * sizeof(double));

	in->p_orig = (double *)malloc(sizeof(double) * in->ymeshpoints);
	memset(in->p_orig, 0, in->ymeshpoints * sizeof(double));

	in->n_orig_f = (double *)malloc(sizeof(double) * in->ymeshpoints);
	memset(in->n_orig_f, 0, in->ymeshpoints * sizeof(double));

	in->p_orig_f = (double *)malloc(sizeof(double) * in->ymeshpoints);
	memset(in->p_orig_f, 0, in->ymeshpoints * sizeof(double));

	in->n_orig_t = (double *)malloc(sizeof(double) * in->ymeshpoints);
	memset(in->n_orig_t, 0, in->ymeshpoints * sizeof(double));

	in->p_orig_t = (double *)malloc(sizeof(double) * in->ymeshpoints);
	memset(in->p_orig_t, 0, in->ymeshpoints * sizeof(double));
}
