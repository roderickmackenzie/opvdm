#!/bin/bash -x
unzip -p sim.opvdm ver.inp >data.dat
ver=`cat data.dat|sed -n 2p`
dist=fc19
mydir=`pwd`
rpmdir=~/rpmbuild
mytarget=x86_64
#i686
#make clean 
#make
rm ${rpmdir}/BUILD ${rpmdir}/SOURCES ${rpmdir}/SPECS -rf
mkdir ${rpmdir} 
cd ${rpmdir} 
mkdir BUILD RPMS SOURCES SPECS SRPMS
cd $mydir
mkdir opvdm-gui-${ver}
cp ./license.txt ./opvdm-gui-${ver}/
cp ./README ./opvdm-gui-${ver}/

mkdir ./opvdm-gui-${ver}/man_pages
cp ./man_pages/opvdm.1.gz ./opvdm-gui-${ver}/man_pages/

mkdir ./opvdm-gui-${ver}/gui
cp ./gui/*.jpg ./opvdm-gui-${ver}/gui/
cp ./gui/*.png ./opvdm-gui-${ver}/gui/
cp ./gui/*.pyc ./opvdm-gui-${ver}/gui/
cp ./gui/opvdm.desktop ./opvdm-gui-${ver}/gui/
cp ./gui/opvdm-opvdm.xml ./opvdm-gui-${ver}/gui/
cp ./gui/application-opvdm.svg ./opvdm-gui-${ver}/gui/

cp ./opvdm ./opvdm-gui-${ver}/opvdm
cp ./makefile ./opvdm-gui-${ver}/
mkdir ./opvdm-gui-${ver}/license

#cp ./opvdm-${ver}/* ./rpm/SOURCES/ -rf

tar -cf  ${rpmdir}/SOURCES/opvdm-gui-${ver}.tar ./opvdm-gui-${ver}/

rm ${rpmdir}/BUILDROOT -rf
mkdir ${rpmdir}/BUILDROOT

cat > ${rpmdir}/SPECS/opvdm-gui.spec << EOF
# spec file for opvdm

Summary:		Organic solar cell device model GUI (OPVDM)
License:		Proprietary
Name:			opvdm-gui 
Version:		${ver}
Release:		3%{dist}
Source:			http://www.roderickmackenzie.eu/opvdm-gui-${ver}.tar
Url:			http://www.opvdm.com
Group:			Development/Tools


#BuildRequires: suitesparse-devel, zlib-devel, openssl-devel, gsl-devel, blas-devel, libcurl-devel, mencoder

#rpmbuild does not pick up gnuplot because it's called using popen
#there is no arch requirement is it is callued using popen
#nor does rpm build pick up numpty of matplotlib 
Requires: gnuplot, numpy, python-matplotlib, texlive, ghostscript, ImageMagick, python-inotify, opvdm-core >= 2.52

%description
GUI for Organic solar cell device model, is a drift-diffusion/Shockley-Read-Hall
solar cell simulator specifically developed for the simulation of organic
solar cells.  It can simulate light/dark JV curves, charge extraction data
and provide information on average recombination rates (tau) as would be
measured from transient photo-voltage experiments.


%prep
%setup -q

%build

%install
make  DEST_DIR=%{buildroot} DEST_LIB=%{_lib} install_gui



%files
%{_bindir}/opvdm
%{_libdir}/opvdm/

%{_datadir}/opvdm/
%{_datadir}/applications/opvdm.desktop
%{_datadir}/mime/packages/opvdm-opvdm.xml
%{_datadir}/icons/gnome/scalable/mimetypes/application-opvdm.svg
%{_mandir}/man1/opvdm*

%doc README
%doc license.txt

%changelog
* Thu Mar 20 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.58-1
  - Removed the GUI from this rpm leaving just opvdm_core and the input files.
EOF

cd ${rpmdir}

rpmbuild -v --target ${mytarget} -ba --clean ./SPECS/opvdm-gui.spec

cp ./SRPMS/*.rpm ~/webpage/
cp ./RPMS/${mytarget}/*.rpm ~/webpage/
cp ./SOURCES/*.tar ~/webpage/
#cp ./SPECS/* ~/webpage/

mkdir ~/yum
mkdir ~/yum/repo
cp ./RPMS/${mytarget}/* ~/yum/repo/
cp ./SRPMS/* ~/yum/repo/
cd $mydir

cp ~/rpmbuild/RPMS/${mytarget}/opvdm-gui-2*.rpm ../
