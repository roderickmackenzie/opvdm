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
#include <math.h>
#include <gsl/gsl_multimin.h>

#include "sim.h"
#include "dump.h"
#include "inp.h"
#include "buffer.h"
#include "dynamic_store.h"
#include "gui_hooks.h"
#include "fit_sin.h"
#include "i.h"

struct istruct out_i;
struct istruct fit_data;

char *fit_file_prefix;

double last_d = 0.0;
double last_magnitude = 0.0;

double sin_f(const gsl_vector * v, void *params)
{
//struct buffer buf;
//buffer_init(&buf);
//buf.norm_x_axis=FALSE;
	double e0;
	double mag = gsl_vector_get(v, 0);
	double delta = gsl_vector_get(v, 1);
	double *p = (double *)params;
	double fit_fx = p[0];
	struct istruct test_i;
	inter_copy(&test_i, &fit_data, TRUE);
	double avg = inter_avg(&test_i);
	inter_sub_double(&test_i, avg);
//char name[200];

/*buffer_malloc(&buf);
buf.y_mul=1.0;
buf.x_mul=1.0;
strcpy(buf.title,"Current origonal- Time");
strcpy(buf.type,"xy");
strcpy(buf.x_label,"Time");
strcpy(buf.y_label,"Current");
strcpy(buf.x_units,"$s$");
strcpy(buf.y_units,"$A$");
buf.logscale_x=0;
buf.logscale_y=0;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,fit_data.x, fit_data.data, fit_data.len);
sprintf(name,"imps_fit_%s_orig.dat",fit_file_prefix);
buffer_dump(name,&buf);
buffer_free(&buf);*/

	inter_sin(&test_i, mag, fit_fx, delta);

/*buffer_malloc(&buf);
buf.y_mul=1.0;
buf.x_mul=1.0;
strcpy(buf.title,"Current guess - Time");
strcpy(buf.type,"xy");
strcpy(buf.x_label,"Time");
strcpy(buf.y_label,"Current");
strcpy(buf.x_units,"$s$");
strcpy(buf.y_units,"$A$");
buf.logscale_x=0;
buf.logscale_y=0;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,test_i.x, test_i.data, test_i.len);
sprintf(name,"imps_fit_%s_guess.dat",fit_file_prefix);
buffer_dump(name,&buf);
buffer_free(&buf);*/

	inter_sub(&test_i, &fit_data);

/*buffer_malloc(&buf);
buf.y_mul=1.0;
buf.x_mul=1.0;
strcpy(buf.title,"Current fit error - Time");
strcpy(buf.type,"xy");
strcpy(buf.x_label,"Time");
strcpy(buf.y_label,"Current");
strcpy(buf.x_units,"$s$");
strcpy(buf.y_units,"$A$");
buf.logscale_x=0;
buf.logscale_y=0;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,test_i.x, test_i.data, test_i.len);
sprintf(name,"imps_fit_%s_delta.dat",fit_file_prefix);
buffer_dump(name,&buf);
buffer_free(&buf);*/

	inter_mod(&test_i);
	e0 = inter_intergrate(&test_i);
	inter_free(&test_i);
	printf("error=%le mag=%le delta=%le fx=%le\n", e0, mag, delta, fit_fx);
	last_d = delta;
	last_magnitude = mag;
	return e0;
}

void fit_sin(double *ret_mag, double *ret_delta, struct istruct *in, double fx,
	     char *prefix)
{
	double size = 0.0;
	int status = 0;
	int fitvars = 2;
	double shift = 1.0;
	fit_file_prefix = prefix;
	inter_copy(&fit_data, in, TRUE);
	double avg = inter_avg(&fit_data);
	inter_sub_double(&fit_data, avg);

	struct istruct peaks;
	inter_init(&peaks);
	inter_find_peaks(&peaks, &fit_data, TRUE);
	double mag = peaks.data[0];
	double time_quater = (1.0 / fx) / 4.0;

	double delta = peaks.x[0] - time_quater;
	double par[5] = { 1.0, 2.0, 10.0, 20.0, 30.0 };
	par[0] = fx;
	gsl_vector *x;
	gsl_vector *ss;

	x = gsl_vector_alloc(fitvars);
	ss = gsl_vector_alloc(fitvars);

	printf("%le %le\n", mag, delta);

	int pos = 0;
	gsl_vector_set(x, pos, mag);
	gsl_vector_set(ss, pos, fabs(mag * shift));
	pos++;
	gsl_vector_set(x, pos, delta);
	gsl_vector_set(ss, pos, fabs(delta * shift));
	pos++;

	const gsl_multimin_fminimizer_type *T =
	    gsl_multimin_fminimizer_nmsimplex;
	gsl_multimin_function minex_func;
	gsl_multimin_fminimizer *s = NULL;

	minex_func.n = fitvars;
	minex_func.f = sin_f;
	minex_func.params = par;

	s = gsl_multimin_fminimizer_alloc(T, fitvars);
	gsl_multimin_fminimizer_set(s, &minex_func, x, ss);
	int stop = FALSE;
	int ittr = 0;
	do {
		status = gsl_multimin_fminimizer_iterate(s);

		size = gsl_multimin_fminimizer_size(s);
		status = gsl_multimin_test_size(size, 1e-8);

		//if (status == GSL_SUCCESS)
		//{
		//      printf ("converged to minimum at\n");
		//}

		if (status != GSL_CONTINUE)
			stop = TRUE;
		mag = gsl_vector_get(s->x, 0);
		delta = gsl_vector_get(s->x, 1);
		//printf("%s %le %10.3e delta=%10.3e\n",prefix,s->fval,mag,delta);
		if (ittr > 100) {
			stop = TRUE;
		}
		ittr++;
	} while (stop == FALSE);

	*ret_mag = last_magnitude;
	*ret_delta = last_d;

	gsl_multimin_fminimizer_free(s);
	gsl_vector_free(x);
	gsl_vector_free(ss);
	inter_free(&peaks);
	inter_free(&fit_data);

}
