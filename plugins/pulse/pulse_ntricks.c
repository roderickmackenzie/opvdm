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
#include <math.h>
#include "../../dump.h"
#include "../../sim.h"
#include "pulse.h"

static double glob_wanted = 0.0;
static int glob_use_cap = 0;

void newton_aux_pulse(struct device *in, double V, double *i, double *didv,
		      double *didphi, double *didxil, double *didxipl,
		      double *didphir, double *didxir, double *didxipr)
{

	double C = in->C;
	if (glob_use_cap == FALSE)
		C = 0.0;
	double i0 = *i;
	double didv0 = *didv;
	double didphi0 = *didphi;
	double didxil0 = *didxil;
	double didxipl0 = *didxipl;
	double didphir0 = *didphir;
	double didxir0 = *didxir;
	double didxipr0 = *didxipr;

	*i = glob_wanted - (V +
			    ((in->Rcontact + pulse_config.Rshort_pulse) * (i0 +
									   V /
									   in->
									   Rshunt
									   +
									   C *
									   (V -
									    in->
									    Vapplied_last)
									   /
									   in->
									   dt)));
	*didv =
	    -(1.0 +
	      ((in->Rcontact + pulse_config.Rshort_pulse) * (didv0 +
							     1.0 / in->Rshunt +
							     C * (1.0) /
							     in->dt)));
	*didphi = -((in->Rcontact + pulse_config.Rshort_pulse) * didphi0);
	*didxil = -((in->Rcontact + pulse_config.Rshort_pulse) * didxil0);
	*didxipl = -((in->Rcontact + pulse_config.Rshort_pulse) * didxipl0);
	*didphir = -((in->Rcontact + pulse_config.Rshort_pulse) * didphir0);
	*didxir = -((in->Rcontact + pulse_config.Rshort_pulse) * didxir0);
	*didxipr = -((in->Rcontact + pulse_config.Rshort_pulse) * didxipr0);

	return;
}

double newton_pulse(struct device *in, double Vtot, int usecap)
{

	double C = in->C;
	in->kl_in_newton = TRUE;
	solver_realloc(in);
	glob_wanted = Vtot;
	glob_use_cap = usecap;
	in->newton_aux = &newton_aux_pulse;
	solve_all(in);
	if (glob_use_cap == FALSE)
		C = 0.0;
	return get_I(in) + in->Vapplied / in->Rshunt + C * (in->Vapplied -
							    in->Vapplied_last) /
	    in->dt;

}
