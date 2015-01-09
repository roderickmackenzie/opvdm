#!/bin/bash
#    Organic Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for organic solar cells. 
#    Copyright (C) 2012 Roderick C. I. MacKenzie
#
#	roderick.mackenzie@nottingham.ac.uk
#	www.roderickmackenzie.eu
#	Room B86 Coats, University Park, Nottingham, NG7 2RD, UK
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

mydir=`pwd`
for i in `find ./plugins/ |grep makefile$`; do
newdir=`dirname $i`
echo "Building plugin" $newdir
cd $newdir
make opt_normal="$1" OPT_ARCH="$2"
if [ $? -ne 0 ]; then
	exit
fi
cd $mydir

done

echo -n >sim_menu.inp
for i in `find ./plugins/ |grep gui_info.inp$`; do
cat $i >>sim_menu.inp
done

for i in `find ./light/ |grep makefile$`; do
newdir=`dirname $i`
echo "Building optical plugin" $newdir
cd $newdir
make opt_normal="$1" OPT_ARCH="$2"
nowdir=`pwd`
curname=`basename $nowdir` 
cp plugin.so ../${curname}.so
if [ $? -ne 0 ]; then
	exit
fi
cd $mydir

done
