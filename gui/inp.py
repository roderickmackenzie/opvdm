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
import signal
from util_zip import replace_file_in_zip_archive
import subprocess
from tempfile import mkstemp
import logging
import zipfile
from util_zip import zip_remove_file
from util_zip import write_lines_to_archive
from util_zip import read_lines_from_archive
from util_zip import archive_isfile
from util_zip import zip_lsdir

def inp_lsdir():
	return zip_lsdir("sim.opvdm")

def inp_remove_file(file_name):
	zip_remove_file("sim.opvdm",file_name)

def inp_read_next_item(lines,pos):
	token=lines[pos]
	pos=pos+1
	value=lines[pos]
	pos=pos+1
	return token,value,pos

def inp_update_token_value(file_path, token, replace,line_number):
	lines=[]
	if token=="#Tll":
		inp_update_token_value("thermal.inp", "#Tlr", replace,1)
		inp_update_token_value("dos0.inp", "#Tstart", replace,1)
		try:
			upper_temp=str(float(replace)+5)
		except:
			upper_temp="300.0"
		inp_update_token_value("dos0.inp", "#Tstop", upper_temp,1)

	path=os.path.dirname(file_path)

	zip_file_name=os.path.join(path,"sim.opvdm")

	read_lines_from_archive(lines,zip_file_name,os.path.basename(file_path))

	for i in range(0, len(lines)):
		if lines[i]==token:
			lines[i+line_number]=replace
			break


	if os.path.isfile(file_path):
		fh, abs_path = mkstemp()

		dump='\n'.join(lines)

		#for item in lines:
		#	dump=dump+item+"\n"

		dump=dump.rstrip("\n")
		f=open(abs_path, mode='wb')
		lines = f.write(dump)
		f.close()

		os.close(fh)
		shutil.move(abs_path, file_path)
	else:
		replace_file_in_zip_archive(zip_file_name,os.path.basename(file_path),lines)

def inp_isfile(file_path):

	zip_file_name=os.path.join(os.path.dirname(file_path),"sim.opvdm")
	return archive_isfile(zip_file_name,os.path.basename(file_path))

def inp_copy_file(dest,src):
	lines=[]
	if inp_load_file(lines,src)==True:
		inp_write_lines_to_file(dest,lines)
		True
	else:
		return False

def inp_load_file(lines,file_path):
	zip_file_path=os.path.join(os.path.dirname(file_path),"sim.opvdm")
	file_name=os.path.basename(file_path)
	return read_lines_from_archive(lines,zip_file_path,file_name)
	
def inp_write_lines_to_file(file_path,lines):
	archive_path=os.path.join(os.path.dirname(file_path),"sim.opvdm")
	file_name=os.path.basename(file_path)
	write_lines_to_archive(archive_path,file_name,lines)

def inp_save_lines(file_path,lines):
	dump=""
	for item in lines:
		dump=dump+item+"\n"

	dump=dump.rstrip("\n")

	f=open(file_path, mode='wb')
	lines = f.write(dump)
	f.close()

def inp_search_token_value(lines, token):

	for i in range(0, len(lines)):
		if lines[i]==token:
			return lines[i+1]

	return False

def inp_search_token_value_multiline(lines, token):
	ret=[]
	for i in range(0, len(lines)):
		if lines[i]==token:
			pos=i+1
			while (lines[pos][0]!="#"):
				ret.append(lines[pos])
				pos=pos+1

			return ret

	return False

def inp_get_next_token_array(lines,pos):

	ret=[]
	ret.append(lines[pos])
	pos=pos+1
	while (lines[pos][0]!="#"):
		ret.append(lines[pos])
		pos=pos+1

	return ret,pos

def inp_get_token_array(file_path, token):

	lines=[]
	ret=[]
	inp_load_file(lines,file_path)

	for i in range(0, len(lines)):
		if lines[i]==token:
			pos=i+1
			while (lines[pos][0]!="#"):
				ret.append(lines[pos])
				pos=pos+1

			return ret

	return False

def inp_get_token_value(file_path, token):

	lines=[]
	inp_load_file(lines,file_path)

	for i in range(0, len(lines)):
		if lines[i]==token:
			return lines[i+1]

	return "0"

def inp_sum_items(lines,token):
	my_sum=0.0
	for i in range(0, len(lines)):
		if lines[i].startswith(token)==True:
			my_sum=my_sum+float(lines[i+1])

	return my_sum

def inp_merge(dest,src):
	ret=[]
	for i in range(0,len(dest)):
		if dest[i].startswith("#") and dest[i]!="#ver" and dest[i]!="#end":
			lookfor=dest[i]
			found=False
			for ii in range(0,len(src)):
				if src[ii]==lookfor:
					#print "Found",dest_lines[i],orig_lines[ii]
					dest[i+1]=src[ii+1]
					found=True
					break
			if found==False:
				ret.append("Warning: token "+lookfor+" not found in archive")

	return ret

