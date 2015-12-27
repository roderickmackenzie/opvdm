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
import gtk
import sys
import os
import shutil

check_list=[]

def scan_items_clear():
	check_list=[]

class scan_item:
	name=""
	token=""
	filename=""
	line=""

def scan_item_add(file_name,token,text_info,line):
	global check_list
	check_list.append(scan_item())
	listpos=len(check_list)-1
	text_info=text_info.replace("<sub>","")
	text_info=text_info.replace("</sub>","")
	check_list[listpos].name=os.path.join(os.path.splitext(file_name)[0],text_info)
	check_list[listpos].filename=file_name
	check_list[listpos].token=token
	check_list[listpos].line=line

def scan_item_save(file_name):
	global check_list
	f = open(file_name,'w')
	f.write(str(len(check_list))+"\n")
	for i in range(0,len(check_list)):
		f.write(check_list[i].name+"\n")
		f.write(check_list[i].filename+"\n")
		f.write(check_list[i].token+"\n")
		f.write(str(check_list[i].line)+"\n")
	f.close()

def scan_remove_file(file_name):
	global check_list
	new_list=[]
	for i in range(0,len(check_list)):
		if 	check_list[i].filename!=file_name:
			new_list.append(check_list[i])

	check_list=new_list

def scan_item_load(file_name):
	global check_list
	check_list=[]
	f = open(file_name)
	lines = f.readlines()
	f.close()
	pos=0
	for i in range(0, len(lines)):
		lines[i]=lines[i].rstrip()

	length=int(lines[0])
	lines.pop(0)

	for i in range(0, length):
		check_list.append(scan_item())
		listpos=len(check_list)-1
		check_list[listpos].name=lines[i*4]
		check_list[listpos].filename=lines[(i*4)+1]
		check_list[listpos].token=lines[(i*4)+2]
		check_list[listpos].line=int(lines[(i*4)+3])

def scan_items_get_list():
	global check_list 
	return check_list

def scan_items_index_item(item):
	global check_list 
	for i in range(0,len(check_list)):
		if check_list[i].name==item:
			return i

	print "Item not found",item

	return -1



