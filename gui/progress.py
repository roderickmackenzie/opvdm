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
from gui_util import dlg_get_text
import threading
import gobject
import multiprocessing
import time
import glob
from help import my_help_class

class progress_class(gtk.Window):

	def init(self):
		self.set_decorated(False)
		self.set_title("opvdm progress")
		self.set_size_request(400, 50)
		self.set_position(gtk.WIN_POS_CENTER)
		self.set_keep_above(True)

		main_hbox = gtk.HBox(False, 5)
		main_hbox.set_border_width(1)
		main_hbox.show()

		self.progress = gtk.ProgressBar(adjustment=None)
		self.progress.show()

		main_hbox.pack_start(self.progress, True, True, 0)

		self.spin=gtk.Spinner()
		self.spin.set_size_request(32, 32)
		self.spin.show()
		self.spin.stop()

		main_hbox.pack_start(self.spin, False, False, 0)


		main_vbox = gtk.VBox(False, 5)
		main_vbox.show()
		main_vbox.pack_start(main_hbox, True, True, 0)

		self.progress_array = []
		for i in range(0,10):
			self.progress_array.append(gtk.ProgressBar(adjustment=None))
			self.progress_array[i].hide()
			self.progress_array[i].set_size_request(-1, 15)
			self.progress_array[i].modify_bg(gtk.STATE_PRELIGHT, gtk.gdk.color_parse("green"))
			main_vbox.pack_end(self.progress_array[i], True, False, 0)

		self.label=gtk.Label("Running...")
		#label.set_justify(gtk.JUSTIFY_LEFT)
		self.label.show()

		main_vbox.pack_start(main_hbox, True, True, 0)
		main_vbox.pack_start(self.label, True, True, 4)


		self.add(main_vbox)

	def set_fraction(self,fraction):
		self.progress.set_fraction(fraction)

	def pulse(self):
		self.progress.pulse()

	def start(self):
		my_help_class.hide()

		s=gtk.gdk.screen_get_default()
		w,h=self.get_size()

		x=s.get_width()-w
		y=0

		self.move(x,y)

		self.show()
		self.spin.start()

	def stop(self):
		self.hide()
		self.spin.stop()
		my_help_class.help_show()

	def set_text(self,text):
		text=text
		l=len(text)
		if l>50:
			l=l-50
			text=text[l:]

		self.label.set_text(text)

