mkdir pub
cp go.o ./pub/opvdm.exe
#openssl
cp libeay32.dll ./pub/
cp libgcc_s_sjlj-1.dll ./pub/
cp umfpack.dll ./pub/
cp crypto.dll ./pub/
cp zlib1.dll ./pub/
cp libzip-2.dll ./pub/

cp *.inp ./pub/

mkdir ./pub/phys
cp ./phys/*.spectra ./pub/phys/

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






