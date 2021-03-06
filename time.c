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
#include "sim.h"
#include "inp.h"

static int unused __attribute__ ((unused));

void time_enable_everything(int in)
{
}

void time_mesh_save()
{
}

void time_load_mesh(struct device *in, int number)
{
}

void time_init(struct device *in)
{
}

void device_timestep(struct device *in)
{
}

int time_run()
{
	return 0;
}

double time_get_voltage()
{
	return 0.0;
}

double time_get_fs_laser()
{
	return 0.0;
}

double time_get_sun()
{
	return 0.0;
}

double time_get_laser()
{
	return 0.0;
}

void time_memory_free()
{
}
