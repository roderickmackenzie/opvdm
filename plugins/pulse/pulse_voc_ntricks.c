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
//    the Free Software Foundation; version 2 of the License
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
#include "../../log.h"

void newton_aux_pulse_voc(struct device *in, double V, double *i, double *didv,
			  double *didphi, double *didxil, double *didxipl,
			  double *didphir, double *didxir, double *didxipr)
{
	return;
}

double pulse_newton_sim_voc_fast(struct device *in, int do_LC)
{
	pulse_newton_sim_voc(in);
	return get_I(in) + in->C * (in->Vapplied - in->Vapplied_last) +
	    in->Vapplied / in->Rshunt;
}

double pulse_newton_sim_voc(struct device *in)
{
	printf_log("Looking for Voc\n");
	double C = in->C;
	double clamp = 0.1;
	double step = 0.01;
	double e0;
	double e1;
	double i0;
	double i1;
	double deriv;
	double Rdrain = pulse_config.pulse_Rload + in->Rcontact;

	solve_all(in);
	i0 = get_I(in);
	e0 = fabs(i0 + in->Vapplied * (1.0 / in->Rshunt - 1.0 / Rdrain));

	in->Vapplied += step;
	solve_all(in);
	i1 = get_I(in);
	e1 = fabs(i1 + in->Vapplied * (1.0 / in->Rshunt - 1.0 / Rdrain));

	deriv = (e1 - e0) / step;
	step = -e1 / deriv;

	step = step / (1.0 + fabs(step / clamp));
	in->Vapplied += step;

	int count = 0;
	int max = 200;
	do {
		e0 = e1;
		solve_all(in);
		i1 = get_I(in);
		e1 = fabs(i1 +
			  in->Vapplied * (1.0 / in->Rshunt - 1.0 / Rdrain));
		deriv = (e1 - e0) / step;
		step = -e1 / deriv;

		step = step / (1.0 + fabs(step / clamp));
		in->Vapplied += step;
		if (get_dump_status(dump_print_text) == TRUE) {
			printf_log
			    ("%d pulse voc find Voc Vapplied=%lf step=%le error=%le\n",
			     count, in->Vapplied, step, e1);
		}
		if (count > max)
			break;
		count++;

	} while (e1 > 1e-12);

	double ret = in->Vapplied - C * (i1 - in->Ilast) / in->dt;
	return ret;
}

void pulse_set_light_for_voc(struct device *in, double Voc)
{
}
