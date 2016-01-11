inc=-I ../i/

ifndef opt_normal
	opt=-Wall
else
	opt=$(opt_normal)
endif

ifndef platform
	platform=linux
endif

main: newton.o
	$(LD) -r newton.o -o plugin.o

newton.o: newton.c
	$(CC) -c newton.c -o newton.o $(opt) $(inc) -D $(platform)

clean:
	rm *.o
