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
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "sim.h"
#include "dump.h"
#include "buffer.h"

static int unused __attribute__ ((unused));

void dump_device_map(char *out_dir, struct device *in)
{
	struct buffer buf;
	char name[200];
	char temp[1000];
	int i;
	int band;
	buffer_init(&buf);

	buffer_malloc(&buf);
	join_path(2, name, out_dir, "nt_map.dat");

	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Charge carrier density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3} eV^{-1}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		for (band = 0; band < in->srh_bands; band++) {
			sprintf(temp, "%le %le %le\n", in->ymesh[i],
				in->Ec[i] + dos_get_band_energy_n(band,
								  in->imat[i]),
				in->nt[i][band]);
			buffer_add_string(&buf, temp);
		}
	}
	buffer_dump(name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);

	join_path(2, name, out_dir, "nt_map.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Charge carrier density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3} eV^{-1}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		for (band = 0; band < in->srh_bands; band++) {
			sprintf(temp, "%le %le %le\n", in->ymesh[i],
				in->Ev[i] - dos_get_band_energy_p(band,
								  in->imat[i]),
				in->pt[i][band]);
			buffer_add_string(&buf, temp);
		}
	}
	buffer_dump(name, &buf);
	buffer_free(&buf);

}
