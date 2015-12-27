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
import math
import random
from layer_widget import layer_widget
from util import read_xyz_data
import os
from cal_path import get_materials_path
from inp import inp_load_file
from inp import inp_search_token_value
from util import str2bool
from tab_base import tab_base
from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_get_width
from epitaxy import epitaxy_get_mat_file
from epitaxy import epitaxy_get_electrical_layer
from help import my_help_class
from cal_path import find_data_file
from cal_path import get_image_file_path
from epitaxy import epitaxy_get_pl_file
from epitaxy import epitaxy_get_name
from tb_pulse_load_type import tb_pulse_load_type

class circuit(gtk.VBox):

	def update(self,object):
		self.darea.queue_draw()

	def init(self,index):
		self.index=index
		self.toolbar = gtk.Toolbar()
		self.toolbar.set_style(gtk.TOOLBAR_ICONS)
		self.toolbar.set_size_request(900, 50)
		self.toolbar.show()

		pos=0
		self.load_type=tb_pulse_load_type()
		self.load_type.init(self.index)
		self.load_type.connect("changed", self.draw_callback)

		self.toolbar.insert(self.load_type, pos)
		pos=pos+1

		self.pack_start(self.toolbar, False, False, 0)

		self.darea = gtk.DrawingArea()
		self.darea.connect("expose-event", self.expose)

		self.pack_start(self.darea, True, True, 0)

		self.diode = gtk.gdk.pixbuf_new_from_file(os.path.join(get_image_file_path(),"diode.png"))
		self.load = gtk.gdk.pixbuf_new_from_file(os.path.join(get_image_file_path(),"load.png"))
		self.voc = gtk.gdk.pixbuf_new_from_file(os.path.join(get_image_file_path(),"voc.png"))

		self.show_all()

	def draw_callback(self,sender):
		self.darea.queue_draw()
		#self.cr = self.darea.window.cairo_create()
		#self.draw()

	def draw(self):
		x=40
		y=40
		self.cr.set_source_rgb(0.0,0.0,0.0)
		self.cr.set_font_size(14)

		self.cr.move_to(x+130, y+120)

		self.cr.show_text("C=")

		self.cr.move_to(x+250, y+120)
		self.cr.show_text("Rshunt=")

		self.cr.move_to(x+250, y+25)
		self.cr.show_text("Rcontact=")

		self.cr.set_source_pixbuf(self.diode, x, y)
		self.cr.paint()

		if self.load_type.sim_mode.get_active_text()=="load":
			self.cr.set_source_pixbuf(self.load, x+610, y+67)
			self.cr.paint()
			self.cr.move_to(x+550, y+150)
			self.cr.show_text("Rload=")
		else:
			self.cr.set_source_pixbuf(self.voc, x+610, y+57)
			self.cr.paint()


	def expose(self, widget, event):
		self.cr = self.darea.window.cairo_create()
		self.draw()

