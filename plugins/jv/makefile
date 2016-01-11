inc=-I ../i/

ifndef opt_normal
	opt_normal=-Wall
endif

ifndef platform
	platform=linux
endif

main: jv.o
	$(LD) -r jv.o -o plugin.o

jv.o: jv.c
	$(CC) -c jv.c  $(inc) -I ../../ -o jv.o $(opt_normal) -D $(platform)

clean:
	rm *.o
