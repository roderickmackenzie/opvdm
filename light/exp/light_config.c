#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <util.h>
#include <true_false.h>
#include "../../const.h"
#include "../../config.h"
#include "../../light.h"
#include "../../inp.h"
//#include "complex_solver.h"

static int unused __attribute__((unused));

void light_load_config(struct light *in,char *output_path)
{
inp_load(output_path,"light_exp.inp");
inp_check(1.0);
inp_search_double(&(in->simplephotondensity),"#simplephotondensity");
inp_search_double(&(in->simple_alpha),"#simple_alpha");
inp_free();
}

