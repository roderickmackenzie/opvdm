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


import pygtk
pygtk.require('2.0')
import gc
import gtk
import sys
import os
import shutil
from search import return_file_list
from plot import check_info_file
from cal_path import find_data_file
from token_lib import tokens
from win_lin import running_on_linux
from inp import inp_get_token_value
from plot_state import plot_state
from plot import plot_populate_plot_token

def gen_plot_line(dirname,plot_token):
	print plot_token.file0,plot_token.file1
	if plot_token.file1=="":
		f = open(os.path.join(dirname,plot_token.file0),'r')
		values=f.readline()
		f.close()
		return values
	else:
		v0=inp_get_token_value(os.path.join(dirname,plot_token.file0), plot_token.tag0)
		v1=inp_get_token_value(os.path.join(dirname,plot_token.file1), plot_token.tag1)
		v2=""
		print os.path.join(dirname,plot_token.file1),plot_token.tag1
		if plot_token.file2!="":
			v2=inp_get_token_value(os.path.join(dirname,plot_token.file2), plot_token.tag2)
		values=v0+" "+v1+" "+v2+"\n"
		return values

def gen_infofile_plot(file_list_in,base_dir,plot_token):
	file_name=os.path.splitext(plot_token.file0)[0]+plot_token.tag0+"#"+os.path.splitext(plot_token.file1)[0]+plot_token.tag1+".dat"
	values=""
	result=[]

	#only allow files from real simulations in the list
	for i in range(0,len(file_list_in)):
		test_name=os.path.join(os.path.dirname(file_list_in[i]),'sim.opvdm')
		if os.path.isfile(test_name):
			result.append(file_list_in[i])


	if len(result)==0:
		print "No files found"
		return

	#pull out first item
	ittr_path=os.path.dirname(result[0])
	
	start_of_sim_dir_path_pos=len(base_dir)+1
	ittr_path=ittr_path[start_of_sim_dir_path_pos:]
	#check it's depth
	if running_on_linux():
		depth=ittr_path.count('/')
	else:
		depth=ittr_path.count('\\')
		
	#print "DEPTHDEPTHDEPTHDEPTH:"
	#print result
	#print ittr_path
	#print str(depth)

	#Remove the first part of the path name just leaving what is in the simulation dir
	if depth==0:
		mydirs=[""]
	else:
		mydirs=[]
		for i in result:
			ittr_path=os.path.dirname(i)
			ittr_path=ittr_path[start_of_sim_dir_path_pos:]
			ittr_path=os.path.split(ittr_path)
			#print ittr_path
			if mydirs.count(ittr_path[0])==0:
				mydirs.append(ittr_path[0])

	

	data=["" for x in range(len(mydirs))]

	#for each directory save the data into an array element?
	for i in range(0, len(result)):
		cur_sim_path=os.path.dirname(result[i])
		if cur_sim_path!=base_dir:
			#print result[i],cur_sim_path
			values=gen_plot_line(cur_sim_path,plot_token)

			if depth==0:
				pos=0
			else:
				ittr_path=os.path.dirname(result[i])
				ittr_path=ittr_path[start_of_sim_dir_path_pos:]
				ittr_path=os.path.split(ittr_path)
				pos=mydirs.index(ittr_path[0])

			#print pos
			
			data[pos]=data[pos]+values
			#print data[pos]
	plot_files=[]
	plot_labels=[]

	#Dump the array elements to disk
	for i in range(0,len(mydirs)):
		newplotfile=os.path.join(base_dir,mydirs[i],file_name)
		plot_files.append(newplotfile)
		plot_labels.append(os.path.basename(mydirs[i]))
		f = open(newplotfile,'w')
		f.write(data[i])
		f.close()

	save_file=os.path.join(base_dir,os.path.splitext(file_name)[0])+".oplot"
	#print "save path",save,plot_files

	plot_populate_plot_token(plot_token,None)

	return plot_files, plot_labels, save_file

def scan_gen_plot_data(plot_token,base_path):
	ret=""
	plot_files=[]
	plot_labels=[]
	save_file=""

	file_name=plot_token.file0
	if file_name=="":
		print "You have given me no file name!!!"
		sys.exit(0)

	#search for the files
	return_file_list(plot_files,base_path,file_name)
	print "search_file=",plot_token.file0
	print "found_files=",plot_files
	num_list=[]

	#remove the file name in the base_dir
	test_file=os.path.join(base_path,file_name)
	if test_file in plot_files:
	    plot_files.remove(test_file)

	#attempt to sort list in numeric order
	try:
		for i in range(0, len(plot_files)):
			dir_name=os.path.basename(os.path.dirname(plot_files[i]))
			if dir_name=="dynamic":
				dir_name=os.path.basename(os.path.dirname(os.path.dirname(plot_files[i])))
			num_list.append(float(dir_name))

		num_list, plot_files = zip(*sorted(zip(num_list, plot_files)))
	except:
		print "There are stings in the list I can not order it"

	#if it is an info file then deal with it
	print check_info_file(file_name),file_name,plot_token.file0,plot_token.file1,plot_token.tag0,plot_token.tag1

	if (check_info_file(file_name)==True):
		#print "Rod",plot_files,self.sim_dir

		print plot_files,"r",plot_labels,"r",save_file,"r",plot_files,"r",base_path,"r",plot_token
		
		plot_files, plot_labels, save_file = gen_infofile_plot(plot_files,base_path,plot_token)

	else:
		ret=plot_populate_plot_token(plot_token,plot_files[0])
		if ret==False:
			message = gtk.MessageDialog(type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_OK)
			message.set_markup("This file "+file_name+" is not in the data base please file a bug report..")
			message.run()
			message.destroy()
			return

		#build plot labels
		for i in range(0,len(plot_files)):
			text=plot_files[i][len(base_path):len(plot_files[i])-1-len(os.path.basename(plot_files[i]))]
			if text.endswith("dynamic"):
				text=text[:-7]

			if text.endswith("light_dump"):
				text=text[:-10]

			plot_labels.append(str(text))

		save_file=os.path.join(base_path,os.path.splitext(os.path.basename(plot_files[0]))[0])+".oplot"
		

	return plot_files, plot_labels, save_file
