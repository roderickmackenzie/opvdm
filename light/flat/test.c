#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <i.h>
#include "light.h"

int main()
{
struct light two;
light_load_config(&two);
light_free_memory(&two);
return 0;
/*int ext=0;
struct fitvars config;
void *lib_handle;
double (*fn)(struct fitvars *,int,char *);
char *error;

char libname[100];

sprintf(libname,"./error_plugin.so");

   lib_handle = dlopen(libname, RTLD_LAZY);
   if (!lib_handle) 
   {
      fprintf(stderr, "%s\n", dlerror());
   }

   fn = dlsym(lib_handle, "lib_error");
   if ((error = dlerror()) != NULL)  
   {
      fprintf(stderr, "%s\n", error);
   }

   double ret=(*fn)(&config,ext,"../../");
printf ("%le\n",ret);
   dlclose(lib_handle);
return 0;*/

}
