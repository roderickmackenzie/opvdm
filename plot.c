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

#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sim.h"
#include "dump.h"
#include "plot.h"
#include <string.h>

struct timeval last_time;

static char plot_script_dir[500];

void set_plot_script_dir(char *in)
{
	strcpy(plot_script_dir, in);
	strcat(plot_script_dir, "/plot/");
}

void plot_open(struct device *in)
{
	if (get_dump_status(dump_plot) == TRUE) {
		gettimeofday(&last_time, NULL);

		in->gnuplot = popen("gnuplot -persist", "w");
		fprintf(in->gnuplot,
			"set terminal x11 title 'Organic Photovoltaic Device Model - www.opvdm.com' \n");

		fflush(in->gnuplot);

		if (in->plottime == TRUE) {
			in->gnuplot_time = popen("gnuplot -persist", "w");
			fprintf(in->gnuplot_time,
				"set terminal x11 title 'Organic Photovoltaic Device Model - www.opvdm.com' \n");

			fflush(in->gnuplot_time);
		}
	}
}

void plot_now(struct device *in, char *name)
{
	struct timeval mytime;
	struct timeval result;
	gettimeofday(&mytime, NULL);

	timersub(&mytime, &last_time, &result);
	double diff = result.tv_sec + result.tv_usec / 1000000.0;

	if (diff < 1e-1) {
		return;
	}

	last_time.tv_sec = mytime.tv_sec;
	last_time.tv_usec = mytime.tv_usec;

	if (get_dump_status(dump_plot) == TRUE) {
		fprintf(in->gnuplot, "load '%s%s'\n", plot_script_dir, name);
		fflush(in->gnuplot);
	}
}

void plot_now_excite(struct device *in)
{
	if (get_dump_status(dump_plot) == TRUE) {

		fprintf(in->gnuplot, "load 'plot_excite'\n");
		fflush(in->gnuplot);

	}
}

void plot_replot(struct device *in)
{
	if (get_dump_status(dump_plot) == TRUE) {

		fprintf(in->gnuplot, "replot\n");
		fflush(in->gnuplot);

		if (in->plottime == TRUE) {
			fprintf(in->gnuplot_time, "replot\n");
			fflush(in->gnuplot_time);
		}
	}
}

void plot_close(struct device *in)
{
	if (get_dump_status(dump_plot) == TRUE) {

		fprintf(in->gnuplot, "exit\n");
		fflush(in->gnuplot_time);
		pclose(in->gnuplot);

		if (in->plottime == TRUE) {
			fprintf(in->gnuplot_time, "quit\n");
			fflush(in->gnuplot_time);
		}
	}
}
