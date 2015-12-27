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

void dump_energy_slice(char *out_dir, struct device *in)
{
	struct buffer buf;
	buffer_init(&buf);
	char name[200];
	int band = 0;
	int i = in->dump_slicepos;

	char outpath[200];

	struct istruct dump1;
	inter_init(&dump1);

	struct istruct dump2;
	inter_init(&dump2);

	struct istruct dump3;
	inter_init(&dump3);

	struct istruct dump4;
	inter_init(&dump4);

	struct istruct dump5;
	inter_init(&dump5);

	struct istruct dump6;
	inter_init(&dump6);

	struct istruct dump9;
	inter_init(&dump9);

	struct istruct dump10;
	inter_init(&dump10);

	struct istruct dump_nt;
	inter_init(&dump_nt);

	struct istruct dump_pt;
	inter_init(&dump_pt);

	int mat = in->imat[in->ymeshpoints / 2];
	for (band = 0; band < in->srh_bands; band++) {
		inter_append(&dump1, get_dos_E_n(band, mat),
			     in->n[i] * in->srh_n_r1[i][band] -
			     in->srh_n_r2[i][band]);
		inter_append(&dump2, get_dos_E_p(band, mat),
			     in->p[i] * in->srh_p_r1[i][band] -
			     in->srh_p_r2[i][band]);
		inter_append(&dump3, get_dos_E_n(band, mat),
			     in->nt[i][band] - in->ntb_save[i][band]);
		inter_append(&dump4, get_dos_E_p(band, mat),
			     in->pt[i][band] - in->ptb_save[i][band]);
		inter_append(&dump5, get_dos_E_n(band, mat),
			     in->p[i] * in->srh_n_r3[i][band] -
			     in->srh_n_r4[i][band]);
		inter_append(&dump6, get_dos_E_p(band, mat),
			     in->n[i] * in->srh_p_r3[i][band] -
			     in->srh_p_r4[i][band]);
		inter_append(&dump9, get_dos_E_n(band, mat), in->Fnt[i][band]);
		inter_append(&dump10, get_dos_E_p(band, mat), in->Fpt[i][band]);
		inter_append(&dump_nt, get_dos_E_n(band, mat), in->nt[i][band]);
		inter_append(&dump_pt, get_dos_E_p(band, mat), in->pt[i][band]);

	}

	sprintf(outpath, "%senergy_slice_nt_cap_%s", out_dir, ".dat");
//inter_save(&dump1,outpath);
	inter_free(&dump1);

	sprintf(outpath, "%senergy_slice_pt_cap_%s", out_dir, ".dat");
//inter_save(&dump2,outpath);
	inter_free(&dump2);

	sprintf(outpath, "%senergy_slice_nt_delta_%s", out_dir, ".dat");
//inter_save(&dump3,outpath);
	inter_free(&dump3);

	sprintf(outpath, "%senergy_slice_pt_delta_%s", out_dir, ".dat");
//inter_save(&dump4,outpath);
	inter_free(&dump4);

	sprintf(outpath, "%senergy_slice_nt_recom_%s", out_dir, ".dat");
//inter_save(&dump5,outpath);
	inter_free(&dump5);

	sprintf(outpath, "%senergy_slice_pt_recom_%s", out_dir, ".dat");
//inter_save(&dump6,outpath);
	inter_free(&dump6);

	sprintf(outpath, "%senergy_slice_fn_%s", out_dir, ".dat");
//inter_save(&dump9,outpath);
	inter_free(&dump9);

	sprintf(outpath, "%senergy_slice_fp_%s", out_dir, ".dat");
//inter_save(&dump10,outpath);
	inter_free(&dump10);

	buffer_malloc(&buf);
	sprintf(name, "energy_slice_nt.dat");
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
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);
	inter_free(&dump_nt);

	buffer_malloc(&buf);
	sprintf(name, "energy_slice_pt.dat");
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
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);
	inter_free(&dump_pt);

}
