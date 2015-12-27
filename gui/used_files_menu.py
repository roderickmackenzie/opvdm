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
import copy
import pdb
import logging
from plot_state import plot_state
import glob
from inp import inp_load_file
from plot_io import plot_load_info
from plot_io import plot_save_oplot_file
class used_files_menu:
	def __init__(self):
		self.menu=gtk.Menu()

	def refresh(self):
		try:
			for i in self.menu.get_children():
				self.menu.remove(i)
		except:
			pass

		self.list=[]
		files=[]
		for my_file in glob.glob(os.path.join(self.base_dir,"*.oplot")):
			files.append(my_file)

		sorted(files)
		for i in range(0,len(files)):
			self.append(files[i])

	def init(self,search_path,callback):

		self.base_dir=search_path
		self.callback=callback

		self.refresh()

	def append(self,file_name):
		lines=[]
		plot_token=plot_state()
		if plot_load_info(plot_token,file_name)==True:
			menu_item = gtk.MenuItem(os.path.basename(file_name).split(".")[0])		   
			self.menu.append(menu_item)
			self.list.append(plot_token)
			menu_item.connect("activate", self.callback,self.list[len(self.list)-1])
			menu_item.show()



