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

#ifndef serverh
#define serverh

#define server_max 100
#define server_no_job 0
#define server_job_ready 1
#define server_job_running 2

#include <time.h>
#include "true_false.h"

struct server globalserver;

struct my_msgbuf {
	long mtype;
	char mtext[200];
};

struct server {

	char command[server_max][200];
	int state[server_max];
	int jobs;
	int jobs_running;
	int cpus;
	int fd;
	int wd;
	int on;
	int readconfig;
	int min_cpus;
	int steel;
	int max_run_time;
	time_t end_time;
	time_t start_time;
};
void server_stop_and_exit();
void server_shut_down(struct server *myserver, char *lock_file);
void server_add_job(struct server *myserver, char *command);
void print_jobs(struct server *myserver);
void server_init(struct server *myserver);
void server_exe_jobs(struct server *myserver);
void server_job_finished(struct server *myserver, char *job);
int server_run_jobs(struct server *myserver);
double server_get_odes_per_s();
double server_get_jobs_per_s();
void change_cpus(struct server *myserver);
void server_check_wall_clock(struct server *myserver);
void server_update_last_job_time();
#endif
