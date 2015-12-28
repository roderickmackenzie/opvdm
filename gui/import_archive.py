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
import pygtk
from win_lin import running_on_linux
import os
import shutil
import signal
import subprocess
from scan_io import get_scan_dirs 
from inp import inp_update_token_value
import os, fnmatch
import stat 
import zipfile
from util import copy_scan_dir
from util import delete_second_level_link_tree
from util_zip import read_lines_from_archive
from inp import inp_search_token_value
from inp import inp_merge
from util_zip import write_lines_to_archive
import tempfile
from util_zip import zip_lsdir
from inp import inp_issequential_file

def copy_check_ver(dest_archive,src_archive,file_name,only_over_write,clever):
	if dest_archive==src_archive:
		print "I can't opperate on the same .opvdm file"
		return
	#remove the dest file if both exist ready to copy
	do_copy=True
	src_ver=""
	dest_ver=""
	orig_exists=False
	dest_exists=False
	src_lines=[]
	dest_lines=[]

	orig_exists=read_lines_from_archive(src_lines,src_archive,file_name)

	if orig_exists==True:
		src_ver=inp_search_token_value(src_lines, "#ver")
	else:
		print "Warning: ",src_archive,file_name," no origonal file to copy"
		return

	#read in the dest file where ever it may be
	dest_exists=read_lines_from_archive(dest_lines,dest_archive,file_name)

	if dest_exists==True:
		dest_ver=inp_search_token_value(dest_lines, "#ver")

		
	#if we are only over writing only copy if dest file exists
	if (only_over_write==True):
		if dest_exists==True:
			do_copy=True
		else:
			print "Warning: ", file_name," - only exists in the source"
			do_copy=False
			return

	if dest_exists==True:
		if src_ver!=dest_ver:
			print "Warning: Verstion numbers do not match for files",dest_archive,src_archive,file_name
			print "src_ver=",src_ver,"dest ver=",dest_ver

			if clever==False:
				print "Not copying that file you will have to deal that with by hand"
				return

	if clever==True and dest_exists==True:
		errors=inp_merge(dest_lines,src_lines) 
		if len(errors)!=0:
			print "File ",file_name,errors
	else:
		dest_lines=src_lines

	if (do_copy==True):
		write_lines_to_archive(dest_archive,file_name,dest_lines)


def import_archive(src_archive,dest_archive,only_over_write):
	if src_archive.endswith('.opvdm')==False:
		print "I can only import from .opvdm files"
		return

	if dest_archive.endswith('.opvdm')==False:
		print "I can only import to .opvdm files"
		return

	src_dir=os.path.dirname(src_archive)
	dest_dir=os.path.dirname(dest_archive)
	files=[ "sim.inp", "device.inp", "stark.inp" ,"shg.inp"   ,"jv.inp" , "optics.inp", "math.inp",  "dump.inp" , "light.inp", "server.inp", "light_exp.inp" ]

	for my_file in files:
		print "Importing",my_file,"to",dest_archive
		copy_check_ver(dest_archive,src_archive,my_file,only_over_write,True)

	files=["info.inp"]

	print src_archive
	ls=zip_lsdir(src_archive)
	for i in range(0,len(ls)):
		if inp_issequential_file(ls[i],"dos"):
			files.append(ls[i])

		if inp_issequential_file(ls[i],"pl"):
			files.append(ls[i])

		if inp_issequential_file(ls[i],"time_mesh_config"):
			files.append(ls[i])

		if inp_issequential_file(ls[i],"pulse"):
			files.append(ls[i])

	for my_file in files:
		print "Importing",my_file,"to",dest_archive
		copy_check_ver(dest_archive,src_archive,my_file,False,True)

	files=[ "epitaxy.inp", "fit.inp", "constraints.inp","duplicate.inp", "thermal.inp","time_mesh_config.inp","mesh.inp" ]

	ls=zip_lsdir(src_archive)
	for i in range(0,len(ls)):
		if inp_issequential_file(ls[i],"time_mesh"):
			files.append(ls[i])

		if inp_issequential_file(ls[i],"homo"):
			files.append(ls[i])

		if inp_issequential_file(ls[i],"lumo"):
			files.append(ls[i])

	for my_file in files:
		print "Importing",my_file
		copy_check_ver(dest_archive,src_archive,my_file,only_over_write,False)

	import_scan_dirs(dest_dir,src_dir)

def import_scan_dirs(dest_dir,src_dir):
	sim_dirs=[]
	get_scan_dirs(sim_dirs,src_dir)
	for my_file in sim_dirs:
		dest=os.path.join(dest_dir,os.path.basename(my_file))
		print "copy scan dir",my_file,"to",dest

		if os.path.exists(dest):
			delete_second_level_link_tree(dest)

		copy_scan_dir(dest,my_file)

def clean_scan_dirs(path):
	sim_dirs=[]
	get_scan_dirs(sim_dirs,path)

	for my_dir in sim_dirs:
		print "cleaning ",my_dir
		files = os.listdir(my_dir)
		for file in files:
			file_name=os.path.join(my_dir,file)
			if file_name.endswith(".dat"):
				print "Remove",file_name
				os.remove(file_name)
			if os.path.isdir(file_name):
				print "remove dir",file_name
				shutil.rmtree(file_name)



