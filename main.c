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

	set_plot_script_dir("./");

	if (scanarg(argv, argc, "--help") == TRUE) {
		printf("opvdm_core - Organic Photovoltaic Device Model\n");
		printf
		    ("Copyright Roderick MacKenzie 2012, released under GPLv2\n");
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
		printf
		    ("Copyright and written by Roderick MacKenzie 2012, Releced under GPLv2\n\n");
		printf
		    ("This is free software; see the source code for copying conditions.\n");
		printf
		    ("There is ABSOLUTELY NO WARRANTY; not even for MERCHANTABILITY or\n");
		printf("FITNESS FOR A PARTICULAR PURPOSE.\n");
		printf("\n");
		exit(0);
	}

	if (geteuid() == 0) {
		printf("Don't run me as root!\n");
		exit(0);
	}
	set_dump_status(dump_stop_plot, FALSE);
	set_dump_status(dump_print_text, TRUE);

	set_io_dump(FALSE);
	srand(time(0));
	textcolor(fg_green);
	randomprint("Organic Photovoltaic Device Model (www.opvdm.com)\n");
	randomprint
	    ("Released under GPL v2, Copyright Roderick MacKenzie 2012\n");
	randomprint
	    ("You should have received a copy of the GNU General Public License\n");
	randomprint
	    ("along with this software.  If not, see www.gnu.org/licenses/.\n");
	randomprint("\n");
	randomprint
	    ("If you wish to collaborate in anyway please get in touch:\n");
	randomprint("roderick.mackenzie@nottingham.ac.uk\n");
	randomprint("www.roderickmackenzie.eu/contact.html\n");
	randomprint("\n");
	textcolor(fg_reset);

	lock_main(argc, argv);

	globalserver.on = FALSE;
	globalserver.cpus = 1;
	globalserver.readconfig = TRUE;

	if (scanarg(argv, argc, "--outputpath") == TRUE) {
		strcpy(sim_output_path(),
		       get_arg_plusone(argv, argc, "--outputpath"));
	} else {
		strcpy(sim_output_path(), "./");
	}

	if (scanarg(argv, argc, "--onlypos") == TRUE) {
		cell.onlypos = TRUE;
	}

	char name[1000];
	FILE *config = fopena(sim_output_path(), "ver.inp", "r");
	if (config == NULL) {
		printf
		    ("No ver.inp file, I suspect there are no input files at all\n");
		exit(0);
	}

	fscanf(config, "%s", name);
	fclose(config);

	if (strcmp(name, version) != 0) {
		printf
		    ("Software is version %s and the input files are version %s\n",
		     version, name);
		exit(0);
	}

	server_init(&globalserver);

#include "main_args.c"
	{

		gen_dos_fd_gaus_fd();

		server_add_job(&globalserver, "./");
		print_jobs(&globalserver);
		server_run_jobs(&globalserver);
	}

	server_shut_down(&globalserver);

	return 0;
}
