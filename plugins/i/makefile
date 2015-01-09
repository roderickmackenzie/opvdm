ifndef opt_normal
	opt_normal=-Wall
endif

ifndef OPT_ARCH
	OPT_ARCH=x86_64
endif

main: i.o util.o list.o frame.o vec.o rnd.o
	ld -m elf_$(OPT_ARCH) -r i.o util.o list.o frame.o vec.o rnd.o -o plugin.o
i.o :i.c
	gcc -c i.c -o i.o $(opt_normal)
vec.o :vec.c
	gcc -c vec.c -o vec.o $(opt_normal)
list.o :list.c
	gcc -c list.c -o list.o $(opt_normal)
frame.o :frame.c
	gcc -c frame.c -o frame.o $(opt_normal)
util.o :util.c
	gcc -c util.c -o util.o $(opt_normal)
rnd.o :rnd.c
	gcc -c rnd.c -o rnd.o -lm $(opt_normal)

clean:
	rm *.o *.a
