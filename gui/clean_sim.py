#    Organic Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for organic solar cells. 
#    Copyright (C) 2012 Roderick C. I. MacKenzie
#
#	roderick.mackenzie@nottingham.ac.uk
#	www.opvdm.com
#	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License v2.0, as published by
#    the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import sys
import os
import shutil
import glob
from scan_io import delete_scan_dirs

def clean_sim_dir():
	print "Deleting scan dirs"
	delete_scan_dirs(os.getcwd())

	del_file=os.path.join(os.getcwd(),"pub")
	if os.path.isdir(del_file):
		print "Deleteing "+del_file
		shutil.rmtree(del_file)

	del_file=os.path.join(os.getcwd(),"dynamic")
	if os.path.isdir(del_file):
		print "Deleteing "+del_file
		shutil.rmtree(del_file)

	del_file=os.path.join(os.getcwd(),"snapshots")
	if os.path.isdir(del_file):
		print "Deleteing "+del_file
		shutil.rmtree(del_file)

	files = os.listdir(os.getcwd())
	for file in files:
		remove=False
		if file.endswith(".txt"):
			remove=True
		if file.endswith("~"):
			remove=True
		if file.endswith(".dat"):
			remove=True
		if file.endswith(".o"):
			remove=True
		if file.endswith(".orig"):
			remove=True
		if file.endswith(".back"):
			remove=True
		if file.endswith(".bak"):
			remove=True
		if file.endswith("gmon.out"):
			remove=True
		if remove==True:
			print "Deleting",file
			os.remove(file)
