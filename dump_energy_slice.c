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
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "sim.h"
#include "dump.h"
#include "buffer.h"

void dump_energy_slice(struct device *in, int num, int i)
{
	struct buffer buf;
	buffer_init(&buf);
	char out_dir[200];
	char name[200];

	sprintf(out_dir, "%s/snapshots/", in->outputpath);
	struct stat st = { 0 };

	if (stat(out_dir, &st) == -1) {
#ifdef windows
		mkdir(out_dir);
#else
		mkdir(out_dir, 0700);
#endif
	}

	int band = 0;

	struct istruct dump_nt;
	inter_init(&dump_nt);

	struct istruct dump_pt;
	inter_init(&dump_pt);

	int mat = in->imat[in->ymeshpoints / 2];
	for (band = 0; band < in->srh_bands; band++) {
		inter_append(&dump_nt, get_dos_E_n(band, mat), in->nt[i][band]);
		inter_append(&dump_pt, get_dos_E_p(band, mat), in->pt[i][band]);

	}

	buffer_malloc(&buf);
	sprintf(name, "energy_slice_nt_%d%s", num, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Energy - trap ocupation");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Energy");
	strcpy(buf.y_label, "Ocupation");
	strcpy(buf.x_units, "eV");
	strcpy(buf.y_units, "m^{-3} eV^{-1}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, dump_nt.x, dump_nt.data, dump_nt.len);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);
	inter_free(&dump_nt);

	buffer_malloc(&buf);
	sprintf(name, "energy_slice_pt_%d%s", num, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Energy - trap ocupation");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Energy");
	strcpy(buf.y_label, "Ocupation");
	strcpy(buf.x_units, "eV");
	strcpy(buf.y_units, "m^{-3} eV^{-1}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, dump_pt.x, dump_pt.data, dump_pt.len);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);
	inter_free(&dump_pt);

}
