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
#include <math.h>
#include <umfpack.h>
#include "sim.h"

static int last_col = 0;
static int last_nz = 0;
static double *x = NULL;
static int *Ap = NULL;
static int *Ai = NULL;
static double *Ax = NULL;
static int solver_dump_every_matrix = 0;

static double total_time = 0.0;

void set_solver_dump_every_matrix(int dump)
{
	solver_dump_every_matrix = dump;
}

void error_report(int status, const char *file, const char *func, int line)
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

void solver_precon(int col, int nz, int *Ti, int *Tj, double *Tx, double *b)
{
	int i;
	int ii;

	for (i = 0; i < nz; i++) {
		if (Ti[i] == Tj[i]) {
			if (Tx[i] != 0.0) {
				b[Ti[i]] /= Tx[Ti[i]];
				for (ii = 0; ii < nz; ii++) {
					if (Ti[ii] == Ti[i]) {
						Tx[ii] /= Tx[Ti[i]];
						//printf("%e % ");
					}
				}
				//getchar();
			}
		}
	}

}

void solver_dump_matrix(int col, int nz, int *Ti, int *Tj, double *Tx,
			double *b, char *index)
{
	FILE *matrix;
	char name[100];
	sprintf(name, "matrix%s.dat", index);
	matrix = fopen(name, "w");
//fprintf(matrix,"%d\n",nz);
//fprintf(matrix,"%d\n",col);

	int i;
	for (i = 0; i < nz; i++) {
		fprintf(matrix, "%d %d %le\n", Tj[i], Ti[i], fabs(Tx[i]));
	}

	for (i = 0; i < col; i++) {
		fprintf(matrix, "%d %d %le\n", 0, i, fabs(b[i]));
	}

	printf("Matrix dumped\n");

	fclose(matrix);
}

void solver_free()
{
	free(x);
	free(Ap);
	free(Ai);
	free(Ax);
	x = NULL;
	Ap = NULL;
	Ai = NULL;
	Ax = NULL;
	last_col = 0;
	last_nz = 0;
}

int solver(int col, int nz, int *Ti, int *Tj, double *Tx, double *b)
{
	if (solver_dump_every_matrix > 0) {
		char name[100];
		sprintf(name, "%d", solver_dump_every_matrix);
		printf("Dumping matrix %d\n", solver_dump_every_matrix);
		solver_dump_matrix(col, nz, Ti, Tj, Tx, b, name);
		solver_dump_every_matrix++;
	}
//getchar();
//int i;
	double stats[2];
	umfpack_tic(stats);
	void *Symbolic, *Numeric;
	int status;
//printf("here1\n");
	if ((last_col != col) || (last_nz != nz)) {
		x = realloc(x, col * sizeof(double));
		Ap = realloc(Ap, (col + 1) * sizeof(int));
		Ai = realloc(Ai, (nz) * sizeof(int));
		Ax = realloc(Ax, (nz) * sizeof(double));
		last_col = col;
		last_nz = nz;
	}

	double Control[UMFPACK_CONTROL], Info[UMFPACK_INFO];

	umfpack_di_defaults(Control);
//Control[UMFPACK_BLOCK_SIZE]=1000;
//Control [UMFPACK_STRATEGY]=UMFPACK_STRATEGY_SYMMETRIC;//UMFPACK_STRATEGY_UNSYMMETRIC;
//Control [UMFPACK_ORDERING]=UMFPACK_ORDERING_BEST;//UMFPACK_ORDERING_AMD;
//printf("%lf\n",Control[UMFPACK_BLOCK_SIZE]);
//Control [UMFPACK_PIVOT_TOLERANCE]=0.001;
	status =
	    umfpack_di_triplet_to_col(col, col, nz, Ti, Tj, Tx, Ap, Ai, Ax,
				      NULL);

	if (status != UMFPACK_OK) {
		error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}
// symbolic analysis
//printf("here2 %d\n",col);
	status =
	    umfpack_di_symbolic(col, col, Ap, Ai, Ax, &Symbolic, Control, Info);
//printf("here3\n");

	if (status != UMFPACK_OK) {
		error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}
// LU factorization
	umfpack_di_numeric(Ap, Ai, Ax, Symbolic, &Numeric, Control, Info);

	if (status != UMFPACK_OK) {
		error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}
// solve system

	umfpack_di_free_symbolic(&Symbolic);

	umfpack_di_solve(UMFPACK_A, Ap, Ai, Ax, x, b, Numeric, Control, Info);

//printf("%lf\n",Info [UMFPACK_ORDERING_USED]);

	if (status != UMFPACK_OK) {
		error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}

	umfpack_di_free_numeric(&Numeric);

	memcpy(b, x, col * sizeof(double));
	umfpack_toc(stats);
	total_time += stats[0];
	return 0;
}

void solver_print_time()
{
	printf("Time in umfpack %lf\n", total_time);
}
