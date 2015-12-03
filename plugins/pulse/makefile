inc=-I ../i/

ifndef opt_normal
	opt_normal=-Wall
endif

ifndef platform
	platform=linux
endif

main: pulse.o pulse_ntricks.o
	$(LD) -r pulse.o pulse_ntricks.o -o plugin.o

pulse.o: pulse.c
	$(CC) -c pulse.c -o pulse.o  $(inc) $(opt_normal) -D $(platform)

pulse_ntricks.o: pulse_ntricks.c
	$(CC) -c pulse_ntricks.c -o pulse_ntricks.o  $(inc) $(opt_normal) -D $(platform)

clean:
	rm *.o
