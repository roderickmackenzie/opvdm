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
import glib
from token_lib import tokens
from numpy import *
from util import pango_to_gnuplot
from cal_path import find_data_file
from ver import ver
from notice import notice

class splash_window():
	def timer_cb(self):
		self.window.destroy()
		return False

	def destroy(self):
		self.window.destroy()

	def callback_destroy(self,widget):
		self.destroy()

	def show_cb(self,widget, data=None):
		glib.timeout_add(1500, self.timer_cb)

	def init(self):
		self.window = gtk.Window()
		self.window.connect("show", self.show_cb)

		self.window.set_decorated(False)
		self.window.set_border_width(0)
		self.window.set_keep_above(True)
		fixed = gtk.Fixed()
		image = gtk.Image()
		image_file=find_data_file("gui/splash.png")
		label = gtk.Label()
		label.set_use_markup(gtk.TRUE)
		label.set_markup('<span color="black" size="88000"><b>opvdm</b></span>')
		label.show()
		image.set_from_file(image_file)
		image.show()
		fixed.put(image, 0, 0)
		fixed.put(label,40,40)

		label = gtk.Label()
		label.set_use_markup(gtk.TRUE)
		label.set_markup(notice()+"\n"+ver())
		label.show()

		fixed.put(label,40,200)

		self.window.add(fixed)
		self.window.set_position(gtk.WIN_POS_CENTER)
		self.window.show_all()
		self.window.connect("destroy", self.callback_destroy)


