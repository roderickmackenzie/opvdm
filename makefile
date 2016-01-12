#<clean=none></clean>
plugins:=$(shell ./get_elec_plugins.sh)

#windows=""

DEST_LIB=lib64

ifndef OPT_ARCH
	OPT_ARCH=x86_64
endif

ifdef OPT_FLAGS
	opt_slow=$(OPT_FLAGS)
	opt_normal=$(OPT_FLAGS)
	opt_fast=$(OPT_FLAGS)
	warn=
	debug_opt=
else
	opt_slow=-O0
	opt_normal=-O2 
	opt_fast=-O5
	warn=-Werror -Wall
	debug_opt=-pg  -ggdb -g
endif

llink=
opt_normal+= -D disable_lock -D full_time_domain -D enable_fx
#ifeq ($(wildcard secure.h),)
#	opt_normal+= 
#	llink=
#else
#	opt_normal+= -D no_debug
#	llink=-lpci -lcurl
#endif

inc=
link=

ifeq ($(wildcard ~/.opvdm_hpc_flag),)
		ifndef windows
        inc+= `pkg-config --cflags dbus-1`
		opt_normal+= -D dbus
        link+= `pkg-config --libs dbus-1`		
		endif
else
        inc+= -I/home/steve/rm/build/libmatheval-1.1.11/lib/
        link+= -L/home/steve/rm/build/libmatheval-1.1.11/lib/.libs/
        debug_opt=
endif

ifdef windows
	CC=i686-w64-mingw32-gcc
	LD=i686-w64-mingw32-ld
	platform=windows
	
        inc+=-I$(HOME)/windll/zlib/include/ -I$(HOME)/windll/openssl-1.0.1j/include/ -I$(HOME)/windll/libzip/libzip-0.11.2/lib/ -I$(HOME)/windll/gsl-1.16/
	llink= -L$(HOME)/windll/gsl-1.16/.libs/ -L$(HOME)/windll/gsl-1.16/cblas/.libs/ ./images/res.o -lzip-2 -lgsl -lgslcblas
	#llink+=-L~/windll/umfpack/AMD/Lib -L~/windll/umfpack/UMFPACK/Lib -L~/windll/openssl-1.0.1j/ -static-libgcc -static-libstdc++
else
	CC=gcc
	LD=ld
	platform=linux
	link+= -rdynamic -export-dynamic -lgsl -lgslcblas -lblas -ldl -lzip -lz -lamd -lmatheval
endif

        flags=${debug_opt} -D dos_bin -D ${platform}
        inc+=-I/usr/include/suitesparse/ -I ./plugins/i/

.PHONY: clean

main: main.c solver.o light_utils.o gui_hooks.o sim_find_n0.o sim_run.o newton_update.o dump_map.o dump_energy_slice.o pos.o inital.o advmath.o config.o plot.o timer.o memory.o dos.o gendosfdgaus.o exp.o pl.o time.o fast.o anal.o dump.o dump_config.o dump_1d_slice.o dump_dynamic.o ntricks.o ntricks_externalv.o dos_an.o startstop.o complex_solver.o thermal.o light_interface.o dump_ctrl.o light_dump.o light_test.o inp.o rand.o buffer.o hard_limit.o epitaxy.o mesh.o patch.o cal_path.o log.o fx.o fit_sin.o
	./buildplugins.sh "$(opt_normal) $(debug_opt)" "$(platform)" "$(CC)" "$(LD)"
	./build_fit_plugins.sh $(platform)
	$(CC) main.c light_dump.o buffer.o light_utils.o light_test.o solver.o gui_hooks.o sim_find_n0.o sim_run.o newton_update.o pos.o inital.o advmath.o config.o plot.o timer.o memory.o dos.o dump_map.o dump_energy_slice.o gendosfdgaus.o exp.o pl.o time.o $(plugins) fast.o anal.o dump.o dump_config.o dump_1d_slice.o dump_dynamic.o ntricks.o ntricks_externalv.o dos_an.o startstop.o complex_solver.o thermal.o light_interface.o dump_ctrl.o inp.o rand.o hard_limit.o epitaxy.o mesh.o patch.o cal_path.o log.o fx.o fit_sin.o -o go.o -L. -lumfpack $(flags) $(link) $(inc)  -Wall -lm  -lcrypto $(opt_normal) $(llink)
# -lefence

.PHONY: install

install:
	
	mkdir $(DESTDIR)/usr
	mkdir $(DESTDIR)/usr/bin
	mkdir $(DESTDIR)/usr/share
	mkdir $(DESTDIR)/usr/share/opvdm
	mkdir $(DESTDIR)/usr/$(DEST_LIB)
	mkdir $(DESTDIR)/usr/$(DEST_LIB)/opvdm
	mkdir $(DESTDIR)/usr/$(DEST_LIB)/opvdm/light

	cp sim.opvdm $(DESTDIR)/usr/share/opvdm/
	cp README $(DESTDIR)/usr/share/opvdm/
	cp ./light/*.so $(DESTDIR)/usr/$(DEST_LIB)/opvdm/light/
	#cp ./exp $(DESTDIR)/usr/share/opvdm/ -r
	cp plot $(DESTDIR)/usr/share/opvdm/ -rf

	cp go.o $(DESTDIR)/usr/bin/opvdm_core


	chmod 755 $(DESTDIR)/usr/bin/opvdm_core
	chmod 0755 $(DESTDIR)/usr/share/opvdm/plot
	chmod 0644 $(DESTDIR)/usr/share/opvdm/plot/*

	#chmod 755 $(DESTDIR)/usr/share/opvdm/exp -R

	#now install the gui

	mkdir $(DESTDIR)/usr/share/applications
	mkdir $(DESTDIR)/usr/share/opvdm/gui
	mkdir $(DESTDIR)/usr/share/opvdm/images
	mkdir $(DESTDIR)/usr/share/mime
	mkdir $(DESTDIR)/usr/share/mime/packages 
	mkdir $(DESTDIR)/usr/share/icons
	mkdir $(DESTDIR)/usr/share/icons/gnome
	mkdir $(DESTDIR)/usr/share/icons/gnome/scalable
	mkdir $(DESTDIR)/usr/share/icons/gnome/scalable/mimetypes

	cp ./images/*.jpg $(DESTDIR)/usr/share/opvdm/images/
	cp ./images/*.png $(DESTDIR)/usr/share/opvdm/images/
	cp ./images/*.svg $(DESTDIR)/usr/share/opvdm/images/
	cp ./gui/*.py $(DESTDIR)/usr/$(DEST_LIB)/opvdm/
	cp ./gui/opvdm.desktop $(DESTDIR)/usr/share/applications/
	cp ./gui/opvdm-opvdm.xml $(DESTDIR)/usr/share/mime/packages/
	cp ./gui/application-opvdm.svg $(DESTDIR)/usr/share/icons/gnome/scalable/mimetypes/

	cp ./lang $(DESTDIR)/usr/share/opvdm/ -rf

	cp opvdm $(DESTDIR)/usr/bin/opvdm

	#man pages
	mkdir $(DESTDIR)/usr/share/man
	mkdir $(DESTDIR)/usr/share/man/man1
	cp ./man_pages/opvdm.1.gz $(DESTDIR)/usr/share/man/man1/
	cp ./man_pages/opvdm_core*.gz $(DESTDIR)/usr/share/man/man1/

	chmod 755 $(DESTDIR)/usr/bin/opvdm

	chmod 0644 $(DESTDIR)/usr/share/opvdm/images/image.jpg
	chmod 0644 $(DESTDIR)/usr/share/opvdm/images/icon.png
	chmod 0644 $(DESTDIR)/usr/share/opvdm/*.opvdm
	chmod 0644 $(DESTDIR)/usr/$(DEST_LIB)/opvdm/*.py
	chmod 0644 $(DESTDIR)/usr/share/applications/opvdm.desktop
	chmod 0644 $(DESTDIR)/usr/share/mime/packages/opvdm-opvdm.xml
	chmod 0644 $(DESTDIR)/usr/share/icons/gnome/scalable/mimetypes/application-opvdm.svg
	chmod 755 $(DESTDIR)/usr/$(DEST_LIB)/opvdm/opvdm.py
	chmod 755 $(DESTDIR)/usr/$(DEST_LIB)/opvdm/opvdm_zip.py

	#material files
	cp ./materials $(DESTDIR)/usr/share/opvdm/ -r
	find $(DESTDIR)/usr/share/opvdm/materials -type f -exec chmod 644 {} +
	#chmod 0644 $(DESTDIR)/usr/share/opvdm/materials -R



inp.o: inp.c
	$(CC) -c inp.c -o inp.o  $(inc) $(flags) $(opt_normal) $(warn)

gui_hooks.o: gui_hooks.c
	$(CC) -c gui_hooks.c -o gui_hooks.o  $(inc) $(flags) $(opt_normal) $(warn)

dump_ctrl.o: dump_ctrl.c
	$(CC) -c dump_ctrl.c -o dump_ctrl.o  $(inc) $(flags) $(opt_normal) $(warn)

stark_ntricks.o: stark_ntricks.c
	$(CC) -c stark_ntricks.c -o stark_ntricks.o  $(inc) $(flags)  $(opt_normal) $(warn)

light_utils.o: light_utils.c
	$(CC) -c light_utils.c -o light_utils.o  $(inc) $(flags) $(opt_normal) $(warn)

light_dump.o: light_dump.c
	$(CC) -c light_dump.c -o light_dump.o  $(inc) $(flags) $(opt_normal) $(warn)

light_test.o: light_test.c
	$(CC) -c light_test.c -o light_test.o  $(inc) $(flags) $(opt_normal) $(warn)

celiv_ntricks.o: celiv_ntricks.c
	$(CC) -c celiv_ntricks.c -o celiv_ntricks.o  $(inc) $(flags) $(opt_normal) $(warn)

tpc_ntricks.o: tpc_ntricks.c
	$(CC) -c tpc_ntricks.c -o tpc_ntricks.o  $(inc) $(flags) $(opt_normal) $(warn)

sim_find_n0.o: sim_find_n0.c
	$(CC) -c sim_find_n0.c -o sim_find_n0.o  $(inc) $(flags) $(opt_fast) $(warn)

sim_run.o: sim_run.c
	$(CC) -c sim_run.c -o sim_run.o  $(inc) $(flags) $(opt_fast) $(warn)

log.o: log.c
	$(CC) -c log.c -o log.o  $(inc) $(flags) $(opt_fast) $(warn)

newton_update.o: newton_update.c
	$(CC) -c newton_update.c -o newton_update.o  $(inc) $(flags) $(opt_fast) -Wall

dump_dynamic.o: dump_dynamic.c
	$(CC) -c dump_dynamic.c -o dump_dynamic.o  $(inc) $(flags) $(opt_normal) $(warn)

dump_1d_slice.o: dump_1d_slice.c
	$(CC) -c dump_1d_slice.c -o dump_1d_slice.o  $(inc) $(flags) $(opt_normal) $(warn)
thermal.o: thermal.c
	$(CC) -c thermal.c -o thermal.o  $(inc) $(flags) $(opt_normal) $(warn)

light_interface.o: light_interface.c
	$(CC) -c light_interface.c -o light_interface.o  $(inc) $(flags) $(opt_normal) $(warn)

render1d.o: render1d.c
	$(CC) -c render1d.c -o render1d.o  $(inc) $(flags) $(opt_normal) $(warn)

startstop.o: startstop.c
	$(CC) -c startstop.c -o startstop.o  $(inc) $(flags) $(opt_normal) $(warn)

dos_an.o: dos_an.c
	$(CC) -c dos_an.c -o dos_an.o  $(inc) $(flags) $(opt_normal) $(warn)


stark.o: stark.c
	$(CC) -c stark.c -o stark.o  $(inc) $(flags) $(opt_normal) $(warn)

ntricks.o: ntricks.c
	$(CC) -c ntricks.c -o ntricks.o  $(inc) $(flags) $(opt_normal) $(warn)

ntricks_externalv.o: ntricks_externalv.c
	$(CC) -c ntricks_externalv.c -o ntricks_externalv.o  $(inc) $(flags) $(opt_normal) $(warn)

celiv.o: celiv.c
	$(CC) -c celiv.c -o celiv.o  $(inc) $(flags) $(opt_normal) $(warn)

dump.o: dump.c
	$(CC) -c dump.c -o dump.o  $(inc) $(flags) $(opt_normal) $(warn)

dump_config.o: dump_config.c
	$(CC) -c dump_config.c -o dump_config.o  $(inc) $(flags) $(opt_normal) $(warn)

dump_energy_slice.o: dump_energy_slice.c
	$(CC) -c dump_energy_slice.c -o dump_energy_slice.o  $(inc) $(flags) $(opt_normal) $(warn)

dump_map.o: dump_map.c
	$(CC) -c dump_map.c -o dump_map.o  $(inc) $(flags) $(opt_normal) $(warn)

anal.o: anal.c
	$(CC) -c anal.c -o anal.o  $(inc) $(flags) $(opt_normal) $(warn)

tpc.o: tpc.c
	$(CC) -c tpc.c -o tpc.o  $(inc) $(flags) $(opt_normal) $(warn)


fast.o: fast.c
	$(CC) -c fast.c -o fast.o  $(inc) $(opt_fast) $(warn)

time.o: time.c
	$(CC) -c time.c -o time.o  $(inc) $(flags) $(opt_normal) $(warn)

fx.o: fx.c
	$(CC) -c fx.c -o fx.o  $(inc) $(flags) $(opt_normal) $(warn)

fit_sin.o: fit_sin.c
	$(CC) -c fit_sin.c -o fit_sin.o  $(inc) $(flags) $(opt_normal) $(warn)

exp.o: exp.c
	$(CC) -c exp.c -o exp.o  $(inc) $(flags) $(opt_normal) $(warn)

pl.o: pl.c
	$(CC) -c pl.c -o pl.o  $(inc) $(flags) $(opt_normal) $(warn)

gendosfdgaus.o: gendosfdgaus.c
	$(CC) -c gendosfdgaus.c -o gendosfdgaus.o  $(inc) $(flags) $(opt_normal) $(warn)

memory.o: memory.c
	$(CC) -c memory.c -o memory.o $(inc) $(flags) $(opt_normal) $(warn)

dos.o: dos.c
	$(CC) -c dos.c -o dos.o $(inc)  $(flags)  $(opt_fast) $(warn)
excite.o: excite.c
	$(CC) -c excite.c -o excite.o  $(inc) $(flags) $(opt_normal) $(warn)

solver.o: solver.c
	$(CC) -c solver.c -o solver.o  $(inc) $(flags) $(opt_normal) $(warn)

pos.o: pos.c
	$(CC) -c pos.c -o pos.o  $(inc) $(flags) $(opt_normal) $(warn)

inital.o: inital.c
	$(CC) -c inital.c -o inital.o  $(inc) $(flags) $(opt_normal) $(warn)

config.o: config.c
	$(CC) -c config.c -o config.o  $(inc) $(flags) $(opt_normal) $(warn)

plot.o: plot.c
	$(CC) -c plot.c -o plot.o  $(flags) $(inc) $(opt_normal) $(warn)

timer.o: timer.c
	$(CC) -c timer.c -o timer.o  $(flags) $(inc) $(opt_normal) $(warn)


advmath.o: advmath.c
	$(CC) -c advmath.c -o advmath.o  $(flags) $(inc) $(opt_slow) $(warn)

complex_solver.o: complex_solver.c
	$(CC) -c complex_solver.c -o complex_solver.o  $(flags) $(inc) $(opt_normal) $(warn)

buffer.o: buffer.c
	$(CC) -c buffer.c -o buffer.o  $(flags) $(inc) $(opt_normal) $(warn)

rand.o: rand.c
	$(CC) -c rand.c -o rand.o  $(flags) $(inc) $(opt_normal) $(warn)

hard_limit.o: hard_limit.c
	$(CC) -c hard_limit.c -o hard_limit.o  $(flags) $(inc) $(opt_normal) $(warn)

epitaxy.o: epitaxy.c
	$(CC) -c epitaxy.c -o epitaxy.o  $(flags) $(inc) $(opt_normal) $(warn)

mesh.o: mesh.c
	$(CC) -c mesh.c -o mesh.o  $(flags) $(inc) $(opt_normal) $(warn)

patch.o: patch.c
	$(CC) -c patch.c -o patch.o  $(flags) $(inc) $(opt_normal) $(warn)

cal_path.o: cal_path.c
	$(CC) -c cal_path.c -o cal_path.o  $(flags) $(inc) $(opt_normal) $(warn)

clean:
	./clean_all.sh

