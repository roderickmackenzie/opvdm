#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "sim.h"
#include "dump.h"

static int unused __attribute__((unused));
static int dump_number;
void dump_init(struct device* in)
{
dump_number=0;
set_dump_status(dump_lock, FALSE);
#ifdef enable_time
FILE *config;
config=fopena(in->outputpath,"/snapshots/dump_slice_info.dat","w");
if (config!=NULL)
{
	fclose(config);
}
#endif
}

void dump_write_to_disk(struct device* in)
{
#ifdef enable_time
char prefix[100];
char out_dir[200];
FILE* out;
int dumped=FALSE;
struct stat st = {0};

	sprintf(prefix,"_%d",dump_number);


	sprintf(out_dir,"%s/snapshots/",in->outputpath);


	if (stat(out_dir, &st) == -1)
	{
	#ifdef windows
		mkdir(out_dir);
	#else
		mkdir(out_dir, 0700);
	#endif
	}

	if (get_dump_status(dump_1d_slices)==TRUE)
	{
		dump_1d_slice(in,prefix);
		dumped=TRUE;
	}

	if (get_dump_status(dump_energy_slice_switch)==TRUE)
	{
		dump_energy_slice(in,dump_number,in->dump_slicepos);
		dumped=TRUE;
	}

	if (get_dump_status(dump_pl)==TRUE)
	{
		exp_cal_emission(dump_number,in);
		dumped=TRUE;
	}

	if (dumped==TRUE)
	{
		out=fopena(in->outputpath,"/snapshots/dump_slice_info.dat","a");
		if (out!=NULL)
		{
			fprintf(out,"%.4lf %le\n",get_equiv_V(in),in->time);
			fclose(out);
		}
		dump_number++;
	}


#endif
}

