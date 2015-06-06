mkdir pub
cp go.o ./pub/opvdm.exe
#openssl
cp libeay32.dll ./pub/
cp libgcc_s_sjlj-1.dll ./pub/
cp umfpack.dll ./pub/
cp crypto.dll ./pub/
cp zlib1.dll ./pub/
cp libzip-2.dll ./pub/

cp dump.inp ./pub/
cp ver.inp ./pub/
cp device_epitaxy.inp ./pub/
cp dos0.inp ./pub/
cp sim.inp ./pub/
cp device.inp ./pub/
cp math.inp ./pub/
cp thermal.inp ./pub/
cp light.inp ./pub/
cp optics_epitaxy.inp ./pub/
cp optics.inp ./pub/
cp physdir.inp ./pub/
cp time_mesh.inp ./pub/
cp jv.inp ./pub/
cp sim_menu.inp ./pub/

mkdir ./pub/phys
cp ./phys/*.inp ./pub/phys/

cp ./phys/ito_hoke ./pub/phys/ -r
cp ./phys/pedotpss ./pub/phys/ -r
cp ./phys/1e7 ./pub/phys/ -r
cp ./phys/p3htpcbm ./pub/phys/ -r
cp ./phys/al ./pub/phys/ -r

#python -m py_compile ./gui/*.py

mkdir ./pub/gui
cp ./gui/*.ico ./pub/gui/
cp ./gui/*.jpg ./pub/gui/
cp ./gui/*.png ./pub/gui/
cp ./gui/*.py ./pub/gui/

cp ./opvdm ./pub/opvdm.py
cp ./go.o ./pub/opvdm.exe
cp ./sim.opvdm ./pub/

mkdir ./pub/light
cp ./light/*.dll ./pub/light/

cp pub ~/windows/share/ -rf






