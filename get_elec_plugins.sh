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
for i in `find ./plugins/ |grep build.inp$`; do
base=`dirname $i`
echo -n $base/plugin.o " "
done

cat ./license/add.txt >elec_plugins.h
for i in `find ./plugins/ |grep "\.h"$`; do
echo "#include \""$i"\"" >>elec_plugins.h
done

echo -n >run_list.c
for i in `find ./plugins/ |grep "run_list.c"$`; do
cat $i  >>run_list.c
echo "else">>run_list.c
done
echo "{" >>run_list.c
echo "printf(\"Mode not recognized\n\");">>run_list.c
echo "}" >>run_list.c

echo -n >main_args.c
for i in `find ./plugins/ |grep "main_args_first.c"$`; do
cat $i  >>main_args.c
done

for i in `find ./plugins/ |grep "main_args.c"$`; do
cat $i  >>main_args.c
done

