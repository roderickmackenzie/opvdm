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
#include <string.h>
#include "sim.h"
#include <math.h>

/*
int i;
struct istruct xrange;
struct istruct data;
struct istruct intergrate;
struct istruct left;
struct istruct time;
struct istruct tau;
struct istruct loss;
struct istruct lookup;

inter_load_a(&xrange,"timedata/CEVoccurrent_sane_axes.dat",0,0);
inter_load_a(&time,"timedata/TIME_for_CE.txt",0,0);
inter_load_a(&tau,"timedata/tpv/taulaser2nd_sane.csv",0,1);
	inter_save(&tau,"tausave.dat");
double charge[xrange.len];
double tottime=time.x[time.len-1]-time.x[0];
double dt=tottime/((double)time.len);
inter_rescale(&xrange,1e-3, 1.0);
//for (i=0;i<xrange.len;i++)
//{
//	printf("%e\n",xrange.x[i]);
//}
double d=180e-9;
int x=0;
int loop=0;

inter_copy(&lookup,&xrange);

for (loop=0;loop<10;loop++)
{
printf("Dooing Loop %d\n",loop);
	for (x=0;x<xrange.len;x++)
	{
		double area=0.06/100.0/100.0;
		inter_load_a(&data,"timedata/CEVoccurrent_sane.dat",x,xrange.len);
		inter_rescale(&data,1.0, 1.0/area);
		inter_save(&data,"testsave.dat");
		inter_copy(&intergrate,&data);
		inter_copy(&left,&data);
		inter_copy(&loss,&data);

		double tot=0.0;
		for (i=0;i<intergrate.len;i++)
		{
		tot+=data.data[i]*dt;
		intergrate.data[i]=tot;
		}
		inter_save(&intergrate,"testsaveintergrate.dat");
		double cap=3.8*epsilon0*area/d;
		double capq=(xrange.x[x]*cap)/1.6e-19;	//charge on capasitor
		capq/=area;
		capq/=d;
		double charge=tot/d;
		charge*=1.0/1.6e-19;
		tot=charge-capq;

		//rewrite as total charge left in the device
		double mytau=0.0;
		for (i=0;i<left.len;i++)
		{

		//printf("%e\n",xrange.x[x]);
		left.data[i]=tot;
		if (loop!=0)
		{
		mytau=inter_get_hard(&lookup,tot);
		loss.data[i]=tot*(1.0-exp(-dt/mytau));
		}else
		{
		loss.data[i]=0.0;
		}

		tot-=data.data[i]*dt/d/1.6e-19;
		}
		double sum_lost=inter_sum(&loss);

		lookup.x[x]=charge-capq-sum_lost;
		lookup.data[x]=inter_get_hard(&tau,xrange.x[x]);

		printf("%e %e %e %e %e %e\n",xrange.x[x],charge,capq,charge-capq,inter_get_hard(&tau,xrange.x[x]),sum_lost);
		inter_save(&left,"left.dat");
		inter_save(&loss,"loss.dat");
	
		inter_free(&loss);
		inter_free(&intergrate);
		inter_free(&data);
		inter_free(&left);

	}
inter_save(&lookup,"lookuptau.dat");
getchar();
}
inter_free(&xrange);
inter_free(&time);
inter_free(&tau);
exit(0);

*/

void process_ce_data(int col, char *input, char *output)
{
	int i;
	struct istruct data;

	double d =
	    read_value("device.inp", 0, 12) + read_value("device.inp", 0, 39);
	double area =
	    read_value("device.inp", 0, 21) * read_value("device.inp", 0, 23);

	double cap = read_value("blom_bulk.inp", 0, 84) * epsilon0 * area / d;
	double capq = 0.0;

	double dt = 0.0;

	int x = 0;

	FILE *out = fopen(output, "w");

	for (x = 0; x < col; x++) {
		printf("loading.... %d\n", x);
		inter_load_by_col(&data, input, x);

		double tot = 0.0;

		for (i = 1; i < data.len - 1; i++) {
			//if ((data.x[i]>1e-6)&&(data.x[i]<1e-5))
			{
				dt = (data.x[i + 1] - data.x[i - 1]) / 2.0;
				tot += data.data[i] * dt;
			}
		}

		capq = (data.x[0] * cap) / Q;	//charge on capasitor
		capq /= area;
		capq /= d;

		tot /= area;
		tot /= d;
		tot /= Q;

		fprintf(out, "%e %e\n", data.data[0], tot - capq);
		inter_free(&data);
	}
//getchar();

	fclose(out);

}
