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
import socket 
import time
import urlparse
import re
import os
from ver import ver_core
from ver import ver_mat
from ver import ver_gui
import gobject
from cal_path import get_image_file_path
from global_objects import global_object_register
import webbrowser


class help_data():
	def __init__(self,token,icon,text):
		self.token=token
		self.icon=icon
		self.text=text

class help_class(gtk.Window):
	def move_window(self):
		s=gtk.gdk.screen_get_default()
		w,h=self.get_size()

		x=s.get_width()-w-50
		y=0

		self.move(x,y)

	def help_show(self):
		self.show_all()
		self.move_window()

	def toggle_visible(self):
		if self.get_visible()==True:
			self.hide()
		else:
			self.show()
		self.move_window()

	def get_help(self,token):
		for i in range(0,len(self.help_lib)):
			if self.help_lib[i].token==token:
				self.help_set_help([self.help_lib[i].icon,self.help_lib[i].text])
		return False 

	def init(self):
		self.last=[]
		self.pos=-1
		self.help_lib=[]
		self.help_lib.append(help_data("device.inp","tab.png","<big><b>Device tab</b></big>\nThis tab contains information about the device, such as width breadth, carrier density on the contacts, shunt and contact resistance."))
		self.help_lib.append(help_data("jv.inp","tab.png","<big><b>JV tab</b></big>\nThis tab controls the JV curve simulation."))
		self.help_lib.append(help_data("jv_simple.inp","tab.png","<big><b>JV simple tab</b></big>\nThe 'jv simple' model does not use opvdm's full device model instead it uses the diode equation to simulate a solar cell.  Sometimes for papers it is useful to do this."))
		self.help_lib.append(help_data("dump.inp","tab.png","<big><b>Dump tab</b></big>\nThe dump tab controls what the simulation writes disk, generally writing to disk is a slow process so by default the model dumps relitavly little data."))
		self.help_lib.append(help_data("celiv.inp","tab.png","<big><b>CELIV tab</b></big>\nThe CELIV tab controls the the parameters for a CELIV simulation."))
		self.help_lib.append(help_data("thermal.inp","tab.png","<big><b>Thermal tab</b></big>\nUse this tab to set the simulation temperature."))
		self.help_lib.append(help_data("tof.inp","tab.png","<big><b>ToF tab</b></big>\nUse this to configure the time of flight simulation."))

		self.move_window()
		self.vbox=gtk.VBox()
		self.vbox.show()
		self.add(self.vbox)

		self.box=[]
		self.image=[]
		self.label=[]
		for i in range(0,5):
			self.box.append(gtk.HBox())
			self.image.append(gtk.Image())
			self.label.append(gtk.Label())

			self.label[i].set_line_wrap(True)
			self.label[i].set_justify(gtk.JUSTIFY_LEFT)
			self.label[i].set_width_chars(30)
			self.label[i].set_alignment(0.1, 0)

			self.box[i].pack_start(self.image[i], False, False, 0)
			self.box[i].pack_start(self.label[i], True, True, 0)



		toolbar = gtk.Toolbar()
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(100, 50)
		toolbar.show()

		pos=0
		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"qe.png"))

		self.back = gtk.ToolButton(gtk.STOCK_GO_BACK)
		toolbar.insert(self.back, pos)
		self.back.show_all()
		self.back.set_sensitive(False)
		self.back.connect("clicked", self.callback_back)
		pos=pos+1

		self.forward = gtk.ToolButton(gtk.STOCK_GO_FORWARD)
		toolbar.insert(self.forward, pos)
		self.forward.set_sensitive(False)
		self.forward.show_all()
		self.forward.connect("clicked", self.callback_forward)
		pos=pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(False)
		sep.set_expand(True)
		sep.show()
		toolbar.insert(sep, pos)
		pos=pos+1

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"www.png"))
		self.play = gtk.ToolButton(image)

		help_book = gtk.ToolButton(image)
		toolbar.insert(help_book, pos)
		help_book.connect("clicked", self.on_line_help)
		help_book.show_all()
		pos=pos+1

		close = gtk.ToolButton(gtk.STOCK_CLOSE)
		toolbar.insert(close, pos)
		close.connect("clicked", self.callback_close)
		close.show_all()
		pos=pos+1

		self.status_bar = gtk.Statusbar()      
		self.context_id = self.status_bar.get_context_id("Statusbar example")
		self.status_bar.show()
		#self.tooltips.set_tip(self.qe_button, "Quantum efficiency")


		self.vbox.pack_start(toolbar, False, False, 0)

		for i in range(0,5):
			self.vbox.pack_start(self.box[i], True, True, 0)

		self.vbox.pack_start(self.status_bar, False, False, 0)




		self.set_border_width(10)
		self.set_title("Help")
		self.resize(300,100)
		self.set_decorated(False)
		self.set_border_width(0)

		self.show()


	def callback_close(self,widget):
		self.toggle_visible()

	def callback_forward(self,widget):
		self.pos=self.pos+1
		if self.pos>=len(self.last):
			self.pos=len(self.last)-1

		self.update()

	def callback_back(self,widget):
		self.pos=self.pos-1
		if self.pos<0:
			self.pos=0
		#print self.pos,self.last_icons
		self.update()

	def on_line_help(self,widget):
		webbrowser.open('http://www.opvdm.com/man/index.html')

	def update(self):
		for i in range(0,5):
			self.box[i].hide_all()

		for i in range(0,len(self.last[self.pos])/2):
			self.image[i].set_from_file(os.path.join(get_image_file_path(),self.last[self.pos][i*2]))
			self.label[i].set_markup(self.last[self.pos][i*2+1]+"\n")
			self.box[i].show_all()
			self.image[i].show()

		self.forward.set_sensitive(True)
		self.back.set_sensitive(True)

		if self.pos==0:
			self.back.set_sensitive(False)

		if self.pos==len(self.last)-1:
			self.forward.set_sensitive(False)

		self.status_bar.push(self.context_id, str(self.pos)+"/"+str(len(self.last)-1))

	def help_set_help(self,array):
		add=True
		if len(self.last)!=0:
			if self.last[self.pos][1]==array[1]:
				add=False

		if add==True:
			self.pos=self.pos+1
			self.last.append(array)
		self.update()
		self.resize(300, 150)
		self.move_window()


my_help_class=help_class()
my_help_class.init()

		
