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
import gobject
from cal_path import find_data_file
from inp import inp_update_token_value
from inp import inp_get_token_value
from inp import inp_load_file
from inp import inp_lsdir
from inp import inp_search_token_value

import i18n
_ = i18n.language.gettext

class store:
	def __init__(self,token,file):
		self.token=token
		self.file=file

class tb_item_sim_mode(gtk.ToolItem):

	def update(self):
		self.sim_mode.get_model().clear()
		lines=[]
		self.store_list=[]

		files=inp_lsdir()
		if files!=False:
			for i in range(0,len(files)):
				if files[i].endswith(".inp"):
					inp_load_file(lines,files[i])
					value=inp_search_token_value(lines, "#sim_menu_name")
					if value!=False:
						if value.count("@")==1:
							value=value.rstrip()
							command,module=value.split("@")
							self.sim_mode.append_text(command)
							self.store_list.append(store(command,module))


			token=inp_get_token_value("sim.inp", "#simmode")
			command,module=token.split("@")
			liststore = self.sim_mode.get_model()
			for i in xrange(len(liststore)):
				if liststore[i][0] == command:
					self.sim_mode.set_active(i)

	def init(self):
		self.sim_mode = gtk.combo_box_entry_new_text()
		self.sim_mode.set_size_request(-1, 20)
		self.sim_mode.child.connect('changed', self.call_back_sim_mode_changed)

		lable=gtk.Label(_("Simulation mode:"))
		lable.show()

		hbox = gtk.HBox(False, 2)

		hbox.pack_start(lable, False, False, 0)
		hbox.pack_start(self.sim_mode, False, False, 0)

		self.add(hbox);
		self.show_all()
		self.update()

	def call_back_sim_mode_changed(self, widget, data=None):
		mode=self.sim_mode.get_active_text()
		for i in range(0,len(self.store_list)):
			if self.store_list[i].token==mode:
				inp_update_token_value("sim.inp", "#simmode", mode+"@"+self.store_list[i].file,1)


