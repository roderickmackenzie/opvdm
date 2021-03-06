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
#include <errno.h>
#include <util.h>
#include <true_false.h>
#include "../../dump_ctrl.h"
#include "../../complex_solver.h"
#include "../../const.h"
#include "../../light.h"

#include "../../device.h"
#include "../../light_interface.h"

#include "../../functions.h"

//static double min_light_error=1e-10;

EXPORT void light_dll_ver()
{
        printf("Full transfer matrix based light model\n");
}


EXPORT void light_dll_free(struct light *in)
{
l_light_free_memory(in);
}


EXPORT int light_dll_solve_lam_slice(struct light *in,int lam)
{
if (l_get_dump_status(dump_optics)==TRUE)
{
	char one[100];
	sprintf(one,"Solve light optical slice at %lf nm\n",in->l[lam]*1e9);
	//printf("%s\n",one);
	l_waveprint(one,in->l[lam]*1e9);
}
int i;

double complex Epl=0.0+0.0*I;
//double complex Epc=0.0+0.0*I;
//double complex Epr=0.0+0.0*I;

//double complex Enl=0.0+0.0*I;
//double complex Enc=0.0+0.0*I;
//double complex Enr=0.0+0.0*I;

double complex rc=0.0+0.0*I;
double complex tc=0.0+0.0*I;
double complex rr=0.0+0.0*I;
double complex tr=0.0+0.0*I;

double complex nbar_c=0.0;
double complex xi_c=0.0;

double complex nbar_r=0.0;
double complex xi_r=0.0;

double complex rl=0.0+0.0*I;
double complex tl=0.0+0.0*I;
double complex nbar_l=0.0;
double complex xi_l=0.0;

double complex fp=0.0;
double complex fn=0.0;

int ittr=0;
int pos=0;
//double error=1000;
double l=in->l[lam];

//double nc;

double tot_error=0.0;
int quit=FALSE;
double test=FALSE;
double dx=in->x[2]-in->x[1];
//for (i=0;i<in->points;i++)
//{
//printf("x=%le dx=%le\n",in->x[i],dx);
//getchar();
//}
do
{

	pos=0;

	for (i=0;i<in->points;i++)
	{

		rc=in->r[lam][i];
		tc=in->t[lam][i];



		if (i==0)
		{
			rl=in->r[lam][i];
			tl=in->t[lam][i];

			nbar_l=in->nbar[lam][i];
			Epl=in->sun_E[lam]+0.0*I;

		}else
		{
			rl=in->r[lam][i-1];
			tl=in->t[lam][i-1];

			nbar_l=in->nbar[lam][i-1];
			Epl=0.0;
		}

		if (i==in->points-1)
		{
			rr=in->r[lam][i];
			tr=in->t[lam][i];
			nbar_r=in->nbar[lam][i];
		}else
		{
			rr=in->r[lam][i+1];
			tr=in->t[lam][i+1];
			nbar_r=in->nbar[lam][i+1];
		}


		nbar_c=in->nbar[lam][i];

		xi_l=((2*PI)/l)*nbar_l;
		xi_c=((2*PI)/l)*nbar_c;
		xi_r=((2*PI)/l)*nbar_r;

		double complex pa=-tl;
		double complex pbp=cexp(xi_c*dx*I);

		double complex pbn=rl*cexp(-xi_c*dx*I);

		double complex na=-tc;//Enc
		double complex nbp=rc*cexp(xi_r*dx*I);//Enr*
		double complex nbn=cexp(-xi_r*dx*I);


		//getchar();
		fp=0.0+0.0*I;
		fn=0.0+0.0*I;

		if (i==0)
		{
			fp=Epl;
			pa=0.0+0.0*I;
		}else
		{
			fp=0.0+0.0*I;
		}




		if (i!=0)
		{
			in->Ti[pos]=i;
			in->Tj[pos]=i-1;
			in->Tx[pos]=creal(pa);
			in->Txz[pos]=cimag(pa);
			pos++;
		}

		in->Ti[pos]=i;
		in->Tj[pos]=i;
		in->Tx[pos]=creal(pbp);
		in->Txz[pos]=cimag(pbp);
		pos++;

		in->Ti[pos]=i;
		in->Tj[pos]=in->points+i;
		in->Tx[pos]=creal(pbn);
		in->Txz[pos]=cimag(pbn);
		pos++;


		if (i!=in->points-1)
		{
			in->Ti[pos]=in->points+i;
			in->Tj[pos]=i+1;
			in->Tx[pos]=creal(nbp);
			in->Txz[pos]=cimag(nbp);
			pos++;
		}

		in->Ti[pos]=in->points+i;
		in->Tj[pos]=in->points+i;
		in->Tx[pos]=creal(na);
		in->Txz[pos]=cimag(na);
		pos++;

		if (i!=in->points-1)
		{
			in->Ti[pos]=in->points+i;
			in->Tj[pos]=in->points+i+1;
			in->Tx[pos]=creal(nbn);
			in->Txz[pos]=cimag(nbn);
			pos++;
		}

		in->b[i]=creal(fp);
		in->bz[i]=cimag(fp);

		in->b[in->points+i]=creal(fn);
		in->bz[in->points+i]=cimag(fn);

		//printf("%d %le %le %d %d\n",i,in->b[i],in->b[i+in->points],pos,in->N);

	}

	//printf("%d %d %d\n",in->N,pos,in->M);
	//complex_solver_print(in->M,in->N,in->Ti,in->Tj, in->Tx, in->Txz,in->b,in->bz);
	//complex_solver_dump_matrix(in->M,in->N,in->Ti,in->Tj, in->Tx, in->Txz,in->b,in->bz);

	if (pos!=in->N)
	{
		printf("not solving because number does not match%d %d\n",pos,in->N);
		exit(0);
	}

	l_complex_solver(in->M,in->N,in->Ti,in->Tj, in->Tx, in->Txz,in->b,in->bz);

	for (i=0;i<in->points;i++)
	{
		double update;

		update=in->b[i];
		in->Ep[lam][i]=update;

		update=in->bz[i];
		in->Epz[lam][i]=update;

		update=in->b[in->points+i];
		in->En[lam][i]=update;

		update=in->bz[in->points+i];
		in->Enz[lam][i]=update;
	}

	ittr++;

if (test==TRUE)
{
	if (ittr>1) quit=TRUE;
	getchar();
}else
{
	quit=TRUE;
}
}while(quit==FALSE);

if (test==TRUE)
{
	printf("Error in optical model tot error=%le\n",tot_error);
}

//getchar();
return 0;
}


