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
mkdir opvdm-core-${ver}
cp ./*.inp ./opvdm-core-${ver}/
cp ./sim.opvdm ./opvdm-core-${ver}/
cp ./plot ./opvdm-core-${ver}/ -rf
cp ./build.sh   ./opvdm-core-${ver}/ 
cp ./buildplugins.sh ./opvdm-core-${ver}/ 
cp ./build_fit_plugins.sh ./opvdm-core-${ver}/ 
cp ./get_elec_plugins.sh ./opvdm-core-${ver}/
cp ./license.txt ./opvdm-core-${ver}/
cp ./README ./opvdm-core-${ver}/

mkdir ./opvdm-core-${ver}/man_pages
cp ./man_pages/opvdm_core.1.gz ./opvdm-core-${ver}/man_pages/

mkdir ./opvdm-core-${ver}/gui
cp ./phys ./opvdm-core-${ver}/ -r
cp ./exp ./opvdm-core-${ver}/ -r

mkdir ./opvdm-core-${ver}/light
cp ./light/*.so ./opvdm-core-${ver}/light/

cp ./opvdm ./opvdm-core-${ver}/opvdm
cp ./*.c ./opvdm-core-${ver}/
cp ./*.h ./opvdm-core-${ver}/
cp ./makefile ./opvdm-core-${ver}/
cp ./plugins ./opvdm-core-${ver}/ -r
mkdir ./opvdm-core-${ver}/license

#cp ./opvdm-${ver}/* ./rpm/SOURCES/ -rf

tar -cf  ${rpmdir}/SOURCES/opvdm-core-${ver}.tar ./opvdm-core-${ver}/

rm ${rpmdir}/BUILDROOT -rf
mkdir ${rpmdir}/BUILDROOT

cat > ${rpmdir}/SPECS/opvdm-core.spec << EOF
# spec file for opvdm-core

Summary:		Organic solar cell device model (OPVDM)
License:		GPLv2+
Name:			opvdm-core
Version:		${ver}
Release:		3%{dist}
Source:			http://www.roderickmackenzie.eu/opvdm-core-${ver}.tar
Url:			http://www.opvdm.com
Group:			Development/Tools


BuildRequires: suitesparse-devel, zlib-devel, openssl-devel, gsl-devel, blas-devel, libcurl-devel, mencoder

#rpmbuild does not pick up gnuplot because it's called using popen
#there is no arch requirement is it is callued using popen
Requires: gnuplot

%description
Organic solar cell device model, is a drift-diffusion/Shockley-Read-Hall
solar cell simulator specifically developed for the simulation of organic
solar cells.  It can simulate light/dark JV curves, charge extraction data
and provide information on average recombination rates (tau) as would be
measured from transient photo-voltage experiments.



%prep
%setup -q

%build
make %{?_smp_mflags} OPT_FLAGS="%{optflags}" OPT_ARCH=%{_arch}

%install
make  DEST_DIR=%{buildroot} DEST_LIB=%{_lib} install


%files
%{_bindir}/opvdm_core
%{_libdir}/opvdm/

%{_datadir}/opvdm/
%{_mandir}/man1/opvdm*

%doc README
%doc license.txt

%changelog
* Sun Mar 30 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-3
  - tab.py can now do comboboxes
  - removed mod function
* Sat Mar 29 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-2
  - Added virtual terminal tab
  - Consolidated gui functions startting and stopping the simulation
  - Added the –lock command line option to opvdm_core
  - move the gui_hooks to main and out of the plugins
  - Auto switching to terminal tab when running
* Fri Mar 22 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-1
  - Bug fixes in rpm build for new release. 
* Fri Mar 21 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.59-1
  - Optical model now dumps to a zip file to save inodes.
* Thu Mar 20 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.58-1
  - Removed the GUI from this rpm leaving just opvdm_core and the input files.
* Sun Mar 16 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.57-1
  - Fixed broken find_voc function
* Mon Feb 10 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.56-1
  - Updated archive import function to handle compressed archives
  - Updated rpm building tools to handle new compressed archives
* Sat Feb 08 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.55-1
  - Started moving input files into archive.
  - dynamic files placed in own directory
  - files that no longer exist are automaticly removed from used file menu.
  - scan_tab opendialog opens in correct directory.
* Mon Jan 20 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.54-1
  - Fixed bug in the close tab button reported by Simon Schmeisser.
  - Fixed bug in rpm_build script which damaged the optical model code
  - removed opvdm_import
* Sun Jan 19 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.53-1
  - Made the about window display the correct version number
  - removed simulation_dir variable from opvdm, fixed more
  - removed opvdm_clone replaced with opvdm --clone
  - Fixed missing icon in optical simulation window
* Fri Jan 17 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.52-1
  - removed bash script opvdm_dump and added option --dump-tab to opvdm
  - made tool bar show before material parameter window
  - disabled menu callbacks during load
  - improved the import function so it tries to import a simulation even if it an old version
  - made version numbers between gui and core the same
* Wed Jan 15 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.5-2  
  - Windows now remember where they are put after application shutdown.
  - Fixed loading bugs
  - Added logging
* Tue Jan 14 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.5-1  
  - Made the optical model dynamicly linked rather than statically linked
  - Improved plotting of single point data, i.e FF, Voc and Jsc.
  - Added window to analyze simulation data
  - More meaningful icons
  - Unified progress bar/spinner widgets
  - Moved optical output into one directory
  - Parameter scan window can now do multiple simulations
  - Parameter scan window can deal with multiple CPUs/cores
  - Improved parameter scan window GUI
  - Mesh editor in it's own window
* Thu Nov 14 2013 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.4-1  
  - Added export material parameters to pdf/tar.gz/jpg file
  - Import material parameters from tar.gz file 
  - Parameter scan window updated and fixed
  - 32 bit compatibility fixed
* Sat Nov 09 2013 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.2-1  
  - Improved gui
  - Better error handeling
  - Fixed rpm installer bugs
* Fri Oct 18 2013 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.1-1
  - Fixed memory leeks in opvdm_core
  - Fixed bugs in free-free recombination calculation that made solver crash wheh it was turned off
  - Made structure of input files more logical  
  - Improved gui
  - Better intergration to x-desktop
* Fri Sep 06 2013 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.0-2
  - Edits to conform to fedora requirements as suggested by Christopher Meng/Michael Schwendt.
* Wed Sep 04 2013 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.0-1
  -  Original submission to bugzilla
EOF

cd ${rpmdir}

rpmbuild -v --target ${mytarget} -ba --clean ./SPECS/opvdm-core.spec

cp ./SRPMS/*.rpm ~/webpage/
cp ./RPMS/${mytarget}/*.rpm ~/webpage/
cp ./SOURCES/*.tar ~/webpage/
#cp ./SPECS/* ~/webpage/

mkdir ~/yum
mkdir ~/yum/repo
cp ./RPMS/${mytarget}/* ~/yum/repo/
cp ./SRPMS/* ~/yum/repo/
cd $mydir

cp ~/rpmbuild/RPMS/${mytarget}/opvdm-core-2*.rpm ../
