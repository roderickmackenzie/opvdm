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
#include <signal.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include "util.h"
#include <pwd.h>
#include "sim.h"
#include "dos.h"
#include "server.h"
#include "elec_plugins.h"
#include "light_interface.h"
#include "dump.h"
#include "complex_solver.h"
#include "elec_plugins.h"
#include "license.h"
#include "inp.h"
#include "gui_hooks.h"

static int unused __attribute__ ((unused));

struct device cell;

void device_init(struct device *in)
{
	in->Voc = 0.0;
	in->Jsc = 0.0;
	in->FF = 0.0;
	in->Pmax = 0.0;
	in->Pmax_voltage = 0.0;
	device_get_memory(in);

}

int main(int argc, char *argv[])
{
	cell.onlypos = FALSE;
#ifndef nolock
	antje0();
#endif

	set_plot_script_dir("./");

	if (scanarg(argv, argc, "--help") == TRUE) {
		printf("opvdm_core - Organic Photovoltaic Device Model\n");
		printf(copyright);
		printf("\n");
		printf("Usage: opvdm_core [options]\n");
		printf("\n");
		printf("Options:\n");
		printf("\n");
		printf("\t--outputpath\tdirectory to run (default ./)\n");
		printf("\t--version\tdisplays the current version\n");
		printf("\n");
		printf
		    ("Additional information about opvdm is available at www.opvdm.com.\n");
		printf("\n");
		printf
		    ("Report bugs to: roderick.mackenzie@nottingham.ac.uk\n\n");
		exit(0);
	}
	if (scanarg(argv, argc, "--version") == TRUE) {
		printf("opvdm_core, Version %s\n", version);
		printf(copyright);
		printf(this_is_free_software);
		printf
		    ("There is ABSOLUTELY NO WARRANTY; not even for MERCHANTABILITY or\n");
		printf("FITNESS FOR A PARTICULAR PURPOSE.\n");
		printf("\n");
		exit(0);
	}

	if (geteuid() == 0) {
		ewe("Don't run me as root!\n");
	}
	lock_command_line(argc, argv);

	set_dump_status(dump_stop_plot, FALSE);
	set_dump_status(dump_print_text, TRUE);

	set_io_dump(FALSE);
	srand(time(0));
	textcolor(fg_green);
	randomprint("Organic Photovoltaic Device Model (www.opvdm.com)\n");
	randomprint(copyright);
#ifdef gnu_copyright
	randomprint
	    ("You should have received a copy of the GNU General Public License\n");
	randomprint
	    ("along with this software.  If not, see www.gnu.org/licenses/.\n");
#endif
	randomprint("\n");
	randomprint
	    ("If you wish to collaborate in anyway please get in touch:\n");
	randomprint("roderick.mackenzie@nottingham.ac.uk\n");
	randomprint("www.roderickmackenzie.eu/contact.html\n");
	randomprint("\n");
	textcolor(fg_reset);

	globalserver.on = FALSE;
	globalserver.cpus = 1;
	globalserver.readconfig = TRUE;

	if (scanarg(argv, argc, "--outputpath") == TRUE) {
		strcpy(sim_output_path(),
		       get_arg_plusone(argv, argc, "--outputpath"));
	} else {
		strcpy(sim_output_path(), "./");
	}

	gui_start();
	if (scanarg(argv, argc, "--optics") == TRUE) {
		struct light two;
		light_init(&two, &cell);

		light_load_config(&two, "./");
		set_dump_status(dump_lock, FALSE);
		set_dump_status(dump_optics, TRUE);
		set_dump_status(dump_optics_verbose, FALSE);
		light_solve_and_update(&cell, &two, 1000.0, 0.0);
		light_dump(&two);
		light_free(&two);
		return 0;
	}

	if (scanarg(argv, argc, "--onlypos") == TRUE) {
		cell.onlypos = TRUE;
	}

	char lock_file[1000];

	if (scanarg(argv, argc, "--lock") == TRUE) {
		strcpy(lock_file, get_arg_plusone(argv, argc, "--lock"));
	} else {
		strcpy(lock_file, "");
	}

	char name[200];
	inp_load(sim_output_path(), "ver.inp");
	inp_check(1.0);
	inp_search_string(name, "#opvdm_version");
	inp_free();

	if (strcmp(name, version) != 0) {
		printf
		    ("Software is version %s and the input files are version %s\n",
		     version, name);
		exit(0);
	}

	server_init(&globalserver);
	int ret = 0;

#include "main_args.c"
	{
		gen_dos_fd_gaus_fd();

		server_add_job(&globalserver, "./");
		print_jobs(&globalserver);
		ret = server_run_jobs(&globalserver);

	}

	server_shut_down(&globalserver, lock_file);
	gui_stop();
	if (ret != 0) {
		return 1;
	}
	return 0;
}
