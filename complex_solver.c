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
#include "umfpack.h"
#include "sim.h"
#include "complex_solver.h"
#include <math.h>

static int last_col = 0;
static int last_nz = 0;
static double *x = NULL;
static double *xz = NULL;
static int *Ap = NULL;
static int *Ai = NULL;
static double *Ax = NULL;
static double *Az = NULL;

void complex_error_report(int status, const char *file, const char *func,
			  int line)
{
	fprintf(stderr, "in %s: file %s, line %d: ", func, file, line);

	switch (status) {
	case UMFPACK_ERROR_out_of_memory:
		fprintf(stderr, "out of memory!\n");
		break;
	case UMFPACK_WARNING_singular_matrix:
		fprintf(stderr, "matrix is singular!\n");
		break;
	default:
		fprintf(stderr, "UMFPACK error code %d\n", status);
	}
}

void complex_solver_dump_matrix(int col, int nz, int *Ti, int *Tj, double *Tx,
				double *Txz, double *b, double *bz)
{
	FILE *matrix;
	matrix = fopen("./matrix.dat", "w");

	int i;
	for (i = 0; i < nz; i++) {
		fprintf(matrix, "%d %d %le\n", Tj[i], Ti[i],
			pow(pow(Tx[i], 2.0) + pow(Txz[i], 2.0), 2.0));
	}

	for (i = 0; i < col; i++) {
		fprintf(matrix, "%d %d %le\n", 0, i,
			fabs(pow(b[i], 2.0) + pow(bz[i], 2.0)));
	}

	printf("Matrix dumped\n");

	fclose(matrix);
}

void complex_solver_free()
{
	free(x);
	free(xz);
	free(Ap);
	free(Ai);
	free(Ax);
	free(Az);
	x = NULL;
	xz = NULL;
	Ap = NULL;
	Ai = NULL;
	Ax = NULL;
	Az = NULL;
	last_col = 0;
	last_nz = 0;
}

void complex_solver_print(int col, int nz, int *Ti, int *Tj, double *Tx,
			  double *Txz, double *b, double *bz)
{
	int i;
	for (i = 0; i < nz; i++) {
		printf("%d %d %le+i%le\n", Ti[i], Tj[i], Tx[i], Txz[i]);
	}

	for (i = 0; i < col; i++) {
		printf("%le+i%le\n", b[i], bz[i]);
	}

}

int complex_solver(int col, int nz, int *Ti, int *Tj, double *Tx, double *Txz,
		   double *b, double *bz)
{

	int i;
	void *Symbolic, *Numeric;
	int status;

	if ((last_col != col) || (last_nz != nz)) {
		x = realloc(x, col * sizeof(double));
		xz = realloc(xz, col * sizeof(double));
		Ap = realloc(Ap, (col + 1) * sizeof(int));
		Ai = realloc(Ai, (nz) * sizeof(int));
		Ax = realloc(Ax, (nz) * sizeof(double));
		Az = realloc(Az, (nz) * sizeof(double));
		last_col = col;
		last_nz = nz;
	}

	double Info[UMFPACK_INFO], Control[UMFPACK_CONTROL];

	umfpack_zi_defaults(Control);

	Control[UMFPACK_PRL] = 1;

	Control[UMFPACK_PRL] = 0;

	umfpack_zi_report_control(Control);

	status =
	    umfpack_zi_triplet_to_col(col, col, nz, Ti, Tj, Tx, Txz, Ap, Ai, Ax,
				      Az, NULL);

	if (status != UMFPACK_OK) {
		error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}

	status =
	    umfpack_zi_symbolic(col, col, Ap, Ai, Ax, Az, &Symbolic, Control,
				Info);
	umfpack_zi_report_status(Control, status);

	if (status != UMFPACK_OK) {
		error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}

	umfpack_zi_numeric(Ap, Ai, Ax, Az, Symbolic, &Numeric, Control, Info);

	if (status != UMFPACK_OK) {
		error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}

	status =
	    umfpack_zi_solve(UMFPACK_A, Ap, Ai, Ax, Az, x, xz, b, bz, Numeric,
			     Control, Info);
	if (status != UMFPACK_OK) {
		error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}

	(void)umfpack_zi_report_vector(col, x, xz, Control);

	umfpack_zi_free_symbolic(&Symbolic);
	umfpack_di_free_numeric(&Numeric);

	for (i = 0; i < col; i++) {
		b[i] = x[i];
		bz[i] = xz[i];
	}

	return 0;
}
