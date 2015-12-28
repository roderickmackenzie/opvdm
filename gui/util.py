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
import subprocess
from tempfile import mkstemp
import logging
import zipfile
import re
from numpy import zeros
import hashlib
import glob
from win_lin import running_on_linux
from cal_path import get_inp_file_path
from util_zip import zip_get_data_file

def gui_print_path(text,path,length):
	remove=len(text)+len(path)-length
	if remove>0:
		ret=text+path[remove:]
	else:
		ret=text+path

	return ret


	
def read_data_2d(x_scale,y_scale,z,file_name):
	found,lines=zip_get_data_file(file_name)
	if found==True:
		x_max=0
		y_max=0
		y_pos=0
		z_store=[]
		for i in range(0, len(lines)):
			if (lines[i][0]!="#"):
				if lines[i]!="\n":

					temp=lines[i]
					temp=re.sub(' +',' ',temp)
					temp=re.sub('\t',' ',temp)
					temp=temp.rstrip()
					sline=temp.split(" ")
					if len(sline)==3:
						if x_max==0:
							y_scale.append(float(lines[i].split(" ")[1]))
						if y_pos==0:
							x_scale.append(float(lines[i].split(" ")[0]))

						z_store.append(float(lines[i].split(" ")[2]))
					y_pos=y_pos+1

					if x_max==0:
						y_max=y_max+1

				if lines[i]=="\n":
					x_max=x_max+1
					y_pos=0

		if  lines[len(lines)-1]!="\n":
			x_max=x_max+1

		#print "load 3d data",x_scale,y_scale

		pos=0
		for x in range(0, x_max):
			z.append([])
			for y in range(0, y_max):
				z[x].append(z_store[pos])
				pos=pos+1
		return True
	else:
		return False

def get_cache_path(path):
	m = hashlib.md5()
	m.update(path)
	cache_file=m.hexdigest()
	cache_path = os.path.expanduser("~")+"/cache/"+cache_file
	return cache_path

def copy_scan_dir(new_dir,old_dir):
	print "trying to copy",old_dir,new_dir
	if not os.path.exists(new_dir):
		os.makedirs(new_dir)
	for filename in glob.glob(os.path.join(old_dir, '*.*')):
		if os.path.isfile(filename):
			shutil.copy(filename, new_dir)

def delete_second_level_link_tree(path):
	for filename in os.listdir(path):
		full_name=os.path.join(path,filename)
		if os.path.isdir(full_name):
			print "Deleteing",full_name
			opvdm_delete_file(full_name)

	opvdm_delete_file(path)

def opvdm_delete_file(path):
	if os.path.islink(path):
		real_path=os.path.realpath(path)
		print "Deleting link:",path
		os.unlink(path)
		print "check",real_path,get_cache_path(path)
		if real_path==get_cache_path(path):	#only delete the cache directory if it is the one we are intending to delete 

			if os.path.isdir(real_path)==True:
				print "Delete",real_path
				if real_path!="/":
					if real_path!="/home/rod":
						if real_path!="/home/rod/":
							shutil.rmtree(real_path)

			elif os.path.isfile(real_path)==True:
				print "Delete",real_path
				os.remove(real_path)
	else:
		if os.path.isdir(path)==True:
			print "Delete",path
			shutil.rmtree(path)
		elif os.path.isfile(path)==True:
			print "Delete",path
			os.remove(path)

def numbers_to_latex(data):
	out=""
	number=False
	open_ten=False
	for i in range(0,len(data)):
		if str.isdigit(data[i])==True and number==False:
			out=out+""#$
			number=True

		if number==True:
			add=data[i]

			if number==True:
				if data[i]=="e":
					add="\\times10^{" 
					open_ten=True
				if str.isdigit(data[i])==True:
					add=data[i]
				else:
					if data[i]!="e" and data[i]!="-" and data[i]!="+" and data[i]!=".": 
						number=False
						add=""+data[i] #$
						if open_ten==True:
							add="}"+data[i] #$
							open_ten=False
			out=out+add
		else:
			out=out+data[i]
	if open_ten==True:
		out=out+"}"#$
		number=False

	if number==True:
		out=out+"" #$

	return out

def str2bool(v):
	if type(v) is bool:
		return v
	else:
		return v.lower() in ("yes", "true", "t", "1")

def pygtk_to_latex_subscript(in_string):
	out_string=in_string.replace("<sub>","_{")
	out_string=out_string.replace("</sub>","}")
	out_string=out_string.replace("<sup>","^{")
	out_string=out_string.replace("</sup>","}")
	return out_string

def latex_to_pygtk_subscript(in_string):
	out=re.compile(r"_\{([^\]]*?)\}").sub("<sub>\\1</sub>", in_string)
	out=re.compile(r"\^\{([^\]]*?)\}").sub("<sup>\\1</sup>", out)
	return out

def lines_to_xyz(x,y,z,lines):
	for i in range(0, len(lines)):
		lines[i]=re.sub(' +',' ',lines[i])
		lines[i]=re.sub('\t',' ',lines[i])
		lines[i]=lines[i].rstrip()
		sline=lines[i].split(" ")
		if len(sline)==2:
			if (lines[i][0]!="#"):
				x.append(float(sline[0]))
				y.append(float(sline[1]))
				z.append("")
		if len(sline)==3:
			if (lines[i][0]!="#"):
				x.append(float(sline[0]))
				y.append(float(sline[1]))
				z.append(sline[2])

def read_xyz_data(x,y,z,file_name):
	found,lines=zip_get_data_file(file_name)
	if found==True:
		lines_to_xyz(x,y,z,lines)
		#print "here z=,",z,x,file_name
		return True
	else:
		return False



def time_with_units(time):
	ret=str(time)
	mul=1.0
	if (time<1000e-15):
		ret="fs"
		mul=1e15
	elif (time<1000e-12):
		ret="ps"
		mul=1e12
	elif (time<1000e-9):
		ret="ns"
		mul=1e9
	elif (time<1000e-6):
		ret="us"
		mul=1e6
	elif (time<1000e-3):
		ret="ms"
		mul=1e3
	else:
		ret="s"
		mul=1.0
	return mul,ret

def fx_with_units(fx):
	ret=str(fx)
	if (fx<1e3):
		ret=str(fx*1)+" Hz"
	elif (fx<1e6):
		ret=str(fx/1e3)+" kHz"
	elif (fx<1e9):
		ret=str(fx/1e6)+" MHz"

	return ret


def pango_to_gnuplot(data):
	one=""
	data.replace("<sub>", "_{")
	data.replace("</sub>", "}")

def opvdm_copy_src(new_dir):
	pwd=os.getcwd()
	file_list=glob.glob(os.path.join(pwd,"*"))

	if not os.path.exists(new_dir):
		os.makedirs(new_dir)
	print file_list
	for name in file_list:
		gui_file_name=os.path.join(name,"opvdm_gui_config.inp")
		
		if os.path.isfile(gui_file_name)==False:
			fname=os.path.basename(name)
			out=os.path.join(new_dir,fname)
			if os.path.isfile(name):
				 shutil.copy(name, out)
			else:
				print "Copy dir:", name
				shutil.copytree(name, out,symlinks=True)
		else:
			print "I will not copy",name





def opvdm_clone():
	src=get_inp_file_path()
	source = os.listdir(src)
	pwd=os.getcwd()
	destination=pwd
	for files in source:
		if files.endswith(".inp"):
			print "copying",files,destination
			shutil.copy(os.path.join(src,files),destination)

	shutil.copy(os.path.join(src,"sim.opvdm"),destination)

	if os.path.isdir(os.path.join(src,"plot")):
		shutil.copytree(os.path.join(src,"plot"), os.path.join(pwd,"plot"))

	if os.path.isdir(os.path.join(src,"exp")):
		shutil.copytree(os.path.join(src,"exp"), os.path.join(pwd,"exp"))

	shutil.copytree(os.path.join(src,"materials"), os.path.join(pwd,"materials"))


def strextract_interger(val):
	build=""
	for i in range(0,len(val)):
		if val[i].isdigit()==True:
			build=build+val[i]

	if len(build)==0:
		return -1
	else:
		return int(build)

