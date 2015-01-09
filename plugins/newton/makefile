inc=-I ../i/

ifndef opt_normal
	opt=-Wall
else
	opt=$(opt_normal)
endif

main: newton.o
	ld -r newton.o -o plugin.o

newton.o: newton.c
	gcc -c newton.c -o newton.o $(opt) $(inc) 

clean:
	rm *.o
