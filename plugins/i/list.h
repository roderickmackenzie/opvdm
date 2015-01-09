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
#ifndef list_h
#define list_h

struct list {
	struct vec *list;
	int max;
	int length;
	double cog_x;
	double cog_y;
	double max_y;
	double min_y;
};

void list_load(struct list *in, char *file_name);
int list_check(struct list *in, struct vec *test);
void list_dump(char *file_name, struct list *in);
void list_init(struct list *in);
void list_add_no_rep(struct list *in, struct vec *test);
void list_add(struct list *in, double one, double two);
int list_get_length(struct list *in);
void list_free(struct list *in);
void list_dump_2d(char *file_name, struct list *in);
void list_cog_cal(struct list *in);
void list_minmax_cal(struct list *in);
void list_remove_bump_up(struct list *in, int start);
void list_remove_bump_down(struct list *in, int start);
#endif
