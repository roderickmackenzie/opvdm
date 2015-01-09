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
#ifndef frame_h
#define frame_h
struct map {
	double **data;
	int xpoints;
	int ypoints;
	double xstart;
	double ystart;
	double xstop;
	double ystop;
	double xdelta;
	double ydelta;
	double *x;
	double *y;
	double *x_fake;
	double *y_fake;
	double cog_x;
	double cog_y;
	int count;
};

void frame_reset(struct map *in);
void frame_free(struct map *in);
void frame_init(struct map *in, int xpoints, int ypoints, double xstart,
		double xstop, double ystart, double ystop);
void frame_data_set(struct map *in, double x, double y, double data);
void frame_data_add(struct map *in, double x, double y, double data);
void frame_data_set_if_bigger(struct map *in, double x, double y, double data);
void frame_dump(char *file, struct map *in);
void frame_scale_delog_scale(struct map *in);
void frame_dump_outline(char *file, struct map *in);
void frame_cog_cal(struct map *in);
void frame_fill(struct map *in);
#endif
