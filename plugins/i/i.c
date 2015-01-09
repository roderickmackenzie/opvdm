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

#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "i.h"
#include "true_false.h"

static int unused __attribute__ ((unused));
static char *unused_pchar __attribute__ ((unused));

void inter_add_to_hist(struct istruct *in, double pos, double value)
{
	int ii = 0;
	double min = in->x[0];
	double max = in->x[in->len - 1];
	double dx = (max - min) / ((double)in->len);

	ii = (int)((pos - min) / dx);

	if (ii < in->len) {
		if (ii >= 0) {
			in->data[ii] += value;
		}
	}

}

double inter_get_center_of_peak(struct istruct *in, int i, int window)
{
	int delta = window / 2;

	if ((i - delta < 0) || (i + delta > in->len - 1)) {
		return in->x[i];
	}
	double top = 0.0;
	double btm = 0.0;
	int ii;
	for (ii = i - delta; ii < i + delta; ii++) {
		top += in->x[ii] * in->data[ii];
		btm += in->data[ii];
	}
	return top / btm;
}

void inter_find_peaks(struct istruct *out, struct istruct *in, int find_max)
{
	int i = 0;
	int ii = 0;
	int window = 2;
	double yn = 0.0;
	double yc = 0.0;
	double yp = 0.0;
	int min = 0;
	int max = 0;
	int grad_l = 0;
	int grad_r = 0;
	for (i = 0; i < in->len; i++) {
		for (ii = 1; ii <= window; ii++) {
			if ((i - ii) >= 0 && ((i - ii) < in->len))
				yn = in->data[i - ii];

			if ((i + ii) >= 0 && ((i + ii) < in->len))
				yp = in->data[i + ii];

			yc = in->data[i];

			if ((yc - yn) < 0.0) {
				grad_l = -1;
			} else {
				grad_l = 1;
			}

			if ((yp - yc) < 0.0) {
				grad_r = -1;
			} else {
				grad_r = 1;
			}

			if ((grad_l == -1) && (grad_r == 1)) {
				min++;
			} else if ((grad_l == 1) && (grad_r == -1)) {
				max++;
			}
		}

		if (min == window) {
			if (find_max == FALSE)
				inter_append(out,
					     inter_get_center_of_peak(in, i,
								      20),
					     in->data[i]);

		} else if (max == window) {
			if (find_max == TRUE)
				inter_append(out,
					     inter_get_center_of_peak(in, i,
								      20),
					     in->data[i]);
		}
		min = 0;
		max = 0;
	}
}

void inter_dft(double *real, double *imag, struct istruct *in, double fx)
{
	double r = 0.0;
	double i = 0.0;
	int j = 0;
	double dt = in->x[1] - in->x[0];
	double len = (double)in->len;
	double n = (len) * fx * dt;

	for (j = 0; j < in->len; j++) {
		r += in->data[j] * cos(2.0 * 3.1415926 * ((double)j) * n /
				       (len));
		i += in->data[j] * sin(2.0 * 3.1415926 * ((double)j) * n /
				       (len));
	}
	*real = r;
	*imag = i;
}

int inter_sort_compare(const void *a, const void *b)
{
	double aa = *(double *)a;
	double bb = *(double *)b;

	if (aa < bb)
		return -1;
	if (aa > bb)
		return 1;

	return 0;
}

void inter_sort(struct istruct *in)
{

	int i = 0;
	double *data = (double *)malloc(in->len * 2 * sizeof(double));
	for (i = 0; i < in->len; i++) {
		data[i * 2] = in->x[i];
		data[(i * 2) + 1] = in->data[i];
	}
	qsort(data, in->len, sizeof(double) * 2, inter_sort_compare);

	for (i = 0; i < in->len; i++) {
		in->x[i] = data[i * 2];
		in->data[i] = data[(i * 2) + 1];
	}

	free(data);
}

int search(double *x, int N, double find)
{
	if (N == 1)
		return 0;
	int pos = N / 2;
	int step = N / 2;
	do {
		step = step / 2 + (step % 2 > 0 ? 1 : 0);

		if (x[pos] > find) {
			pos -= step;
		} else {
			pos += step;
		}

		if (pos <= 0) {
			pos = 0;
			break;
		}
		if (pos >= (N - 1)) {
			pos = N - 1;
			break;
		}
		if (step == 0)
			break;
		if (x[pos] == find)
			break;
		if ((x[pos] <= find) && ((x[pos + 1] > find)))
			break;

	} while (1);

	if (pos == (N - 1))
		pos = N - 2;

	return pos;
}

int get_file_len(char *file_name)
{
	FILE *file;
	if (!(file = fopen(file_name, "r"))) {
		printf("Error opening file %s\n", file_name);
		exit(0);
	}
	char buffer[1000];

	int i;
	i = 0;
	char *p;
	do {
		buffer[0] = 0;
		unused_pchar = fgets(buffer, 1000, file);
		p = buffer;
		while (*p == ' ' || *p == '\t')
			p++;
		if ((*p == '\r') || (*p == '\n') || (*p == 0))
			i--;
		i++;
	} while (!feof(file));

	fclose(file);
	return i;
}

double inter_get_quartile(struct istruct *in, double value)
{
	int i;
	double sum = 0.0;
	double sum2 = 0.0;
	double dl = 0.0;
	double dr = 0.0;
	double dx = 0.0;
	for (i = 0; i < in->len; i++) {
		if (i == 0) {
			dl = in->x[0];
		} else {
			dl = in->x[i - 1];
		}

		if (i == in->len - 1) {
			dr = in->x[in->len - 1];
		} else {
			dr = in->x[i + 1];
		}

		dx = dr - dl;
		sum += dx * in->data[i];
	}

	for (i = 0; i < in->len; i++) {
		if (i == 0) {
			dl = in->x[0];
		} else {
			dl = in->x[i - 1];
		}

		if (i == in->len - 1) {
			dr = in->x[in->len - 1];
		} else {
			dr = in->x[i + 1];
		}

		dx = dr - dl;
		sum2 += dx * in->data[i];
		if (sum2 > (sum * value))
			return in->x[i];
	}

	return 0.0;
}

void inter_add(struct istruct *out, struct istruct *in)
{
	int i;
	for (i = 0; i < out->len; i++) {
		out->data[i] += in->data[i];
	}

}

void inter_alloc_dd(struct istruct *in, int m, int len)
{
	int i = 0;
	in->m = m;
	in->dd = (double **)malloc(in->m * sizeof(double));

	for (i = 0; i < in->m; i++) {
		in->dd[i] = (double *)malloc(len * sizeof(double));
	}

	in->data = in->dd[0];
}

void inter_to_log_mesh(struct istruct *out, struct istruct *in)
{
	double a = log10(in->x[0]);
	double b = log10(in->x[in->len - 1]);
	double step = (b - a) / ((double)out->len);
	int i;
	double pos = a;
	for (i = 0; i < out->len; i++) {
		out->x[i] = pow(10.0, pos);
		out->data[i] = inter_get(in, pow(10.0, pos));
		pos += step;

	}

}

void inter_to_new_mesh(struct istruct *in, struct istruct *out)
{
	int i;
	int ii;
	double pos = 0.0;
	double delta = (in->x[in->len - 1] - in->x[0]) / (double)out->len;
	pos += delta / 2.0;
	for (i = 0; i < out->len; i++) {
		ii = search(in->x, in->len, pos);

		double x0 = in->x[ii];
		double x1 = in->x[ii + 1];

		double y0 = in->data[ii];
		double y1 = in->data[ii + 1];

		out->x[i] = pos;
		out->data[i] = y0 + ((y1 - y0) / (x1 - x0)) * (pos - x0);

		pos += delta;
	}

	return;
}

void inter_realloc_dd(struct istruct *in, int len)
{
	int m;
	for (m = 0; m < in->m; m++) {
		in->dd[m] = (double *)realloc(in->dd[m], len * sizeof(double));
	}

	in->data = in->dd[0];
}

void inter_new(struct istruct *in, int len)
{
	int i;
	in->m = 1;
	strcpy(in->name, "new");

	in->len = len;

	in->x = (double *)malloc(in->len * sizeof(double));

	inter_alloc_dd(in, in->m, in->len);

	for (i = 0; i < in->len; i++) {
		in->data[i] = 0.0;
	}

}

double inter_sum_mod(struct istruct *in)
{
	int i;
	double sum = 0.0;

	for (i = 0; i < in->len; i++) {
		sum += fabs(in->data[i]);
	}
	return sum;
}

double inter_avg_range(struct istruct *in, double start, double stop)
{
	int i;
	double sum = 0.0;
	double points = 0.0;
	for (i = 0; i < in->len; i++) {
		if ((in->x[i] > start) && (in->x[i] < stop)) {
			sum += in->data[i];
			points += 1.0;
		}
	}
	return sum / points;
}

double inter_avg(struct istruct *in)
{
	int i;
	double sum = 0.0;

	for (i = 0; i < in->len; i++) {
		sum += in->data[i];
	}
	return sum / ((double)(in->len));
}

double inter_sum(struct istruct *in)
{
	int i;
	double sum = 0.0;

	for (i = 0; i < in->len; i++) {
		sum += in->data[i];
	}
	return sum;
}

void inter_convolve(struct istruct *one, struct istruct *two)
{
	int i;

	for (i = 0; i < one->len; i++) {
		one->data[i] *= two->data[i];
	}
}

void inter_mul(struct istruct *in, double mul)
{
	int i;

	for (i = 0; i < in->len; i++) {
		in->data[i] *= mul;
	}
}

double inter_get_diff(char *out_path, struct istruct *one, struct istruct *two,
		      double start, double stop, double error_mul)
{
	FILE *out;
	if (out_path != NULL)
		out = fopen(out_path, "w");
	double error = 0.0;
	if (one->x[0] > start)
		start = one->x[0];

	double points = 0.0;
	double dx = (stop - start) / 400;
	double pos = start;
	double etemp = 0.0;
	do {

		if (pos >= start) {
			etemp =
			    error_mul * fabs(inter_get(one, pos) -
					     inter_get(two, pos));
			if (out_path != NULL)
				fprintf(out, "%le %le\n", pos, etemp);
			error += etemp;
			points += 1.0;
		}

		pos += dx;
	} while (pos < stop);
	if (out_path != NULL)
		fclose(out);

	return error / points;
}

double inter_intergrate(struct istruct *in)
{
	int i;
	double tn = 0.0;
	double tp = 0.0;

	double dt = 0.0;

	double sum = 0.0;
	double n;

	for (i = 0; i < in->len; i++) {

		if (i == 0) {
			tn = in->x[i];
		} else {
			tn = in->x[i - 1];
		}

		if (i == in->len - 1) {
			tp = in->x[i];
		} else {
			tp = in->x[i + 1];
		}

		n = in->data[i];
		dt = fabs((tp - tn) / 2.0);

		sum += n * dt;

	}

	return sum;
}

double inter_intergrate_lim(struct istruct *in, double from, double to)
{
	int i;
	double tn = 0.0;
	double tp = 0.0;

	double dt = 0.0;

	double sum = 0.0;
	double n;

	for (i = 0; i < in->len; i++) {

		if (i == 0) {
			tn = in->x[i];
		} else {
			tn = in->x[i - 1];
		}

		if (i == in->len - 1) {
			tp = in->x[i];
		} else {
			tp = in->x[i + 1];
		}

		n = in->data[i];
		dt = fabs((tp - tn) / 2.0);

		if (tn > from)
			sum += n * dt;
		if (tn > to)
			break;

	}

	return sum;
}

double inter_norm_to_one_range(struct istruct *in, double start, double stop)
{
	int i;
	double max = 0.0;

	for (i = 0; i < in->len; i++) {
		if (in->x[i] > start) {
			max = in->data[i];
			break;
		}
	}

	for (i = 0; i < in->len; i++) {
		if ((in->x[i] > start) && (in->x[i] < stop)) {
			if (in->data[i] > max)
				max = in->data[i];
		}
	}

	for (i = 0; i < in->len; i++) {
		in->data[i] /= max;
	}

	return max;
}

double inter_get_max(struct istruct *in)
{
	int i;
	double max = in->data[0];

	for (i = 0; i < in->len; i++) {
		if (in->data[i] > max)
			max = in->data[i];
	}

	return max;
}

double inter_get_fabs_max(struct istruct *in)
{
	int i;
	double max = fabs(in->data[0]);

	for (i = 0; i < in->len; i++) {
		if (fabs(in->data[i]) > max)
			max = fabs(in->data[i]);
	}

	return max;
}

double inter_norm_to_one(struct istruct *in)
{
	int i;
	double max = in->data[0];

	for (i = 0; i < in->len; i++) {
		if (in->data[i] > max)
			max = in->data[i];
	}

	for (i = 0; i < in->len; i++) {
		in->data[i] /= max;
	}

	return max;
}

void inter_log_y_m(struct istruct *in)
{
	int i;
	double mull = 1.0;

	for (i = 0; i < in->len; i++) {
		mull = 1.0;
		if (in->data[i] < 0.0)
			mull = -1.0;
		in->data[i] = log10(sqrt(in->data[i] * in->data[i])) * mull;
	}
}

void inter_log_y(struct istruct *in)
{
	int i;
	for (i = 0; i < in->len; i++) {
		in->data[i] = log10(sqrt(in->data[i] * in->data[i]));
	}
}

void inter_log_x(struct istruct *in)
{
	int i;
	for (i = 0; i < in->len; i++) {
		if (in->x[i] > 0.0)
			in->x[i] = log10(sqrt(in->x[i] * in->x[i]));
	}
}

void inter_smooth_range(struct istruct *in, int points, double x)
{
	int i = 0;
	int ii = 0;
	int pos = 0;
	double tot_point = 0.0;
	double tot = 0;
	for (i = 0; i < in->len; i++) {
		for (ii = 0; ii < points; ii++) {

			pos = i + ii;

			if (pos < in->len) {
				tot += in->data[pos];
				tot_point += 1.0;
			}
		}

		if (in->x[i] > x) {

			in->data[i] = (tot / (double)tot_point);
		}
		tot = 0.0;
		tot_point = 0.0;
	}
}

void inter_smooth(struct istruct *in, int points)
{
	int i = 0;
	int ii = 0;
	int pos = 0;
	double tot_point = 0.0;
	double tot = 0;
	for (i = 0; i < in->len; i++) {
		for (ii = 0; ii < points; ii++) {

			pos = i + ii;

			if (pos < in->len) {
				tot += in->data[pos];
				tot_point += 1.0;
			}
		}

		in->data[i] = (tot / (double)tot_point);
		tot = 0.0;
		tot_point = 0.0;
	}
}

void inter_purge_zero(struct istruct *in)
{
	int i;
	int write = 0;
	int read = 0;
	for (i = 0; i < in->len; i++) {
		in->data[write] = in->data[read];
		in->x[write] = in->x[read];

		if (in->data[read] == 0.0) {
			write--;
		}
		read++;
		write++;
	}

	in->len = write;

	in->x = (double *)realloc(in->x, in->len * sizeof(double));
	inter_realloc_dd(in, in->len);

}

double inter_get_min(struct istruct *in)
{
	int i = 0;
	double min = in->data[i];
	for (i = 0; i < in->len; i++) {
		if (in->data[i] < min)
			min = in->data[i];

	}
	return min;
}

double inter_get_min_range(struct istruct *in, double min, double max)
{
	int i = 0;
	double ret = in->data[i];
	for (i = 0; i < in->len; i++) {
		if ((in->x[i] > min) && (in->x[i] < max)) {
			if (in->data[i] < ret)
				ret = in->data[i];
		}

	}
	return ret;
}

void inter_chop(struct istruct *in, double min, double max)
{
	int i;
	int write = 0;
	int read = 0;
	for (i = 0; i < in->len; i++) {
		in->data[write] = in->data[read];
		in->x[write] = in->x[read];
		write++;
		if (in->x[read] < min) {
			write--;
		}

		if (in->x[read] > max)
			break;
		read++;

	}
	in->len = write;

	in->x = (double *)realloc(in->x, in->len * sizeof(double));
	inter_realloc_dd(in, in->len);
}

void inter_div_double(struct istruct *in, double div)
{
	int i;
	for (i = 0; i < in->len; i++) {
		in->data[i] /= div;
	}

}

void inter_rescale(struct istruct *in, double xmul, double ymul)
{
	int i;
	for (i = 0; i < in->len; i++) {
		in->x[i] *= xmul;
		in->data[i] *= ymul;
	}

}

void inter_mod(struct istruct *in)
{
	int i;
	for (i = 0; i < in->len; i++) {
		if (in->data[i] < 0.0)
			in->data[i] *= -1.0;
	}

}

void inter_pow(struct istruct *in, double p)
{
	int i;
	for (i = 0; i < in->len; i++) {
		in->data[i] = pow(in->data[i], p);
	}

}

void inter_add_x(struct istruct *in, double value)
{
	int i;
	for (i = 0; i < in->len; i++) {
		in->x[i] += value;
	}

}

void inter_sub_double(struct istruct *in, double value)
{
	int i;
	for (i = 0; i < in->len; i++) {
		in->data[i] -= value;
	}

}

void inter_div(struct istruct *one, struct istruct *two)
{
	if (one->len != two->len) {
		printf("The arrays are not the same length\n");
		exit(0);
	}

	int i;
	for (i = 0; i < one->len; i++) {
		if (one->x[i] != two->x[i]) {
			printf("The arrays do not have the same x axis\n");
			exit(0);
		}
		if (two->data[i] != 0)
			one->data[i] /= two->data[i];
	}

}

void inter_sub(struct istruct *one, struct istruct *two)
{
	if (one->len != two->len) {
		printf("The arrays are not the same length\n");
		exit(0);
	}

	int i;
	for (i = 0; i < one->len; i++) {
		if (one->x[i] != two->x[i]) {
			printf("The arrays do not have the same x axis\n");
			exit(0);
		}
		one->data[i] -= two->data[i];
	}

}

void inter_add_double(struct istruct *in, double value)
{
	int i;
	for (i = 0; i < in->len; i++) {
		in->data[i] += value;
	}

}

void inter_norm(struct istruct *in, double mul)
{
	int i;
	double tot = 0.0;
	double dx = 0.0;
	for (i = 0; i < in->len; i++) {
		if (i == 0) {
			dx = in->x[1] - in->x[0];
		} else if (i == in->len - 1) {
			dx = in->x[i] - in->x[in->len - 2];
		} else {
			dx = (in->x[i + 1] - in->x[i]) / 2.0 + (in->x[i] -
								in->x[i -
								      1]) / 2.0;
		}

		tot += dx * in->data[i];
	}

	for (i = 0; i < in->len; i++) {

		in->data[i] /= tot;
		in->data[i] *= mul;

	}

}

void inter_append(struct istruct *in, double x, double y)
{
	in->x[in->len] = x;
	in->data[in->len] = y;
	in->len++;

	if ((in->max_len - in->len) < 10) {
		in->max_len += 100;
		in->x = (double *)realloc(in->x, in->max_len * sizeof(double));
		inter_realloc_dd(in, in->max_len);
	}

}

void inter_init_mesh(struct istruct *in, int len, double min, double max)
{
	int i;
	in->m = 1;
	in->len = len;
	in->x = (double *)malloc(in->len * sizeof(double));
	inter_alloc_dd(in, in->m, in->len);
	memset(in->data, 0, in->len * sizeof(double));
	double pos = min;
	double dx = (max - min) / ((double)in->len);

	for (i = 0; i < in->len; i++) {
		in->x[i] = pos;
		pos += dx;
	}

}

void inter_init_m(struct istruct *in, int m)
{
	in->len = 0;
	in->max_len = 100;
	in->m = m;
	in->x = (double *)malloc(in->max_len * sizeof(double));

	inter_alloc_dd(in, m, in->max_len);
}

void inter_init(struct istruct *in)
{
	in->m = 1;
	inter_init_m(in, in->m);
}

void inter_load_a(struct istruct *in, char *name, int col, int ncol)
{
	in->m = 1;
	strcpy(in->name, name);
	int len = get_file_len(name);
	FILE *file;
	file = fopen(name, "r");
	if (file == NULL) {
		printf("inter_load_a can not open file %s\n", name);
		exit(0);
	}

	in->len = len;
	int i = 0;
	double out = 0.0;
	in->x = (double *)malloc(in->len * sizeof(double));

	inter_alloc_dd(in, in->m, in->len);
	int ii = 0;
	for (i = 0; i < in->len; i++) {
		unused = fscanf(file, "%lf", &(in->x[i]));
		for (ii = 0; ii < ncol; ii++) {
			unused = fscanf(file, "%lf", &(out));
			if (ii == col)
				in->data[i] = out;
		}

	}

	fclose(file);
}

void inter_copy(struct istruct *in, struct istruct *orig, int alloc)
{
	int i;
	in->m = orig->m;
	in->len = orig->len;

	if (alloc == TRUE) {
		in->x = (double *)malloc(orig->len * sizeof(double));
		inter_alloc_dd(in, in->m, orig->len);
	}

	for (i = 0; i < orig->len; i++) {
		in->x[i] = orig->x[i];
		in->data[i] = orig->data[i];
	}

}

void inter_import_array(struct istruct *in, double *x, double *y, int len,
			int alloc)
{
	int i;
	in->m = 1;
	in->len = len;

	if (alloc == TRUE) {
		in->x = (double *)malloc(in->len * sizeof(double));
		inter_alloc_dd(in, in->m, in->len);
	}

	for (i = 0; i < in->len; i++) {
		in->x[i] = x[i];
		in->data[i] = y[i];
	}

}

void inter_deriv(struct istruct *out, struct istruct *in)
{
	int i;
	double yl = 0.0;
	double yr = 0.0;
	double xl = 0.0;
	double xr = 0.0;
	double dy = 0.0;
	for (i = 0; i < in->len; i++) {
		if (i == 0) {
			xl = in->x[i];
			yl = in->data[i];
		} else {
			xl = in->x[i - 1];
			yl = in->data[i - 1];
		}

		if (i == (in->len - 1)) {
			xr = in->x[i];
			yr = in->data[i];
		} else {
			xr = in->x[i + 1];
			yr = in->data[i + 1];
		}

		dy = (yr - yl) / (xr - xl);
		out->x[i] = in->x[i];
		out->data[i] = dy;
	}

}

void inter_swap(struct istruct *in)
{
	int i;
	double *xtemp = malloc(sizeof(double) * in->len);
	double *dtemp = malloc(sizeof(double) * in->len);

	for (i = 0; i < in->len; i++) {
		dtemp[i] = in->data[i];
		xtemp[i] = in->x[i];
	}

	for (i = 0; i < in->len; i++) {
		in->data[i] = dtemp[in->len - 1 - i];
		in->x[i] = xtemp[in->len - 1 - i];
	}

	free(xtemp);
	free(dtemp);
}

void inter_load(struct istruct *in, char *name, int len, int col, int ncol)
{
	in->m = 1;
	FILE *file;
	file = fopen(name, "r");
	in->len = len;
	strcpy(in->name, name);
	int i = 0;
	double out = 0.0;
	in->x = (double *)malloc(in->len * sizeof(double));
	inter_alloc_dd(in, in->m, in->len);
	int ii = 0;
	for (i = 0; i < in->len; i++) {
		unused = fscanf(file, "%lf", &(in->x[i]));
		for (ii = 0; ii < ncol; ii++) {
			unused = fscanf(file, "%lf", &(out));
			if (ii == col)
				in->data[i] = out;
		}

	}

	fclose(file);
}

void inter_set_value(struct istruct *in, double value)
{
	int i = 0;
	int m = 0;
	for (i = 0; i < in->len; i++) {
		in->dd[m][i] = value;
	}

}

void inter_y_mul_dx(struct istruct *in)
{
	int i = 0;
	int m = 0;
	double dx = 0.0;
	double d0 = 0.0;
	double d1 = 0.0;
	for (i = 0; i < in->len; i++) {
		for (m = 0; m < in->m; m++) {
			if (i == 0) {
				d0 = (in->x[0]);
			} else {
				d0 = (in->x[i - 1]);
			}

			if (i == in->len - 1) {
				d1 = (in->x[i]);
			} else {
				d1 = (in->x[i + 1]);
			}

			dx = (d1 - d0) / 2.0;
			in->dd[m][i] = in->dd[m][i] * dx;
		}
	}

}

void inter_make_cumulative(struct istruct *in)
{
	int i = 0;
	double dx = 0.0;
	double d0 = 0.0;
	double d1 = 0.0;
	double tot = 0.0;
	for (i = 0; i < in->len; i++) {
		if (i == 0) {
			d0 = (in->x[0]);
		} else {
			d0 = (in->x[i - 1]);
		}

		if (i == in->len - 1) {
			d1 = (in->x[i]);
		} else {
			d1 = (in->x[i + 1]);
		}

		dx = (d1 - d0) / 2.0;
		tot += in->data[i] * dx;
		in->data[i] = tot;
	}

}

void inter_dump(struct istruct *in)
{
	int i = 0;
	int m = 0;
	for (i = 0; i < in->len; i++) {
		printf("%e", in->x[i]);
		for (m = 0; m < in->m; m++) {
			printf(" %e", in->dd[m][i]);
		}
		printf("\n");
	}

}

void inter_save_backup(struct istruct *in, char *name, int backup)
{
	char wholename[200];
	char backup_file[200];
	sprintf(wholename, "%s", name);
	if (backup == FALSE) {
		inter_save(in, wholename);
	} else {

		sprintf(backup_file, "%s.back", name);

		if (access(wholename, F_OK) != -1) {
			remove(backup_file);
			rename(wholename, backup_file);
		}
		inter_save(in, wholename);
	}

}

void inter_save_a(struct istruct *in, char *path, char *name)
{
	char wholename[200];
	sprintf(wholename, "%s/%s", path, name);
	inter_save(in, wholename);
}

void inter_save_seg(struct istruct *in, char *path, char *name, int seg)
{
	FILE *file = NULL;
	int i = 0;
	int m = 0;

	int count_max = in->len / seg;
	int count = 0;
	char temp[1000];
	int file_count = 0;
	for (i = 0; i < in->len; i++) {
		if (count == 0) {
			sprintf(temp, "%s/%s%d.dat", path, name, file_count);

			file = fopen(temp, "w");
			file_count++;
		}
		fprintf(file, "%e", in->x[i]);
		for (m = 0; m < in->m; m++) {
			fprintf(file, " %e", in->dd[m][i]);
		}
		count++;
		fprintf(file, "\n");

		if (count == count_max) {
			fclose(file);
			count = 0;
		}

	}
	if (count != 0)
		fclose(file);

}

void inter_save(struct istruct *in, char *name)
{
	FILE *file;
	file = fopen(name, "w");
	int i = 0;
	int m = 0;

	for (i = 0; i < in->len; i++) {
		fprintf(file, "%e", in->x[i]);
		for (m = 0; m < in->m; m++) {
			fprintf(file, " %e", in->dd[m][i]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

double inter_get_raw(double *x, double *data, int len, double pos)
{
	double x0;
	double x1;
	double y0;
	double y1;

	double ret;
	int i = 0;

	if (pos < x[0]) {

		return 0.0;
	}

	if (pos >= x[len - 1]) {
		i = len - 1;
		x0 = x[i - 1];
		x1 = x[i];

		y0 = data[i - 1];
		y1 = data[i];

	} else {
		i = search(x, len, pos);
		x0 = x[i];
		x1 = x[i + 1];

		y0 = data[i];
		y1 = data[i + 1];
	}
	ret = y0 + ((y1 - y0) / (x1 - x0)) * (pos - x0);
	return ret;
}

double inter_get(struct istruct *in, double x)
{
	double x0;
	double x1;
	double y0;
	double y1;

	double ret;
	int i = 0;

	if (x < in->x[0]) {

		return 0.0;
	}

	if (x >= in->x[in->len - 1]) {

		i = in->len - 1;
		x0 = in->x[i - 1];
		x1 = in->x[i];

		y0 = in->data[i - 1];
		y1 = in->data[i];

	} else {
		i = search(in->x, in->len, x);
		x0 = in->x[i];
		x1 = in->x[i + 1];

		y0 = in->data[i];
		y1 = in->data[i + 1];
	}
	ret = y0 + ((y1 - y0) / (x1 - x0)) * (x - x0);
	return ret;
}

double inter_get_hard(struct istruct *in, double x)
{

	if (x > in->x[in->len - 1]) {

		return 0.0;
	}

	return inter_get(in, x);
}

double inter_get_noend(struct istruct *in, double x)
{
	double x0;
	double x1;
	double y0;
	double y1;

	double ret;
	int i = 0;

	if (x < in->x[0]) {
		return in->data[0];
	}

	if (x >= in->x[in->len - 1]) {
		return in->data[in->len - 1];
	}

	if (x >= in->x[in->len - 1]) {

		i = in->len - 1;
		x0 = in->x[i - 1];
		x1 = in->x[i];

		y0 = in->data[i - 1];
		y1 = in->data[i];

	} else {
		i = search(in->x, in->len, x);
		x0 = in->x[i];
		x1 = in->x[i + 1];

		y0 = in->data[i];
		y1 = in->data[i + 1];
	}
	ret = y0 + ((y1 - y0) / (x1 - x0)) * (x - x0);
	return ret;
}

void inter_free(struct istruct *in)
{
	int m = 0;
	in->len = 0;
	in->max_len = 0;
	free(in->x);
	for (m = 0; m < in->m; m++) {
		free(in->dd[m]);
	}
	free(in->dd);
	in->m = 0;
}

double inter_array_get_max(double *data, int len)
{
	int i;
	double max = data[0];
	for (i = 0; i < len; i++) {
		if (max < data[i])
			max = data[i];
	}
	return max;
}
