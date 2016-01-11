inc=-I ../../plugins/i/

ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	lib_type=-DBUILDING_EXAMPLE_DLL
	link_command=-shared interface.o light.o light_dll_fixup.o -o plugin.dll -Wl,--out-implib,plugin.a

else
	CC=gcc
	lib_type=-fPIC
	link_command=-shared -Wl,-soname,plugin.so interface.o light.o light_dll_fixup.o -o plugin.so
endif


ifndef opt_normal
	opt_normal=-Wall
endif

main: light.o interface.o light_dll_fixup.o
	$(CC) $(link_command)

interface.o: interface.c
	$(CC) $(lib_type) -c interface.c -o interface.o  $(inc) $(opt_normal)

light.o: light.c
	$(CC) $(lib_type) -c light.c -o light.o  $(inc) $(opt_normal)

light_dll_fixup.o: ../../light_dll_fixup.c
	$(CC) $(lib_type) -c ../../light_dll_fixup.c -o light_dll_fixup.o  $(inc) $(opt_normal)


clean:
	rm *.o *.dll *.so
