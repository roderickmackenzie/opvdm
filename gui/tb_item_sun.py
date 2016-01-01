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
from inp import inp_update_token_value
from inp import inp_get_token_value
from global_objects import global_object_get
from global_objects import global_isobject

import i18n
_ = i18n.language.gettext

class tb_item_sun(gtk.ToolItem):
	def call_back_light_changed(self, widget, data=None):
		light_power=self.light.get_active_text()
		#print light_power
		inp_update_token_value("light.inp", "#Psun", light_power,1)
		if global_isobject("experiment_graph_update")==True:
			global_object_get("experiment_graph_update")()

		self.emit("refresh")

	def __init__(self):
		self.__gobject_init__()
		self.light = gtk.combo_box_entry_new_text()
		#self.sim_mode.set_size_request(-1, 20)
		sun_values=["0.0","0.01","0.1","1.0","10"]
		token=inp_get_token_value("light.inp", "#Psun")
		if sun_values.count(token)==0:
			sun_values.append(token)

		for i in range(0,len(sun_values)):
			self.light.append_text(sun_values[i])

		liststore = self.light.get_model()
		for i in xrange(len(liststore)):
		    if liststore[i][0] == token:
		        self.light.set_active(i)

		self.light.child.connect('changed', self.call_back_light_changed)

		lable=gtk.Label(_("Light intensity (Suns):"))
		lable.show
		hbox = gtk.HBox(False, 2)
		hbox.pack_start(lable, False, False, 0)
		hbox.pack_start(self.light, False, False, 0)

		self.add(hbox)
		self.show_all()

gobject.type_register(tb_item_sun)
gobject.signal_new("refresh", tb_item_sun, gobject.SIGNAL_RUN_FIRST,gobject.TYPE_NONE, ())

