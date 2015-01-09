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
#ifndef i_h
#define i_h

struct istruct {
	double *x;
	double **dd;
	double *data;

	int m;
	int len;
	int max_len;
	char name[200];
};

void inter_add_to_hist(struct istruct *in, double pos, double value);
void inter_init_mesh(struct istruct *in, int len, double min, double max);
void inter_smooth_range(struct istruct *in, int points, double x);
double inter_avg_range(struct istruct *in, double start, double stop);
double inter_array_get_max(double *data, int len);
void inter_div(struct istruct *one, struct istruct *two);
void inter_div_double(struct istruct *in, double div);
double inter_get_min_range(struct istruct *in, double min, double max);
void inter_make_cumulative(struct istruct *in);
void inter_y_mul_dx(struct istruct *in);
void inter_add_x(struct istruct *in, double value);
void inter_sort(struct istruct *in);
double inter_get_quartile(struct istruct *in, double value);
void inter_save_seg(struct istruct *in, char *path, char *name, int seg);
double inter_intergrate(struct istruct *in);
void inter_to_log_mesh(struct istruct *out, struct istruct *in);
void inter_smooth(struct istruct *in, int points);
double inter_sum_mod(struct istruct *in);
void inter_set_value(struct istruct *in, double value);
double inter_get_neg(struct istruct *in, double x);
double inter_get_noend(struct istruct *in, double x);
void inter_new(struct istruct *in, int len);
void inter_to_new_mesh(struct istruct *in, struct istruct *out);
void inter_swap(struct istruct *in);
void inter_log_y_m(struct istruct *in);
double inter_get_min(struct istruct *in);
double inter_get_fabs_max(struct istruct *in);
double inter_norm_to_one_range(struct istruct *in, double min, double max);
void inter_chop(struct istruct *in, double min, double max);
void inter_save_a(struct istruct *in, char *path, char *name);
void inter_dump(struct istruct *in);
void inter_purge_zero(struct istruct *in);
void inter_append(struct istruct *in, double x, double y);
void inter_init(struct istruct *in);
void inter_sub_double(struct istruct *in, double value);
void inter_sub(struct istruct *one, struct istruct *two);
double inter_sum(struct istruct *in);
void inter_copy(struct istruct *in, struct istruct *orig, int alloc);
int inter_get_col(char *file);
void inter_load_a(struct istruct *in, char *name, int col, int ncol);
double inter_get_diff(char *out_path, struct istruct *one, struct istruct *two,
		      double start, double stop, double error_mul);
void inter_pow(struct istruct *in, double p);
double inter_get_raw(double *x, double *data, int len, double pos);
double inter_norm_to_one(struct istruct *in);
void inter_log_y(struct istruct *in);
void inter_mul(struct istruct *in, double mul);
void inter_log_x(struct istruct *in);
void inter_save(struct istruct *in, char *name);
void inter_load(struct istruct *in, char *name, int len, int col, int ncol);
double inter_get_hard(struct istruct *in, double x);
double inter_get(struct istruct *in, double x);
void inter_print(struct istruct *in);
void inter_free(struct istruct *in);
void inter_rescale(struct istruct *in, double xmul, double ymul);
void inter_mod(struct istruct *in);
void inter_add(struct istruct *out, struct istruct *in);
void inter_norm(struct istruct *in, double mul);
double inter_get_max(struct istruct *in);
void inter_add_double(struct istruct *in, double value);
double inter_intergrate_lim(struct istruct *in, double from, double to);
void inter_deriv(struct istruct *out, struct istruct *in);
void inter_import_array(struct istruct *in, double *x, double *y, int len,
			int alloc);
double inter_avg(struct istruct *in);
void inter_convolve(struct istruct *one, struct istruct *two);
void inter_save_backup(struct istruct *in, char *name, int backup);
void inter_dft(double *real, double *imag, struct istruct *in, double fx);
#endif
