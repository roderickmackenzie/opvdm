inc=-I ../i/

ifndef opt_normal
	opt_normal=-Wall
endif

main: stub.o
	ld -r stub.o -o plugin.o

stub.o: stub.c
	gcc -c stub.c  $(inc) -I ../../ -o stub.o $(opt_normal)

clean:
	rm *.o
