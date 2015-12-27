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
import gc
import gtk
import sys
import os
import shutil
from inp import inp_update_token_value
from inp import inp_get_token_value
from search import return_file_list
from plot import check_info_file
from cal_path import find_data_file
from about import about_dialog_show
from used_files_menu import used_files_menu
from server import server
from plot_dlg import plot_dlg_class
import threading
import gobject
#import pyinotify
import multiprocessing
import time
import glob
from window_list import windows
from scan_item import scan_items_get_list

class select_param:
	def init(self,liststore_combobox,dest_treeview):
		self.win_list=windows()
		self.liststore_combobox=liststore_combobox
		self.dest_treeview=dest_treeview
		self.select_window = gtk.Window(gtk.WINDOW_TOPLEVEL)
		self.select_window.set_title("Select simulation parameter")
		self.select_window.set_size_request (300,700) 
		self.treestore = gtk.TreeStore(str)
		self.win_list.set_window(self.select_window,"scan_select")
		old=""
		param_list=scan_items_get_list()
		for item in range(0, len(param_list)):
			split=os.path.split(param_list[item].name)
			main=split[0]
			sub=split[1]
			if main!=old:
				piter = self.treestore.append(None, [main])
				old=main

			if main==old:
				self.treestore.append(piter, [sub])

		#for item in range(0, len(self.param_list)):
		#	self.combo_box_list.append(self.param_list[item].name)

		self.treeview = gtk.TreeView(self.treestore)

		self.tvcolumn = gtk.TreeViewColumn('Column 0')

		self.treeview.append_column(self.tvcolumn)

		self.cell = gtk.CellRendererText()

		self.tvcolumn.pack_start(self.cell, True)

		self.tvcolumn.add_attribute(self.cell, 'text', 0)

		self.treeview.set_search_column(0)

		self.tvcolumn.set_sort_column_id(0)

		self.treeview.set_reorderable(True)
		my_vbox=gtk.VBox()
		self.treeview.show()

		button_hbox=gtk.HBox()
		self.tree_apply = gtk.Button(stock=gtk.STOCK_APPLY)
		self.tree_apply.set_size_request (-1,40) 
		self.tree_apply.connect("clicked", self.tree_apply_click, None)
		self.tree_apply.show()
		button_hbox.add(self.tree_apply)

		self.tree_close = gtk.Button(stock=gtk.STOCK_CLOSE)
		self.tree_close.set_size_request (-1,40)
		self.tree_close.connect("clicked", self.tree_close_click, None) 
		self.tree_close.show()
		button_hbox.add(self.tree_close)

		button_hbox.show()

		scrolled_window = gtk.ScrolledWindow()
		scrolled_window.set_border_width(10)

		scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_ALWAYS)

		my_vbox.pack_start(scrolled_window, gtk.TRUE, gtk.TRUE, 0)
		scrolled_window.show()


		# pack the table into the scrolled window

		scrolled_window.add_with_viewport(self.treeview)

		my_vbox.pack_start(button_hbox, gtk.FALSE, gtk.FALSE, 0)
		my_vbox.show_all()
		self.select_window.connect('delete-event', self.on_destroy)
		self.select_window.add(my_vbox)

	def on_destroy(self, widget, data=None):
		self.win_list.update(self.select_window,"scan_select")
		self.select_window.hide()
		return True

	def tree_apply_click(self, widget, data=None):
		tree_selection=self.treeview.get_selection()
		tree_selection.set_mode(gtk.SELECTION_MULTIPLE)
		(model, pathlist) = tree_selection.get_selected_rows()
		print "model=",model
		print "pathlist",pathlist
		for path in pathlist :
			tree_iter = model.get_iter(path)
			parent=model.iter_parent(tree_iter)
			value = model.get_value(tree_iter,0)
			ret=os.path.join(model.get_value(parent,0),value)
			print ret

			dest_tree_selection=self.dest_treeview.get_selection()
			(dest_model, dest_pathlist) = dest_tree_selection.get_selected_rows()
			self.liststore_combobox[dest_pathlist[0][0]][0]=ret

	def tree_close_click(self, widget, data=None):
		self.select_window.hide()
