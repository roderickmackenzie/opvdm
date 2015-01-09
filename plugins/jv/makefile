inc=-I ../i/

ifndef opt_normal
	opt_normal=-Wall
endif

main: jv.o
	ld -r jv.o -o plugin.o

jv.o: jv.c
	gcc -c jv.c  $(inc) -I ../../ -o jv.o $(opt_normal)

clean:
	rm *.o
