ifndef opt_normal
	opt_normal=-Wall
endif

ifndef platform
	platform=linux
endif

main: i.o util.o list.o frame.o vec.o rnd.o
	$(LD) -r i.o util.o list.o frame.o vec.o rnd.o -o plugin.o
i.o :i.c
	$(CC) -c i.c -o i.o $(opt_normal) -D $(platform)
vec.o :vec.c
	$(CC) -c vec.c -o vec.o $(opt_normal) -D $(platform)
list.o :list.c
	$(CC) -c list.c -o list.o $(opt_normal) -D $(platform)
frame.o :frame.c
	$(CC) -c frame.c -o frame.o $(opt_normal) -D $(platform)
util.o :util.c
	$(CC) -c util.c -o util.o $(opt_normal) -D $(platform)
rnd.o :rnd.c
	$(CC) -c rnd.c -o rnd.o -lm $(opt_normal) -D $(platform)

clean:
	rm *.o *.a
