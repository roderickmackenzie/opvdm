#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <util.h>
#include "true_false.h"
#include "../../device.h"
#include "../../dump_ctrl.h"
#include "../../light.h"
#include "../../light_interface.h"
#include "functions.h"

static double last_Psun=-1000.0;
static double last_laser_eff=-1000.0;
static double last_wavelength_laser=-1000.0;

EXPORT void light_dll_init()
{
printf("Light init\n");
last_Psun=-1000.0;
last_laser_eff=-1000.0;
last_wavelength_laser=-1000.0;
}

EXPORT void light_dll_solve_and_update(struct device *cell,struct light *in,double Psun_in,double laser_eff_in,double pulse_width)
{
in->Psun=Psun_in;
in->laser_eff=laser_eff_in;

if ((last_laser_eff!=in->laser_eff)||(last_Psun!=in->Psun)||(last_wavelength_laser!=in->laser_wavelength))
{
	l_light_solve_optical_problem(in);

	last_laser_eff=in->laser_eff;
	last_Psun=in->Psun;
	last_wavelength_laser=in->laser_wavelength;
}

l_light_dump_1d(in, in->laser_pos,"");

l_light_transfer_gen_rate_to_device(cell,in);
}
