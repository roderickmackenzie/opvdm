//<clean=none></clean>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
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
			//printf("here1 %lf\n",);
			//delay.tv_nsec=(long)(*1e9);
			if (nanosleep(&delay, NULL) < 0) {
				ewe("Nano sleep failed \n");
			}
		}

	}

}
