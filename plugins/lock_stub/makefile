inc=-I ../i/

ifndef opt_normal
	opt_normal=-Wall
endif

ifndef OPT_ARCH
	OPT_ARCH=x86_64
endif

main: lock_stub.o
	ld  -m elf_$(OPT_ARCH) -r lock_stub.o -o plugin.o

lock_stub.o: lock_stub.c
	gcc -c lock_stub.c  $(inc) -I ../../ -o lock_stub.o $(opt_normal)

clean:
	rm *.o
