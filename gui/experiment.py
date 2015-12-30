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
from cal_path import find_data_file
from about import about_dialog_show
from used_files_menu import used_files_menu
from server import server
from scan_tab import scan_vbox
from gui_util import dlg_get_text
import threading
import gobject
import multiprocessing
import time
import glob
from window_list import windows
from search import return_file_list
from win_lin import running_on_linux
import webbrowser
from debug import debug_mode
from inp import inp_update_token_value
from inp import inp_get_token_value
from experiment_tab import experiment_tab
from util_zip import zip_lsdir
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_remove_file
from util import strextract_interger
from global_objects import global_object_get
from cal_path import get_image_file_path

import i18n
_ = i18n.language.gettext

def experiment_new_filename():
	for i in range(0,20):
		pulse_name="pulse"+str(i)+".inp"
		time_mesh_name="time_mesh_config"+str(i)+".inp"
		if inp_isfile(pulse_name)==False:
			return i
	return -1

class experiment(gtk.Window):

	def get_main_menu(self, window):
		accel_group = gtk.AccelGroup()
		item_factory = gtk.ItemFactory(gtk.MenuBar, "<main>", accel_group)

		item_factory.create_items(self.menu_items)
		if debug_mode()==False:
			item_factory.delete_item(_("/Advanced"))

		window.add_accel_group(accel_group)

		self.item_factory = item_factory

		return item_factory.get_widget("<main>")

	def callback_close(self, widget, data=None):
		self.win_list.update(self,"experiment_window")
		self.hide()
		return True

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.opvdm.com/man/index.html')

	def callback_add_page(self, widget, data=None):
		new_sim_name=dlg_get_text( _("New experiment name:"), _("experiment ")+str(len(self.notebook.get_children())+1))

		if new_sim_name!=None:
			index=experiment_new_filename()
			inp_copy_file("pulse"+str(index)+".inp","pulse0.inp")
			inp_copy_file("time_mesh_config"+str(index)+".inp","time_mesh_config0.inp")
			inp_update_token_value("pulse"+str(index)+".inp", "#sim_menu_name", new_sim_name+"@pulse",1)
			self.add_page(index)
			global_object_get("tb_item_sim_mode_update")()

	def callback_remove_page(self,widget,name):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		self.toggle_tab_visible(tab.tab_name)

	def callback_copy_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		old_index=tab.index
		new_sim_name=dlg_get_text( _("Clone the current experiment to a new experiment called:"), tab.tab_name.split("@")[0])
		if new_sim_name!=None:
			new_sim_name=new_sim_name+"@"+tab.tab_name.split("@")[1]
			index=experiment_new_filename()
			inp_copy_file("pulse"+str(index)+".inp","pulse"+str(old_index)+".inp")
			inp_copy_file("time_mesh_config"+str(index)+".inp","time_mesh_config"+str(old_index)+".inp")
			inp_update_token_value("pulse"+str(index)+".inp", "#sim_menu_name", new_sim_name,1)
			self.add_page(index)
			global_object_get("tb_item_sim_mode_update")()

	#def callback_run_experiment(self,widget,data):
	#	pageNum = self.notebook.get_current_page()
	#	tab = self.notebook.get_nth_page(pageNum)
	#	tab.simulate(True,True)


	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_rename_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		new_sim_name=dlg_get_text( _("Rename the experiment to be called:"), tab.tab_name.split("@")[0])

		if new_sim_name!=None:
			#new_sim_name=self.remove_invalid(new_sim_name)
			#new_dir=os.path.join(self.sim_dir,new_sim_name)
			#shutil.move(old_dir, new_dir)
			tab.rename(new_sim_name)
			global_object_get("tb_item_sim_mode_update")()
			#edit

	def callback_delete_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		md = gtk.MessageDialog(None, 0, gtk.MESSAGE_QUESTION,  gtk.BUTTONS_YES_NO, _("Should I remove the experiment file ")+tab.tab_name.split("@")[0])

		response = md.run()

		if response == gtk.RESPONSE_YES:
			inp_remove_file("pulse"+str(tab.index)+".inp")
			inp_remove_file("time_mesh_config"+str(tab.index)+".inp")
			self.notebook.remove_page(pageNum)
			global_object_get("tb_item_sim_mode_update")()
			
		elif response == gtk.RESPONSE_NO:
			print _("Not deleting")
			#edit


		md.destroy()

	def toggle_tab_visible(self,name):
		tabs_open=0
		print name
		for i in range(0, self.number_of_tabs):
			if self.rod[i].visible==True:
				tabs_open=tabs_open+1

		print _("tabs open"),tabs_open,self.number_of_tabs

		for i in range(0, self.number_of_tabs):
			print self.rod[i].tab_name, name, self.rod[i].visible
			if self.rod[i].tab_name==name:
				if self.rod[i].visible==False:
					self.rod[i].set_visible(True)
					self.rod[i].visible=True
				else:
					if tabs_open>1:
						print self.rod[i].tab_label
						self.rod[i].set_visible(False)
						self.rod[i].visible=False

	def callback_view_toggle(self, widget, data):
		self.toggle_tab_visible(widget.get_label())

	def callback_view_toggle_tab(self, widget, data):
		self.toggle_tab_visible(data)

	def load_tabs(self):

		file_list=zip_lsdir(os.path.join(os.getcwd(),"sim.opvdm"))
		files=[]
		for i in range(0,len(file_list)):
			if file_list[i].startswith("pulse") and file_list[i].endswith(".inp"):
				files.append(file_list[i])

		print "load tabs",files

		for i in range(0,len(files)):
			value=strextract_interger(files[i])
			if value!=-1:
				self.add_page(value)

	def clear_pages(self):
		for items in self.tab_menu.get_children():
			self.tab_menu.remove(items)

		for child in self.notebook.get_children():
    			self.notebook.remove(child)

		self.rod=[]

	def add_page(self,index):
		new_tab=experiment_tab()
		new_tab.init(index)
		new_tab.close_button.connect("clicked", self.callback_view_toggle_tab,new_tab.tab_name)

		self.notebook.append_page(new_tab,new_tab.title_hbox)
		self.notebook.set_tab_reorderable(new_tab,True)

	def switch_page(self,page, page_num, user_param1):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		self.status_bar.push(self.context_id, tab.tab_name)

	def init(self):
		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"experiment_window")
		print "constructur"

		self.tooltips = gtk.Tooltips()

		self.set_border_width(2)
		self.set_title(_("Time domain experiment window - opvdm"))

		self.status_bar = gtk.Statusbar()      
		self.status_bar.show()
		self.context_id = self.status_bar.get_context_id("Statusbar example")

		box=gtk.HBox()
		box.add(self.status_bar)
		box.set_child_packing(self.status_bar, True, True, 0, 0)
		box.show()


		self.menu_items = (
		    ( _("/_File"),         None,         None, 0, "<Branch>" ),
		    ( _("/File/Close"),     None, self.callback_close, 0, None ),
		    ( _("/Experiments/_New"),     None, self.callback_add_page, 0, "<StockItem>", "gtk-new" ),
		    ( _("/Experiments/_Delete experiment"),     None, self.callback_delete_page, 0, "<StockItem>", "gtk-delete" ),
		    ( _("/Experiments/_Rename experiment"),     None, self.callback_rename_page, 0, "<StockItem>", "gtk-edit" ),
		    ( _("/Experiments/_Clone experiment"),     None, self.callback_copy_page, 0, "<StockItem>", "gtk-copy" ),
		    ( _("/_Help"),         None,         None, 0, "<LastBranch>" ),
		    ( _("/_Help/Help"),   None,         self.callback_help, 0, None ),
		    ( _("/_Help/About"),   None,         about_dialog_show, 0, "<StockItem>", "gtk-about" ),
		    )


		main_vbox = gtk.VBox(False, 3)

		menubar = self.get_main_menu(self)
		main_vbox.pack_start(menubar, False, False, 0)
		menubar.show()

		toolbar = gtk.Toolbar()
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 50)
		pos=0

		tb_new_scan = gtk.ToolButton(gtk.STOCK_NEW)
		tb_new_scan.connect("clicked", self.callback_add_page)
		self.tooltips.set_tip(tb_new_scan, _("New experiment"))

		toolbar.insert(tb_new_scan, pos)
		pos=pos+1

		delete = gtk.ToolButton(gtk.STOCK_DELETE)
		delete.connect("clicked", self.callback_delete_page,None)
		self.tooltips.set_tip(delete, _("Delete experiment"))
		toolbar.insert(delete, pos)
		pos=pos+1

		copy = gtk.ToolButton(gtk.STOCK_COPY)
		copy.connect("clicked", self.callback_copy_page,None)
		self.tooltips.set_tip(copy, _("Clone experiment"))
		toolbar.insert(copy, pos)
		pos=pos+1


		rename = gtk.ToolButton(gtk.STOCK_EDIT)
		rename.connect("clicked", self.callback_rename_page,None)
		self.tooltips.set_tip(rename, _("Rename experiment"))
		toolbar.insert(rename, pos)
		pos=pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(True)
		sep.set_expand(False)
		toolbar.insert(sep, pos)
		pos=pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(False)
		sep.set_expand(True)
		toolbar.insert(sep, pos)
		pos=pos+1

		tb_help = gtk.ToolButton(gtk.STOCK_HELP)
		tb_help.connect("clicked", self.callback_help)
		self.tooltips.set_tip(tb_help, "Help")
		toolbar.insert(tb_help, pos)
		pos=pos+1


		toolbar.show_all()
		main_vbox.pack_start(toolbar, False, False, 0)

		main_vbox.set_border_width(1)
		self.add(main_vbox)
		main_vbox.show()
	

		self.notebook = gtk.Notebook()
		self.notebook.show()
		self.notebook.set_tab_pos(gtk.POS_LEFT)

		self.load_tabs()
		main_vbox.pack_start(self.notebook, True, True, 0)
		main_vbox.pack_start(box, False, False, 0)

		self.connect("delete-event", self.callback_close)
		self.notebook.connect("switch-page",self.switch_page)
		self.set_icon_from_file(os.path.join(get_image_file_path(),"image.jpg"))

		self.hide()

