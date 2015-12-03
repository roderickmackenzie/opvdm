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
#include <true_false.h>
#include <util.h>
#include "../../sim.h"
#include "../../server.h"
#include "../../gui_hooks.h"

void server_stop_and_exit()
{
	exit(0);
}

void server_send_finished_to_gui(struct server *myserver)
{
	if (strcmp(myserver->dbus_finish_signal, "") != 0) {
		gui_send_data(myserver->dbus_finish_signal);
	}
}

void server_set_dbus_finish_signal(struct server *myserver, char *signal)
{
	strcpy(myserver->dbus_finish_signal, signal);
}

void server_init(struct server *myserver)
{
	strcpy(myserver->dbus_finish_signal, "");
}

void print_jobs(struct server *myserver)
{

}

void server_add_job(struct server *myserver, char *command, char *output)
{
	int odes = 0;

	if (cmpstr_min(command, "gendosn") == 0) {
		gen_dos_fd_gaus_n(extract_str_number(command, "gendosn_"));
	} else if (cmpstr_min(command, "gendosp") == 0) {
		gen_dos_fd_gaus_p(extract_str_number(command, "gendosp_"));
	} else {
		odes = run_simulation(command, output);
	}
	printf("Solved %d ODEs\n", odes);
}

int server_run_jobs(struct server *myserver)
{
	return 0;
}

void server_shut_down(struct server *myserver)
{

	server_send_finished_to_gui(&globalserver);

}
