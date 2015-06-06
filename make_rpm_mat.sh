#!/bin/bash -x
unzip -p sim.opvdm ver.inp >data.dat
ver=`cat data.dat|sed -n 6p`
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
mkdir opvdm-mat-${ver}
cp ./license.txt ./opvdm-mat-${ver}/
cp ./README ./opvdm-mat-${ver}/

mkdir ./opvdm-mat-${ver}/man_pages
cp ./man_pages/opvdm.1.gz ./opvdm-mat-${ver}/man_pages/

mkdir ./opvdm-mat-${ver}/phys
cp ./phys/* ./opvdm-mat-${ver}/phys/ -rf

cp ./makefile ./opvdm-mat-${ver}/
mkdir ./opvdm-mat-${ver}/license

#cp ./opvdm-${ver}/* ./rpm/SOURCES/ -rf

tar -cf  ${rpmdir}/SOURCES/opvdm-mat-${ver}.tar ./opvdm-mat-${ver}/

rm ${rpmdir}/BUILDROOT -rf
mkdir ${rpmdir}/BUILDROOT

cat > ${rpmdir}/SPECS/opvdm-mat.spec << EOF
# spec file for opvdm

Summary:		Organic solar cell device model material models (OPVDM)
License:		Proprietary
Name:			opvdm-mat 
Version:		${ver}
Release:		3%{dist}
Source:			http://www.roderickmackenzie.eu/opvdm-mat-${ver}.tar
Url:			http://www.opvdm.com
Group:			Development/Tools


#rpmbuild does not pick up gnuplot because it's called using popen
#there is no arch requirement is it is callued using popen
#nor does rpm build pick up numpty of matplotlib 
Requires: opvdm-core >= 2.52

%description
Material parameters for OPVDM

%prep
%setup -q

%build

%install
make  DEST_DIR=%{buildroot} DEST_LIB=%{_lib} install_mat



%files
%{_datadir}/opvdm/

%doc README
%doc license.txt

%changelog
* Sun May 25 2014  Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 1.0-1
  -  Moved materials data into it's own rpm.
EOF

cd ${rpmdir}

rpmbuild -v --target ${mytarget} -ba --clean ./SPECS/opvdm-mat.spec

cp ./SRPMS/*.rpm ~/webpage/
cp ./RPMS/${mytarget}/*.rpm ~/webpage/
cp ./SOURCES/*.tar ~/webpage/
#cp ./SPECS/* ~/webpage/

mkdir ~/yum
mkdir ~/yum/repo
cp ./RPMS/${mytarget}/* ~/yum/repo/
cp ./SRPMS/* ~/yum/repo/
cd $mydir

cp ~/rpmbuild/RPMS/${mytarget}/opvdm-mat-*.rpm ../
