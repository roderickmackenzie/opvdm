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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "sim.h"

static int unused __attribute__ ((unused));

void stop_start(struct device *in)
{
	struct timespec delay;

	if (in->stop_start == TRUE) {

		if (in->start_stop_time == 0.0) {
			getchar();
		} else {
			double sec = (int)in->start_stop_time;
			double ns = (in->start_stop_time - (double)sec) * 1e9;
			delay.tv_sec = (long int)sec;
			delay.tv_nsec = (long int)ns;

			if (nanosleep(&delay, NULL) < 0) {
				ewe("Nano sleep failed \n");
			}
		}

	}

}
