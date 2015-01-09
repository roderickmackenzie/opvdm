inc=-I ../../plugins/i/

ifndef opt_normal
	opt_normal=-Wall
endif

main: light_config.o light.o interface.o
	gcc -shared -Wl,-soname,plugin.so light_config.o interface.o light.o -o plugin.so


light_config.o: light_config.c
	gcc -fPIC -c light_config.c -o light_config.o  $(inc) $(opt_normal)

interface.o: interface.c
	gcc -fPIC -c interface.c -o interface.o  $(inc) $(opt_normal)

light.o: light.c
	gcc -fPIC -c light.c -o light.o  $(inc) $(opt_normal)


clean:
	rm *.o
