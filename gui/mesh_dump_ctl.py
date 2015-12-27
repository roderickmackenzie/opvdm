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
import gobject
from inp import inp_load_file
from inp import inp_sum_items
from inp import inp_search_token_value
from inp import inp_update_token_value
from util import str2bool
class mesh_dump_ctl(gtk.VBox):

	def scroll(self, adj):
		self.pos=int(adj.value)
		inp_update_token_value("dump.inp", "#dump_energy_slice_pos", str(self.pos),1)
		self.emit("update")

	def check_clicked(self, widget):
		self.enable=widget.get_active()
		self.vscale.set_sensitive(self.enable)
		inp_update_token_value("dump.inp", "#dump_energy_slice_switch", str(int(self.enable)),1)
		self.emit("update")

	def init(self):
		total=0
		self.pos=0
		lines=[]
		if inp_load_file(lines,"mesh.inp")==True:
			total=inp_sum_items(lines, "#mesh_layer_points0")

		if inp_load_file(lines,"dump.inp")==True:
			self.pos=str2bool(inp_search_token_value(lines, "#dump_energy_slice_pos"))


		label=gtk.Label("Energy slice dump")
		label.show()
		self.pack_start(label, True, True, 0)

		check = gtk.CheckButton("Enable")
		self.pack_start(check, True, True, 0)

		print "total=",total
		adj=gtk.Adjustment(self.pos, 0, total, 1.0, 1.0, 1.0)
		adj.connect("value_changed", self.scroll)
		self.vscale = gtk.HScale(adj)
		self.vscale.set_size_request(150, 30)
		self.pack_start(self.vscale, True, True, 0)
		self.vscale.show()


		self.enable=False
		if inp_load_file(lines,"dump.inp")==True:
			self.enable=str2bool(inp_search_token_value(lines, "#dump_energy_slice_switch"))

		check.set_active(self.enable)
		self.vscale.set_sensitive(self.enable)

		check.unset_flags(gtk.CAN_FOCUS)
		check.connect("clicked", self.check_clicked)



gobject.type_register(mesh_dump_ctl)
gobject.signal_new("update", mesh_dump_ctl, gobject.SIGNAL_RUN_FIRST,gobject.TYPE_NONE, ())

