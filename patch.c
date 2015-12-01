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
#include <util.h>
#include <true_false.h>
#include "patch.h"
#include "../../dump_ctrl.h"
#include "../../inp.h"

static int unused __attribute__ ((unused));

void patch(char *dest, char *patch_file)
{
	FILE *in;
	char token[100];
	char file[100];
	char newtext[100];

	if ((in = fopen(patch_file, "r")) == NULL) {
		ewe("Error opening file: %s\n", patch_file);
	}
	char filetoedit[200];
	if (get_dump_status(dump_iodump) == TRUE)
		printf("Patch %s\n", patch_file);
	int found = FALSE;

	struct inp_file ifile;
	inp_init(&ifile);

	do {
		unused = fscanf(in, "%s", token);

		if (strcmp(token, "#end") == 0) {
			break;
		}
		if (token[0] != '#') {
			ewe("error token does not begin with #\n", token);
		} else {
			found = TRUE;
			unused = fscanf(in, "%s", file);
			unused = fscanf(in, "%s", newtext);
			join_path(2, filetoedit, dest, file);
			inp_load(&ifile, filetoedit);
			inp_replace(&ifile, token, newtext);

		}

	} while (!feof(in));

	inp_free(&ifile);

	if (strcmp(token, "#end") != 0) {
		ewe("Error at end of patch file\n");
	}

	fclose(in);

	if (found == FALSE) {
		ewe("Token not found\n");
	}

	return;
}
