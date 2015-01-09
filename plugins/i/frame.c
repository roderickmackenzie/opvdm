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
#include "list.h"
#include "vec.h"
#include "frame.h"

void frame_free(struct map *in)
{
	int i;

	for (i = 0; i < in->xpoints; i++) {
		free(in->data[i]);
	}
	free(in->data);

	free(in->x);
	free(in->y);
	free(in->x_fake);
	free(in->y_fake);

}

void frame_init(struct map *in, int xpoints, int ypoints, double xstart,
		double xstop, double ystart, double ystop)
{

	int x;
	int y;
	double pos = 0.0;
	in->xpoints = xpoints;
	in->ypoints = ypoints;
	in->xstart = xstart;
	in->xstop = xstop;
	in->ystart = ystart;
	in->ystop = ystop;
	in->count = 0;

	in->data = (double **)malloc(in->xpoints * sizeof(double *));
	for (x = 0; x < in->xpoints; x++) {
		in->data[x] = (double *)malloc(in->ypoints * sizeof(double));
	}

	for (x = 0; x < in->xpoints; x++) {
		for (y = 0; y < in->ypoints; y++) {
			in->data[x][y] = 0.0;
		}
	}

	pos = in->xstart;
	in->xdelta = (in->xstop - in->xstart) / ((double)in->xpoints);
	in->x = (double *)malloc(in->xpoints * sizeof(double));
	in->x_fake = (double *)malloc(in->xpoints * sizeof(double));
	for (x = 0; x < in->xpoints; x++) {
		in->x[x] = pos;
		in->x_fake[x] = pos;
		pos += in->xdelta;
	}

	pos = in->ystart;
	in->ydelta = (in->ystop - in->ystart) / ((double)in->ypoints);
	in->y = (double *)malloc(in->ypoints * sizeof(double));
	in->y_fake = (double *)malloc(in->ypoints * sizeof(double));
	for (y = 0; y < in->ypoints; y++) {
		in->y[y] = pos;
		in->y_fake[y] = pos;
		pos += in->ydelta;
	}
}

void frame_reset(struct map *in)
{
	int x;
	int y;
	for (x = 0; x < in->xpoints; x++) {
		for (y = 0; y < in->ypoints; y++) {
			in->data[x][y] = 0.0;
		}
	}
}

void frame_data_set(struct map *in, double x, double y, double data)
{
	int xpos = (x - in->xstart) / in->xdelta;
	int ypos = (y - in->ystart) / in->ydelta;
	if (xpos < 0)
		xpos = 0;
	if (ypos < 0)
		ypos = 0;
	if (xpos >= in->xpoints)
		xpos = in->xpoints - 1;
	if (ypos >= in->ypoints)
		ypos = in->ypoints - 1;

	in->data[xpos][ypos] = data;

}

void frame_data_add(struct map *in, double x, double y, double data)
{
	int xpos = (x - in->xstart) / in->xdelta;
	int ypos = (y - in->ystart) / in->ydelta;
	if (xpos < 0)
		xpos = 0;
	if (ypos < 0)
		ypos = 0;
	if (xpos >= in->xpoints)
		xpos = in->xpoints - 1;
	if (ypos >= in->ypoints)
		ypos = in->ypoints - 1;

	in->data[xpos][ypos] += data;

}

void frame_data_set_if_bigger(struct map *in, double x, double y, double data)
{
	int xpos = (x - in->xstart) / in->xdelta;
	int ypos = (y - in->ystart) / in->ydelta;

	if (xpos < 0)
		xpos = 0;
	if (ypos < 0)
		ypos = 0;
	if (xpos >= in->xpoints)
		xpos = in->xpoints - 1;
	if (ypos >= in->ypoints)
		ypos = in->ypoints - 1;

	if (in->data[xpos][ypos] < data)
		in->data[xpos][ypos] = data;
}

void frame_scale_delog_scale(struct map *in)
{
	int x = 0;
	int y = 0;

	for (x = 0; x < in->xpoints; x++) {
		in->x_fake[x] = pow(10, in->x_fake[x]);
	}

	for (y = 0; y < in->ypoints; y++) {
		in->y_fake[y] = pow(10, in->y_fake[y]);
	}

}

void frame_dump(char *file, struct map *in)
{
	int x = 0;
	int y = 0;
	double ret;

	FILE *out;
	out = fopen(file, "w");
	for (x = 0; x < in->xpoints; x++) {
		for (y = 0; y < in->ypoints; y++) {
			ret = in->data[x][y];
			if (ret != 0.0)
				fprintf(out, "%le %le %le\n", in->x_fake[x],
					in->y_fake[y], ret);

		}
		fprintf(out, "\n");

	}
	fclose(out);
}

void frame_dump_outline(char *file, struct map *in)
{
	int x = 0;
	int y = 0;
	double ret = 0.0;
	struct list l;
	struct vec v;
	list_init(&l);

	for (x = 0; x < in->xpoints; x++) {
		for (y = in->ypoints - 1; y >= 0; y--) {
			ret = in->data[x][y];
			if (ret != 0.0) {
				set_vec(&v, in->x[x], in->y[y], 0.0);
				list_add_no_rep(&l, &v);
				break;
			}
		}

	}

	for (x = in->xpoints - 1; x >= 0; x--) {
		for (y = 0; y < in->ypoints; y++) {
			ret = in->data[x][y];
			if (ret != 0.0) {
				set_vec(&v, in->x[x], in->y[y], 0.0);
				list_add_no_rep(&l, &v);
				break;
			}
		}

	}

	list_cog_cal(&l);
	list_minmax_cal(&l);
	list_free(&l);
	list_init(&l);
	int count;
	for (x = 0; x < in->xpoints; x++) {
		count = 0.0;
		for (y = in->ypoints - 1; y >= 0; y--) {
			if (in->data[x][y] != 0.0)
				count++;
		}
		for (y = in->ypoints - 1; y >= 0; y--) {
			ret = in->data[x][y];
			if (ret != 0.0) {
				if (count < 2) {
					if (fabs(l.min_y - in->y[y]) >
					    fabs(l.max_y - in->y[y])) {
						set_vec(&v, in->x_fake[x],
							in->y_fake[y], 0.0);
						list_add_no_rep(&l, &v);
					}
				} else {
					set_vec(&v, in->x_fake[x],
						in->y_fake[y], 0.0);
					list_add_no_rep(&l, &v);
				}
				break;
			}
		}

	}

	for (x = 0; x < 20; x++) {
		list_remove_bump_down(&l, 0);
	}
	int pos = l.length;
	for (x = in->xpoints - 1; x >= 0; x--) {
		count = 0.0;
		for (y = 0; y < in->ypoints; y++) {
			if (in->data[x][y] != 0.0)
				count++;
		}

		for (y = 0; y < in->ypoints; y++) {
			ret = in->data[x][y];
			if (ret != 0.0) {
				if (count < 2) {
					if (fabs(l.min_y - in->y[y]) <
					    fabs(l.max_y - in->y[y])) {
						set_vec(&v, in->x_fake[x],
							in->y_fake[y], 0.0);
						list_add_no_rep(&l, &v);
					}
				} else {
					set_vec(&v, in->x_fake[x],
						in->y_fake[y], 0.0);
					list_add_no_rep(&l, &v);
				}
				break;
			}
		}

	}

	for (x = 0; x < 20; x++) {
		list_remove_bump_up(&l, pos);
	}

	list_add(&l, l.list[0].x, l.list[0].y);

	list_dump_2d(file, &l);
	list_free(&l);
}

void frame_cog_cal(struct map *in)
{
	int x;
	int y;
	double x_tot = 0.0;
	double y_tot = 0.0;

	for (x = 0; x < in->xpoints; x++) {
		for (y = 0; y < in->ypoints; y++) {
			x_tot += in->data[x][y] * in->x[x];
			y_tot += in->data[x][y] * in->y[x];
		}
	}
	in->cog_x = x_tot / ((double)in->xpoints);
	in->cog_y = y_tot / ((double)in->ypoints);
}

void frame_fill(struct map *in)
{
	int x;
	int y;

	for (x = 0; x < in->xpoints; x++) {
		for (y = 0; y < in->ypoints; y++) {
			if ((x != 0) && (x != in->xpoints - 1)) {
				if ((in->data[x - 1][y] == 1)
				    && (in->data[x + 1][y] == 1))
					in->data[x][y] = 1.0;
			}

		}
	}
}
