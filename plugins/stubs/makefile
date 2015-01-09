inc=-I ../i/

ifndef opt_normal
	opt_normal=-Wall
endif

ifndef OPT_ARCH
	OPT_ARCH=x86_64
endif

main: stub.o
	ld  -m elf_$(OPT_ARCH) -r stub.o -o plugin.o

stub.o: stub.c
	gcc -c stub.c  $(inc) -I ../../ -o stub.o $(opt_normal)

clean:
	rm *.o
