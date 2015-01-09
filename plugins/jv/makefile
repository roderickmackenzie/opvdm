inc=-I ../i/

ifndef opt_normal
	opt_normal=-Wall
endif

ifndef OPT_ARCH
	OPT_ARCH=x86_64
endif

main: jv.o
	ld -m elf_$(OPT_ARCH) -r jv.o -o plugin.o

jv.o: jv.c
	gcc -c jv.c  $(inc) -I ../../ -o jv.o $(opt_normal)

clean:
	rm *.o
