#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

static double start_times[10];

void timer_init(int timer)
{
	struct timeval result;
	gettimeofday(&result, NULL);
	start_times[timer] = result.tv_sec + result.tv_usec / 1000000.0;
}

double timer_get_time(int timer)
{
	struct timeval result;
	gettimeofday(&result, NULL);
	double cur_time = result.tv_sec + result.tv_usec / 1000000.0;
	return cur_time - start_times[timer];
}
