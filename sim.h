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
#ifndef sim_h
#define sim_h
#include <i.h>
#include <true_false.h>
#include "code_ctrl.h"
#include "const.h"
#include "true_false.h"
#include "sim_modes.h"
#include "version.h"

#define srh_1	1
#define srh_2	2
#define srh_3	3
#define srh_4	4
#define interface_schottky	 1

#include "device.h"
#include "dos.h"
#include "exp.h"
#include "plot.h"
#include "util.h"
#include "dump.h"
#include "complex_solver.h"
#include "config.h"
#include "solver.h"
#include "advmath.h"
#include "light_interface.h"
#include "newton.h"

char *sim_output_path();

int solve_cur(struct device *in);
int solve_cur_thermal(struct device *in, int thermal);
int solve_pos(struct device *in);
void get_initial(struct device *in);
void device_get_memory(struct device *in);
void device_free(struct device *in);
void time_load_mesh(struct device *in);
void time_update_dt(struct device *in);
void time_init(struct device *in);
void update_arrays(struct device *in);
void device_timestep(struct device *in);
void find_n0(struct device *in);

int get_clamp_state();

void antje0();
int antje1();
void antje2();

void get_max_layers(int in);
void lock_main(int argc, char *argv[]);

void init_mat_arrays(struct device *in);

void device_init(struct device *in);
void load_config(char *simfile, struct device *in);
void update(struct device *cell);
int run_simulation(char *name);
void solve_all(struct device *in);
void solver_free_memory(struct device *in);
void solver_realloc(struct device *in);
void solver_free();

void gen_dos_fd_gaus_fd();

void solve_light(struct device *cell, struct light *in, double Psun_in,
		 double Plaser_in);
void light_solve_and_update(struct device *cell, struct light *in,
			    double Psun_in, double Plaser_in);

void stop_start(struct device *in);
#endif
