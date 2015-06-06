inc=-I ../i/

ifndef opt_normal
	opt_normal=-Wall
endif

ifndef platform
	platform=linux
endif

main: stub.o
	$(LD)  -r stub.o -o plugin.o

stub.o: stub.c
	$(CC) -c stub.c  $(inc) -I ../../ -o stub.o $(opt_normal) -D $(platform)

clean:
	rm *.o
