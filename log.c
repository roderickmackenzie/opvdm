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
//    the Free Software Foundation; version 2 of the License
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
#include <stdarg.h>
#include "util.h"
#include "true_false.h"
#include "lang.h"
#include "log.h"

static int log_level = 0;

void set_logging_level(int value)
{
	log_level = value;
}

void log_clear()
{
	FILE *out;
	out = fopen("log.dat", "w");
	fprintf(out, "opvdm log file:\n");
	fclose(out);
}

void printf_log(const char *format, ...)
{
	FILE *out;
	char data[1000];
	va_list args;
	va_start(args, format);
	vsprintf(data, format, args);

	//printf("%s\n",temp);
	if ((log_level == log_level_screen)
	    || (log_level == log_level_screen_and_disk)) {
		printf("%s", data);
	}

	if ((log_level == log_level_disk)
	    || (log_level == log_level_screen_and_disk)) {
		out = fopen("log.dat", "a");
		fprintf(out, "%s", data);
		fclose(out);
	}
}
