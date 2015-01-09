inc=-I ../i/

ifndef opt_normal
	opt=-Wall
else
	opt=$(opt_normal)
endif

ifndef OPT_ARCH
	OPT_ARCH=x86_64
endif

main: newton.o
	ld -m elf_$(OPT_ARCH) -r newton.o -o plugin.o

newton.o: newton.c
	gcc -c newton.c -o newton.o $(opt) $(inc) 

clean:
	rm *.o
