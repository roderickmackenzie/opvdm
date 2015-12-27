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
#include <unistd.h>

#include <sys/types.h>
#include <dirent.h>
#include "inp.h"
#include "true_false.h"
#include "device.h"
#include "dump_ctrl.h"
#include "rand.h"

int random_int_range(int start_in, int stop_in)
{
	int start = start_in;
	int stop = stop_in;

	int temp = 0;

	if (start > stop) {
		temp = start;
		start = stop;
		stop = temp;
	}
	int ret = 0;
	int delta = (stop - start);

	ret = random_int(delta);

	return start + ret;
}

int random_int(int in)
{
	if (in == 0)
		return 0;
	int out = 0;
	int random;
	FILE *fp = fopen("/dev/urandom", "r");
	if (fread(&random, sizeof(int), 1, fp) == 0) {
		ewe("No data read from urandom\n");
	}
	random = fabs(random);
	out = random % (in + 1);
//printf("%d\n", out);
	fclose(fp);
	return out;
}

void randomize_input_files()
{
	struct inp_file inp;
	inp_init(&inp);

	struct inp_file ifile;
	inp_init(&ifile);

	inp_load(&inp, "random.inp");
	inp_check(&inp, 1.0);
	inp_reset_read(&inp);
	char *data;
	char file[100];
	char token[100];
	double man_min = 0.0;
	double man_max = 0.0;
	double exp_min = 0.0;
	double exp_max = 0.0;
	double a = 0.0;
	double b = 0.0;
	double value = 0.0;
	char value_string[100];
	do {
		data = inp_get_string(&inp);

		if (strcmp(data, "#ver") == 0) {
			break;
		}

		sscanf(data, "%s %s %le %le %le %le", file, token, &man_min,
		       &man_max, &exp_min, &exp_max);
		//printf("%s '%s' %f %f %f %f\n",file,token,man_min,man_max,exp_min,exp_max);
		a = (double)random_int_range(man_min, man_max);
		b = (double)random_int_range(exp_min, exp_max);
		value = a * pow(10, b);
		sprintf(value_string, "%le", value);
		inp_load(&ifile, file);
		inp_replace(&ifile, token, value_string);
		inp_free(&ifile);

		//printf("%f %f %le\n",a,b,value);

	} while (1);

	inp_free(&inp);

}
