#ifndef h_dump
#define h_dump
#include <frame.h>
#include "sim.h"
#include "dump_ctrl.h"
#include "dynamic_store.h"

void dump_init(struct device* in);
void dump_load_config(struct device* in);
void dump_remove_snapshots(struct device *in);
void dump_dynamic_init(struct dynamic_store *store,struct device *in);
void dump_dynamic_save(char *outputpath,struct dynamic_store *store);
void dump_dynamic_add_data(struct dynamic_store *store,struct device *in, double x_value);
void dump_dynamic_free(struct dynamic_store *store);
void dump_build_2d_charge_frame(struct map *mapin_e,struct map *mapin_h,struct device *in);
void dump_write_2d_charge_map(struct map *in_e,struct map *in_h,struct device *dev);
void frame_add_data(struct map *in,double x,double y,double data);
void dump_slice(struct device *in,char *prefix);
void dump_energy_slice(struct device *in,int num,int i);

void dump_write_2d_charge_single_map(struct map *in,struct device *dev);
void dump_build_2d_charge_single_frame(struct map *mapin,struct device *in);
void dump_device_map(char* out_dir,char* extra,struct device *in);
void dump_1d_slice(struct device *in,char *extra);
void dump_write_to_disk(struct device* in);
#endif
