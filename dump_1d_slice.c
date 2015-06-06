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

void dump_1d_slice(struct device *in, char *extra)
{
	char out_dir[200];
	struct buffer buf;
	buffer_init(&buf);

	if (strcmp(extra, "") != 0) {
		sprintf(out_dir, "%s/snapshots/", in->outputpath);
	} else {
		strcpy(out_dir, in->outputpath);
	}

	int i;
	int band;

	char name[100];
	char temp[200];
	cal_J_drift_diffusion(in);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Jn_drift", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Drift current density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron current density (drift)");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "A m^{-2}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Transport");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jn_drift, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Jn_diffusion", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Diffusion current density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron current density (diffusion)");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "A m^{-2}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Transport");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jn_diffusion, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Jp_drift", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Drift current density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Hole current density (drift)");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "A m^{-2}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Transport");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jp_drift, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Jp_diffusion", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Diffusion current density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Hole current density (diffusion)");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "A m^{-2}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Transport");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jp_diffusion, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Ec", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "LUMO-position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron Energy");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "eV");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Band structure");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Ec, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Ev", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "HOMO-position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron Energy");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "eV");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Band structure");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Ev, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Te", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Electron temperature - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Temperature (K)");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "K");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Transport");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Te, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Th", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Hole temperature - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Temperature (K)");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "K");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Transport");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Te, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Nad", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Doping - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Doping density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Charge density");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Nad, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Eg", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Band gap-position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron Energy");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "eV");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Band structure");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Eg, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Fn", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Electron quasi Fermi-level position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron Energy");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "eV");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Band structure");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Fn, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Fp", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Hole quasi Fermi-level position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron Energy");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "eV");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Band structure");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Fp, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "phi", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Potential");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Potential");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "V");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Band structure");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->phi, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "dphi", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Potential");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Potential");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "V");
	strcpy(buf.section_one, "Change in 1D position space output");
	strcpy(buf.section_two, "Band structure");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			(in->phi[i] - in->phi_save[i]));
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Jn", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Current density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron current density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "A m^{-2}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Transport");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jn, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Jp", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Current density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Hole current density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "A m^{-2}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Transport");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jp, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Jn_plus_Jp", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Total current density (Jn+Jp) - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Total current density (Jn+Jp)");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "A m^{-2}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Transport");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	sprintf(temp, "%e %e\n", in->ymesh[0] - in->ymesh[1] / 2,
		get_J_left(in));
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i], (in->Jp[i] + in->Jn[i]));
		buffer_add_string(&buf, temp);
	}
	sprintf(temp, "%e %e\n",
		in->ymesh[in->ymeshpoints - 1] - in->ymesh[1] / 2,
		get_J_right(in));
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Jp_drift_plus_diffusion", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Total current density (Jn+Jp) - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Total current density (Jn+Jp)");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "A m^{-2}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Transport");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			(in->Jp_drift[i] + in->Jp_diffusion[i]));
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Fi", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Equlibrium Fermi-level - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Energy");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "eV");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Band structure");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Fi, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "epsilon_r", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Relative permittivity - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Relative permittivity");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "au");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Material parameters");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->epsilonr, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "mu_n", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Electron mobility - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron mobility");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{2} V^{-1} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Material parameters");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->mun, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "mu_p", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Hole mobility - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Hole mobility");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{2} V^{-1} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Material parameters");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->mup, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "mu_n_ft", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Electron mobility free/all- position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Mobility");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{2} V^{-1} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Material parameters");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->mun[i] * in->n[i] / in->nt_all[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "mu_p_ft", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Hole mobility free/all- position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Mobility");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{2} V^{-1} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Material parameters");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->mup[i] * in->p[i] / in->pt_all[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "nf", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Free electron carrier density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Material parameters");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->n, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "pf", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Free hole carrier density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Material parameters");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->p, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "nt", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Trapped electron carrier density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Charge density");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->nt_all, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "pt", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Trapped hole carrier density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Charge density");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->pt_all, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "p", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Total hole density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Charge density");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->p[i] + in->pt_all[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "n", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Total hole density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Charge density");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->n[i] + in->nt_all[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "dn", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Change in electron population - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Chaerge density");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			(in->n[i] + in->nt_all[i]) - (in->nf_save[i] +
						      in->nt_save[i]));
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "charge", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Total charge - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Charge density");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->p[i] - in->n[i] + in->pt_all[i] - in->nt_all[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "dcharge", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Change in total charge - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Charge density");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->p[i] - in->n[i] + in->pt_all[i] - in->nt_all[i] -
			(in->pt_save[i] - in->nf_save[i] + in->pf_save[i] -
			 in->nt_save[i]));
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "dp", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Change in hole population - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Carrier density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Charge density");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->p[i] + in->pt_all[i] - in->pf_save[i] -
			in->pt_save[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "dnt", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Excess electron density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Charge density");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %le", in->ymesh[i],
			in->nt_all[i] - in->nt_save[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "dpt", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Excess electron density - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Hole density");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Charge density");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %le", in->ymesh[i],
			in->pt_all[i] - in->pt_save[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Gn", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Free electron generation rate - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Generation rate");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Recombination");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Gn, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Gp", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Free hole generation rate - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Generation rate");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Recombination");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Gp, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Rn", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Electron recombination rate - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Recombination rate");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Recombination");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Rn, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Rp", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Hole recombination rate - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Recombination rate");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Recombination");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Rp, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "fsrhn", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Trap fermi level - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron Fermi level");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "eV");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Recombination");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e ", in->ymesh[i]);
		buffer_add_string(&buf, temp);
		for (band = 0; band < in->srh_bands; band++) {
			sprintf(temp, "%e %e ", in->Fnt[i][band],
				-in->phi[i] - in->Xi[i] +
				dos_srh_get_fermi_n(in->n[i], in->p[i], band,
						    in->imat[i], in->Te[i]));
			buffer_add_string(&buf, temp);
		}
		buffer_add_string(&buf, "\n");

	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "fsrhh", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Trap fermi level - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Electron Fermi level");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "eV");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Recombination");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e ", in->ymesh[i]);
		buffer_add_string(&buf, temp);
		for (band = 0; band < in->srh_bands; band++) {
			sprintf(temp, "%e %e ", in->Fpt[i][band],
				-in->phi[i] - in->Xi[i] - in->Eg[i] -
				dos_srh_get_fermi_p(in->n[i], in->p[i], band,
						    in->imat[i], in->Th[i]));
			buffer_add_string(&buf, temp);
		}
		buffer_add_string(&buf, "\n");

	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "imat", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Material number - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Number");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "au");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Model");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %d\n", in->ymesh[i], in->imat[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "Efield", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Material number - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Number");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "au");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Band structure");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	double deriv = 0.0;
	for (i = in->ymeshpoints - 1; i > 1; i--) {

		deriv =
		    -(in->phi[i] - in->phi[i - 1]) / (in->ymesh[i] -
						      in->ymesh[i - 1]);
		sprintf(temp, "%e %le\n", in->ymesh[i], deriv);
		buffer_add_string(&buf, temp);
	}

	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "pf_to_nt", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Free hole to trapped electron - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Rate");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Recombination");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->ntrap_to_p, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "nf_to_pt", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Free electron to trapped hole - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Rate");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Recombination");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->ptrap_to_n, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "prelax", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Hole relaxation rate - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Rate");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Recombination");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->prelax, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s%s%s", "nrelax", extra, ".dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, "Electron relaxation rate - position");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Position");
	strcpy(buf.y_label, "Rate");
	strcpy(buf.x_units, "nm");
	strcpy(buf.y_units, "m^{-3} s^{-1}");
	strcpy(buf.section_one, "1D position space output");
	strcpy(buf.section_two, "Recombination");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->nrelax, in->ymeshpoints);
	buffer_dump(out_dir, name, &buf);
	buffer_free(&buf);

	dump_device_map(out_dir, extra, in);

}