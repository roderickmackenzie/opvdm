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
from numpy import *
from matplotlib.figure import Figure
from numpy import arange, sin, pi
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg as NavigationToolbar
import gobject
from scan_item import scan_item_add
from inp import inp_load_file
from inp import inp_read_next_item
from gui_util import dlg_get_text
from inp import inp_get_token_value
import matplotlib.mlab as mlab
from debug import debug_mode
from inp import inp_write_lines_to_file
import webbrowser
from util import time_with_units
from inp import inp_search_token_value
from tmesh import tab_time_mesh
from circuit import circuit
from inp import inp_update_token_value
from cal_path import get_image_file_path

(
SEG_LENGTH,
SEG_DT,
SEG_VOLTAGE_START,
SEG_VOLTAGE_STOP,
SEG_MUL,
SEG_SUN,
SEG_LASER
) = range(7)

mesh_articles = []

class experiment_tab(gtk.VBox):

	def update(self):
		self.tmesh.update()

	def init(self,index):
		self.tab_label=None


		self.index=index
		lines=[]

		if inp_load_file(lines,"pulse"+str(self.index)+".inp")==True:
			self.tab_name=inp_search_token_value(lines, "#sim_menu_name")
		else:
			self.tab_name=""


		self.title_hbox=gtk.HBox()

		self.title_hbox.set_size_request(-1, 25)
		self.label=gtk.Label(self.tab_name.split("@")[0])
		self.label.set_justify(gtk.JUSTIFY_LEFT)
		self.title_hbox.pack_start(self.label, False, True, 0)

		self.close_button = gtk.Button()
		close_image = gtk.Image()
   		close_image.set_from_file(os.path.join(get_image_file_path(),"close.png"))
		close_image.show()
		self.close_button.add(close_image)
		self.close_button.props.relief = gtk.RELIEF_NONE

		self.close_button.set_size_request(25, 25)
		self.close_button.show()

		self.title_hbox.pack_end(self.close_button, False, False, 0)
		self.title_hbox.show_all()

		self.notebook=gtk.Notebook()
		self.notebook.show()
		self.tmesh = tab_time_mesh()
		self.tmesh.init(self.index)

		self.notebook.append_page(self.tmesh, gtk.Label(_("time mesh")))

		self.pack_start(self.notebook, False, False, 0)

		self.circuit=circuit()
		self.circuit.init(self.index)

		self.notebook.append_page(self.circuit, gtk.Label(_("Circuit")))

		self.show()

	def set_tab_caption(self,name):
		mytext=name
		if len(mytext)<10:
			for i in range(len(mytext),10):
				mytext=mytext+" "
		self.label.set_text(mytext)

	def rename(self,tab_name):
		self.tab_name=tab_name+"@"+self.tab_name.split("@")[1]
		inp_update_token_value("pulse"+str(self.index)+".inp", "#sim_menu_name", self.tab_name,1)
		self.set_tab_caption(self.tab_name.split("@")[0])


