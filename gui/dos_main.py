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
import commands
import subprocess
from win_lin import running_on_linux
from cal_path import get_exe_command
import os
import gobject
from tab_base import tab_base
from epitaxy import epitaxy_get_dos_files
from tab import tab_class
from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_get_electrical_layer
from epitaxy import epitaxy_get_mat_file
from global_objects import global_object_register
from help import my_help_class
from cal_path import find_data_file
from epitaxy import epitaxy_get_name

class dos_main(gtk.HBox,tab_base):
	
	lines=[]
	edit_list=[]
	file_name=""
	line_number=[]
	save_file_name=""
	name="Desnity of states"


	def init(self):
		self.notebook = gtk.Notebook()
		print "Welcome"
		read_page=False
		self.pack_start(self.notebook, True, True, 0)
		self.notebook.set_tab_pos(gtk.POS_LEFT)
		self.notebook.show()
		global_object_register("dos-update",self.update)

	def update(self):
		print "DoS update"


		for child in self.notebook.get_children():
				self.notebook.remove(child)

		files=epitaxy_get_dos_files()
		for i in range(0,epitaxy_get_layers()):
			dos_layer=epitaxy_get_electrical_layer(i)
			if dos_layer!="none":
				add_to_widget=True
				tab=tab_class()
				tab.show()
				tab.visible=True
				name="DoS of "+epitaxy_get_name(i)
				print dos_layer,files

				tab.init(dos_layer+".inp",name)
				tab.label_name=name
				self.notebook.append_page(tab, gtk.Label(name))

	def help(self):
		my_help_class.help_set_help(["tab.png","<big><b>Density of States</b></big>\nThis tab contains the electrical model parameters, such as mobility, tail slope energy, and band gap."])

#gobject.type_register(dos_main)
#gobject.signal_new("update", dos_main, gobject.SIGNAL_RUN_FIRST,gobject.TYPE_NONE, ())

