#ifndef hfuctions
void light_load_config(struct light *in);
void (*l_waveprint)(char*,double);
int (*l_get_dump_status)(int);
void (*l_light_dump_1d)(struct light *, int , char * );
void (*l_light_solve_optical_problem)(struct light *);
void (*l_light_free_memory)(struct light *);
void (*l_light_transfer_gen_rate_to_device)(struct device *,struct light *);
int (*l_complex_solver)(int col,int nz,int *Ti,int *Tj, double *Tx, double *Txz,double *b,double *bz);
#endif
