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

#include "functions.h"

EXPORT void light_dll_ver()
{
        printf("Exponential light model\n");
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
	(*l_waveprint)(one,in->l[lam]*1e9);
}

int i;

double complex n0=0.0+0.0*I;

//complex double r=0.0+0.0*I;
complex double t=0.0+0.0*I;
double complex beta0=0.0+0.0*I;
complex double Ep=in->sun_E[lam]+0.0*I;
complex double En=0.0+0.0*I;
double dx=in->x[1]-in->x[0];

for (i=0;i<in->points;i++)
{
	//printf("%d %d\n",i,in->points);
	n0=in->nbar[lam][i];
	beta0=(2*PI*n0/in->l[lam]);
	Ep=Ep*cexp(-beta0*dx*I);

	t=in->t[lam][i];



	in->Ep[lam][i]=creal(Ep);
	in->Epz[lam][i]=cimag(Ep);
	in->En[lam][i]=creal(En);
	in->Enz[lam][i]=cimag(En);

	if (i!=(in->points-1))
	{
		if ((in->n[lam][i]!=in->n[lam][i+1])||(in->alpha[lam][i]!=in->alpha[lam][i+1]))
		{
			Ep=Ep*t;
		}
	}
}

return 0;
}


