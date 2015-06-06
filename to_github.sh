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
kiallmy_dir=`pwd`
files="*.c *.h makefile *.sh"
dest=/home/rod/webpage/git/opvdm/opvdm/

read -p "What happened : " message

for i in $files
do
	cp $i $dest
done

cp plugins $dest/ -r
mkdir $dest/light
cp ./light/exp $dest/light/ -r
cp ./light/flat $dest/light/ -r

cd $dest

for i in $files
do
git add $i
done
git commit -m "$message"
git push origin master --force
cd $my_dir
