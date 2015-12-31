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
#include "util.h"
#include "lang.h"

void dump_1d_slice(struct device *in, char *out_dir)
{
	int i;
	int band;
	char name[100];
	char temp[200];
	double Vexternal = get_equiv_V(in);
	struct buffer buf;
	buffer_init(&buf);

	struct stat st = { 0 };

	if (stat(out_dir, &st) == -1) {
		mkdir(out_dir, 0700);
	}

	cal_J_drift_diffusion(in);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Jn_drift.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Drift current density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron current density (drift)"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("A m^{-2}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Transport"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jn_drift, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Jn_diffusion.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Diffusion current density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron current density (diffusion)"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("A m^{-2}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Transport"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jn_diffusion, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Jp_drift.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Drift current density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Hole current density (drift)"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("A m^{-2}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Transport"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jp_drift, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Jp_diffusion.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Diffusion current density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Hole current density (diffusion)"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("A m^{-2}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Transport"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jp_diffusion, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Ec.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("LUMO-position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron Energy"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("eV"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Band structure"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Ec, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Ev.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("HOMO-position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron Energy"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("eV"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Band structure"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Ev, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Te.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Electron temperature - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Temperature (K)"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("K"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Transport"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Te, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Th.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Hole temperature - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Temperature (K)"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("K"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Transport"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Te, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Nad.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Doping - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Doping density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Charge density"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Nad, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Eg.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Band gap-position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron Energy"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("eV"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Band structure"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Eg, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Fn.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Electron quasi Fermi-level position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron Energy"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("eV"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Band structure"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Fn, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Fp.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Hole quasi Fermi-level position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron Energy"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("eV"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Band structure"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Fp, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "phi.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Potential"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Potential"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("V"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Band structure"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->phi, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "dphi.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Potential"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Potential"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("V"));
	strcpy(buf.section_one, _("Change in 1D position space output"));
	strcpy(buf.section_two, _("Band structure"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			(in->phi[i] - in->phi_save[i]));
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Jn.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Current density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron current density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("A m^{-2}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Transport"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jn, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Jp.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Current density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Hole current density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("A m^{-2}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Transport"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Jp, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Jn_plus_Jp.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Total current density (Jn+Jp) - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Total current density (Jn+Jp)"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("A m^{-2}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Transport"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
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
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Jp_drift_plus_diffusion.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Total current density (Jn+Jp) - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Total current density (Jn+Jp)"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("A m^{-2}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Transport"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			(in->Jp_drift[i] + in->Jp_diffusion[i]));
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Fi.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Equlibrium Fermi-level - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Energy"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("eV"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Band structure"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Fi, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "epsilon_r.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Relative permittivity - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Relative permittivity"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("au"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Material parameters"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->epsilonr, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "mu_n.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Electron mobility - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron mobility"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{2} V^{-1} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Material parameters"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->mun, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "mu_p.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Hole mobility - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Hole mobility"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{2} V^{-1} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Material parameters"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->mup, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "mu_n_ft.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Average electron mobility free mu0*nf/nall"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Mobility"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{2} V^{-1} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Material parameters"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->mun[i] * in->n[i] / (in->nt_all[i] + in->n[i]));
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "mu_p_ft.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Average electron mobility free mu0*nf/nall"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Mobility"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{2} V^{-1} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Material parameters"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->mup[i] * in->p[i] / (in->pt_all[i] + in->p[i]));
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "nf.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Free electron carrier density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Carrier density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Material parameters"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->n, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "pf.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Free hole carrier density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Carrier density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Material parameters"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->p, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "nt.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Trapped electron carrier density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Carrier density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Charge density"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->nt_all, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "pt.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Trapped hole carrier density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Carrier density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Charge density"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->pt_all, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "p.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Total hole density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Carrier density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Charge density"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->p[i] + in->pt_all[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "n.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Total hole density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Carrier density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Charge density"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->n[i] + in->nt_all[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "dn.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Change in electron population - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Carrier density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Chaerge density"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			(in->n[i] + in->nt_all[i]) - (in->nf_save[i] +
						      in->nt_save[i]));
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "charge.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Total charge - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Carrier density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Charge density"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->p[i] - in->n[i] + in->pt_all[i] - in->nt_all[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "dcharge.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Change in total charge - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Carrier density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Charge density"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->p[i] - in->n[i] + in->pt_all[i] - in->nt_all[i] -
			(in->pt_save[i] - in->nf_save[i] + in->pf_save[i] -
			 in->nt_save[i]));
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "dp.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Change in hole population - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Carrier density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Charge density"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %e\n", in->ymesh[i],
			in->p[i] + in->pt_all[i] - in->pf_save[i] -
			in->pt_save[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "dnt.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Excess electron density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Charge density"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %le", in->ymesh[i],
			in->nt_all[i] - in->nt_save[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "dpt.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Excess electron density - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Hole density"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Charge density"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %le", in->ymesh[i],
			in->pt_all[i] - in->pt_save[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Gn.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Free electron generation rate - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Generation rate"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Recombination"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Gn, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Gp.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Free hole generation rate - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Generation rate"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Recombination"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Gp, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Rn.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Electron recombination rate - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Recombination rate"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Recombination"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Rn, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Rp.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Hole recombination rate - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Recombination rate"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Recombination"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->Rp, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "fsrhn.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Trap fermi level - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron Fermi level"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("eV"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Recombination"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
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
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "fsrhh.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Trap fermi level - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Electron Fermi level"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("eV"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Recombination"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
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
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "imat.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Material number - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Number"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("au"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Model"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	for (i = 0; i < in->ymeshpoints; i++) {
		sprintf(temp, "%e %d\n", in->ymesh[i], in->imat[i]);
		buffer_add_string(&buf, temp);
	}
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "Efield.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Material number - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Number"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("au"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Band structure"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	double deriv = 0.0;
	for (i = in->ymeshpoints - 1; i > 1; i--) {

		deriv =
		    -(in->phi[i] - in->phi[i - 1]) / (in->ymesh[i] -
						      in->ymesh[i - 1]);
		sprintf(temp, "%e %le\n", in->ymesh[i], deriv);
		buffer_add_string(&buf, temp);
	}

	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "pf_to_nt.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Free hole to trapped electron - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Rate"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Recombination"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->ntrap_to_p, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "nf_to_pt.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Free electron to trapped hole - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Rate"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Recombination"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->ptrap_to_n, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "prelax.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Hole relaxation rate - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Rate"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Recombination"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->prelax, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name, "%s", "nrelax.dat");
	buf.y_mul = 1.0;
	buf.x_mul = 1e9;
	strcpy(buf.title, _("Electron relaxation rate - position"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Position"));
	strcpy(buf.y_label, _("Rate"));
	strcpy(buf.x_units, _("nm"));
	strcpy(buf.y_units, _("m^{-3} s^{-1}"));
	strcpy(buf.section_one, _("1D position space output"));
	strcpy(buf.section_two, _("Recombination"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buf.time = 0.0;
	buf.Vexternal = Vexternal;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, in->ymesh, in->nrelax, in->ymeshpoints);
	buffer_dump_path(out_dir, name, &buf);
	buffer_free(&buf);

}
