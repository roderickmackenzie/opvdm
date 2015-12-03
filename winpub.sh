mkdir pub
cp go.o ./pub/opvdm_core.exe
#openssl ~/windll/
cp libeay32.dll ./pub/
cp libgcc_s_sjlj-1.dll ./pub/
cp umfpack.dll ./pub/
cp crypto.dll ./pub/
cp zlib1.dll ./pub/
cp libzip-2.dll ./pub/


mkdir ./pub/phys
cp ./phys/*.spectra ./pub/phys/

cp ./phys/ito ./pub/phys/ -r
cp ./phys/npb ./pub/phys/ -r
cp ./phys/generic_organic ./pub/phys/ -r
cp ./phys/generic_metal ./pub/phys/ -r
cp ./phys/pedotpss ./pub/phys/ -r
cp ./phys/p3htpcbm ./pub/phys/ -r
cp ./phys/al ./pub/phys/ -r
cp ./phys/ag ./pub/phys/ -r
cp ./phys/au ./pub/phys/ -r

cp ./device_lib ./pub/device_lib -r

#python -m py_compile ./gui/*.py

mkdir ./pub/gui
cp ./gui/*.ico ./pub/gui/
cp ./gui/*.jpg ./pub/gui/
cp ./gui/*.png ./pub/gui/
cp ./gui/*.py ./pub/gui/

cp ./sim.opvdm ./pub/

mkdir ./pub/light
cp ./light/*.dll ./pub/light/

cp pub ~/windows/share/ -rf






