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
import time
import glob
from cal_path import get_device_lib_path
from util_zip import read_lines_from_archive
from inp import inp_search_token_value_multiline
from util_zip import zip_lsdir

(
  COLUMN_FILENAME,
  COLUMN_TYPE,
  COLUMN_INFO
) = range(3)

class device_lib_class(gtk.Dialog):

	def __add_columns(self, treeview):

		model = treeview.get_model()

		renderer = gtk.CellRendererText()
		#renderer.connect("edited", self.on_cell_edited, model)
		renderer.set_data("column", COLUMN_FILENAME)
		#renderer.set_property("editable", True)
		column = gtk.TreeViewColumn("File name", renderer, text=COLUMN_FILENAME,editable=True)
		treeview.append_column(column)

		renderer = gtk.CellRendererText()
		#renderer.connect("edited", self.on_cell_edited, model)
		renderer.set_data("column", COLUMN_TYPE)
		#renderer.set_property("editable", True)
		column = gtk.TreeViewColumn("Device type", renderer, text=COLUMN_TYPE,editable=True)
		treeview.append_column(column)

		renderer = gtk.CellRendererText()
		#renderer.connect("edited", self.on_cell_edited, model)
		renderer.set_data("column", COLUMN_INFO)
		#renderer.set_property("editable", True)
		renderer.props.wrap_width = 400
		renderer.props.wrap_mode = gtk.WRAP_WORD

		column = gtk.TreeViewColumn("Description", renderer, text=COLUMN_INFO,editable=True)
		treeview.append_column(column)

	def __create_model(self):

		# create list store
		model = gtk.ListStore(str,str,str)

		# add items

		files=glob.glob(os.path.join(get_device_lib_path(),"*.opvdm")) 
		for i in range(0,len(files)):
			print "working on",files[i],zip_lsdir(files[i])
			iter = model.append()
			lines=[]
			if read_lines_from_archive(lines,files[i],"info.inp")==True:
				print lines
				model.set (iter,
					COLUMN_FILENAME, os.path.basename(files[i]),
					COLUMN_TYPE, " ".join(inp_search_token_value_multiline(lines,"#info_device_type")),
					COLUMN_INFO, " ".join(inp_search_token_value_multiline(lines,"#info_description"))
				)
		return model

	def init(self):
		self.file_path=""
		self.set_default_response(gtk.RESPONSE_OK)
		self.set_title("Device library")
		self.set_flags(gtk.DIALOG_DESTROY_WITH_PARENT)
		#self.add_buttons("OK",True,"Cancel",False)

		self.set_size_request(800, 400)
		self.set_position(gtk.WIN_POS_CENTER)


		self.model = self.__create_model()

		self.treeview = gtk.TreeView(self.model)
		self.treeview.set_size_request(380, 150)
		self.treeview.set_rules_hint(True)
		self.treeview.get_selection().set_mode(gtk.SELECTION_SINGLE)
		self.__add_columns(self.treeview)
		self.treeview.show()
		
		self.vbox.pack_start(self.treeview, True, True, 0)

		self.hbox=gtk.HBox()
		self.vbox.pack_start(self.hbox, False, False, 0)
		ok = gtk.Button(label="Import", stock=gtk.STOCK_OK)
		ok.connect("clicked", self.on_ok_clicked)
		close = gtk.Button(label="Close", stock=gtk.STOCK_CLOSE)
		self.hbox.pack_end(ok, False, False, 0)
		close.connect("clicked", self.on_close_clicked)
		self.hbox.pack_end(close, False, False, 0)
		self.hbox.show_all()
		self.show()

	def on_ok_clicked(self, button):

		selection = self.treeview.get_selection()
		model, iter = selection.get_selected()


		model, iter = selection.get_selected()

		if iter:
			path = model.get_path(iter)[0]
			self.file_path=os.path.join(get_device_lib_path(),model[path][0])
			md = gtk.MessageDialog(None, 0, gtk.MESSAGE_QUESTION,  gtk.BUTTONS_YES_NO, "You are about to import the simulation file "+self.file_path+" into this simulation.  The result will be that all model parameters will be overwritten.  Do you really want to do that?")

			response = md.run()

			if response == gtk.RESPONSE_YES:
				self.response(True)
			elif response == gtk.RESPONSE_NO:
				return
			md.destroy()

			

	def on_close_clicked(self, button):
			self.response(False)

