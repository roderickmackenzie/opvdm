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
from util import opvdm_delete_file
from util import delete_second_level_link_tree
from util import copy_scan_dir
from search import return_file_list
from win_lin import running_on_linux
import webbrowser
from search import find_fit_log
from scan_io import get_scan_dirs
from hpc import hpc_class
from debug import debug_mode
from inp import inp_update_token_value
from inp import inp_get_token_value

import i18n
_ = i18n.language.gettext

class scan_class(gtk.Window):

	def callback_cluster(self, widget, data=None):
		if self.cluster_window==None:
			self.cluster_window=hpc_class()
			self.cluster_window.init(self.hpc_root_dir,self.myserver.terminal)

		print self.cluster_window.get_property("visible")

		if self.cluster_window.get_property("visible")==True:
			self.cluster_window.hide()
		else:
			self.cluster_window.show()

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
		self.win_list.update(self,"scan_window")
		self.hide()
		return True

	def callback_change_dir(self, widget, data=None):
		dialog = gtk.FileChooserDialog(_("Change directory"),
                               None,
                               gtk.FILE_CHOOSER_ACTION_OPEN,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_OK, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)
		dialog.set_action(gtk.FILE_CHOOSER_ACTION_CREATE_FOLDER)

		filter = gtk.FileFilter()
		filter.set_name(_("All files"))
		filter.add_pattern("*")
		dialog.add_filter(filter)


		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			self.sim_dir=dialog.get_filename()

	 		a = open("scan_window.inp", "w")
			a.write(self.sim_dir)
			a.close()

			self.clear_pages()
			self.load_tabs()
			dialog.destroy()

		return True

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.opvdm.com/man/index.html')

	def callback_add_page(self, widget, data=None):
		new_sim_name=dlg_get_text( _("New simulation name:"), _("Simulation ")+str(self.number_of_tabs+1))

		if new_sim_name!=None:
			new_sim_name=self.remove_invalid(new_sim_name)
			name=os.path.join(os.getcwd(),new_sim_name)
			self.add_page(name)

	def callback_remove_page(self,widget,name):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		self.toggle_tab_visible(tab.tab_name)

	def callback_cluster_sleep(self,widget,data):
		self.myserver.sleep()

	def callback_cluster_poweroff(self,widget,data):
		self.myserver.poweroff()

	def callback_cluster_get_data(self,widget):
		self.myserver.get_data()

	def callback_cluster_print_jobs(self,widget):
		self.myserver.print_jobs()

	def callback_cluster_fit_log(self,widget):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		name=tab.tab_name
		path=os.path.join(self.sim_dir,name)
		find_fit_log("./fit.dat",path)
		os.system("gnuplot -persist ./fit.dat &\n")


	def callback_copy_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		name=tab.tab_name
		old_dir=os.path.join(self.sim_dir,name)
		new_sim_name=dlg_get_text( _("Clone the current simulation to a new simulation called:"), name)
		if new_sim_name!=None:
			new_sim_name=self.remove_invalid(new_sim_name)
			new_dir=os.path.join(self.sim_dir,new_sim_name)

			copy_scan_dir(new_dir,old_dir)
			print _("I want to copy"),new_dir,old_dir
			self.add_page(new_sim_name)

	def callback_run_simulation(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.simulate(True,True)

	def callback_build_simulation(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.simulate(False,True)

	def callback_run_simulation_no_build(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.simulate(True,False)

	def callback_nested_simulation(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.nested_simulation()


	def callback_clean_simulation(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.clean_scan_dir()

	def callback_clean_unconverged_simulation(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.scan_clean_unconverged()

	def callback_clean_simulation_output(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.scan_clean_simulation_output()

	def callback_import_from_hpc(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.import_from_hpc()

	def callback_push_to_hpc(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.push_to_hpc()

	def callback_push_unconverged_to_hpc(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.push_unconverged_to_hpc()

	def callback_set_hpc_dir(self,widget,data):
		config_file=os.path.join(self.sim_dir,"server.inp")
		hpc_path=inp_get_token_value(config_file, "#hpc_dir")

		dialog = gtk.FileChooserDialog(_("Select HPC dir"),
                               None,
                               gtk.FILE_CHOOSER_ACTION_OPEN,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_OK, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)
		dialog.set_action(gtk.FILE_CHOOSER_ACTION_SELECT_FOLDER)
		if os.path.isdir(hpc_path):
			dialog.set_current_folder(hpc_path)

		filter = gtk.FileFilter()
		filter.set_name(_("All files"))
		filter.add_pattern("*")
		dialog.add_filter(filter)


		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			inp_update_token_value(config_file, "#hpc_dir", dialog.get_filename(),1)

		dialog.destroy()

	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_rename_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		name=tab.tab_name
		old_dir=os.path.join(self.sim_dir,name)
		new_sim_name=dlg_get_text( _("Rename the simulation to be called:"), name)

		if new_sim_name!=None:
			new_sim_name=self.remove_invalid(new_sim_name)
			new_dir=os.path.join(self.sim_dir,new_sim_name)
			shutil.move(old_dir, new_dir)
			tab.rename(new_dir)

	def callback_delete_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		name=tab.tab_name
		dir_to_del=os.path.join(self.sim_dir,name)

		md = gtk.MessageDialog(None, 0, gtk.MESSAGE_QUESTION,  gtk.BUTTONS_YES_NO, _("Should I remove the simulation directory ")+dir_to_del)

#gtk.MessageDialog(self, gtk.DIALOG_DESTROY_WITH_PARENT, gtk.MESSAGE_QUESTION, 
		# gtk.BUTTONS_CLOSE, "Should I remove the simulation directory "+dir_to_del)
		response = md.run()

		if response == gtk.RESPONSE_YES:


			self.notebook.remove_page(pageNum)

			for items in self.tab_menu.get_children():
				if items.get_label()==name:
					self.tab_menu.remove(items)


			print _("I am going to delete file"),dir_to_del
			delete_second_level_link_tree(dir_to_del)
			self.number_of_tabs=self.number_of_tabs-1
		elif response == gtk.RESPONSE_NO:
			print _("Not deleting")


		md.destroy()

	def toggle_tab_visible(self,name):
		tabs_open=0
		print name
		for i in range(0, self.number_of_tabs):
			if self.rod[i].visible==True:
				tabs_open=tabs_open+1

		#print "tabs open",tabs_open,self.number_of_tabs

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
		#print "one",widget.get_label()
		self.toggle_tab_visible(widget.get_label())

	def callback_view_toggle_tab(self, widget, data):
		self.toggle_tab_visible(data)

	def callback_run_all_simulations(self,widget):
		for i in range(0,self.notebook.get_n_pages()):
			tab = self.notebook.get_nth_page(i)
			tab.simulate(True,True)


	def callback_stop_simulation(self,widget):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.stop_simulation()

	def load_tabs(self):
		sim_dirs=[]

		get_scan_dirs(sim_dirs,self.sim_dir)

		print sim_dirs,self.sim_dir
		
		if len(sim_dirs)==0:
			sim_dirs.append("scan1")
		else:
			for i in range(0,len(sim_dirs)):
				sim_dirs[i]=sim_dirs[i]

		for i in range(0,len(sim_dirs)):
			self.add_page(sim_dirs[i])

	def clear_pages(self):
		for items in self.tab_menu.get_children():
			self.tab_menu.remove(items)

		for child in self.notebook.get_children():
    			self.notebook.remove(child)

		self.rod=[]

	def add_page(self,name):

		hbox=gtk.HBox()

		hbox.set_size_request(-1, 25)
		label=gtk.Label("")
		sim_name=os.path.basename(os.path.normpath(name))
		print "Looking for",sim_name,name
		self.rod.append(scan_vbox())
		self.rod[len(self.rod)-1].init(self.myserver,self.tooltips,self.status_bar,self.context_id,label,self.sim_dir,sim_name)
		label.set_justify(gtk.JUSTIFY_LEFT)
		hbox.pack_start(label, False, True, 0)

		button = gtk.Button()
		close_image = gtk.Image()
   		close_image.set_from_file(find_data_file(os.path.join("gui","close.png")))
		close_image.show()
		button.add(close_image)
		button.props.relief = gtk.RELIEF_NONE
		button.connect("clicked", self.callback_view_toggle_tab,self.rod[len(self.rod)-1].tab_name)
		button.set_size_request(25, 25)
		button.show()

		hbox.pack_end(button, False, False, 0)
		hbox.show_all()
		self.notebook.append_page(self.rod[len(self.rod)-1],hbox)
		self.notebook.set_tab_reorderable(self.rod[len(self.rod)-1],True)

		menu_item = gtk.CheckMenuItem(sim_name)		   
		menu_item.set_active(True)
		self.tab_menu.append(menu_item)
		menu_item.show()
		menu_item.set_active(self.rod[len(self.rod)-1].visible)
		#print "Rod",name,self.rod[len(self.rod)-1].visible
		menu_item.connect("activate", self.callback_view_toggle,menu_item)

		self.number_of_tabs=self.number_of_tabs+1

	def callback_last_menu_click(self, widget, data):
		print [data]

	def switch_page(self,page, page_num, user_param1):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		self.status_bar.push(self.context_id, tab.sim_dir)

	def callback_remove_all_results(self, widget, data):
		results=[]
		return_file_list(results,self.sim_dir,"scan.inp")
		for i in range(0,len(results)):
			dir_name=os.path.dirname(results[i])
			if os.path.isdir(dir_name):
				print "delete:",dir_name
				#opvdm_delete_file(dir_name)

	def callback_wol(self, widget, data):
		self.myserver.wake_nodes()

	def init(self,my_server):
		self.cluster_window=None
		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"scan_window")
		print "constructur"

		self.rod=[]
		if os.path.isfile("scan_window.inp"):
			f = open("scan_window.inp")
			lines = f.readlines()
			f.close()

			path=lines[0].strip()
			if path.startswith(os.getcwd()):
				self.sim_dir=path
			else:
				self.sim_dir=os.getcwd()
		else:
			self.sim_dir=os.getcwd()

		self.tooltips = gtk.Tooltips()

		self.set_border_width(2)
		self.set_title(_("Parameter scan - opvdm"))

		n=0

		self.hpc_root_dir= os.path.abspath(os.getcwd()+'/../')

		self.number_of_tabs=0
		items=0

		self.status_bar = gtk.Statusbar()      

		self.status_bar.show()

		self.context_id = self.status_bar.get_context_id("Statusbar example")

		box=gtk.HBox()
		box.add(self.status_bar)
		box.set_child_packing(self.status_bar, True, True, 0, 0)
		box.show()


		self.menu_items = (
		    ( _("/_File"),         None,         None, 0, "<Branch>" ),
		    ( _("/File/Change dir"),     None, self.callback_change_dir, 0, None ),
		    ( _("/File/Close"),     None, self.callback_close, 0, None ),
		    ( _("/Simulations/_New"),     None, self.callback_add_page, 0, "<StockItem>", "gtk-new" ),
		    ( _("/Simulations/_Delete simulaton"),     None, self.callback_delete_page, 0, "<StockItem>", "gtk-delete" ),
		    ( _("/Simulations/_Rename simulation"),     None, self.callback_rename_page, 0, "<StockItem>", "gtk-edit" ),
		    ( _("/Simulations/_Clone simulation"),     None, self.callback_copy_page, 0, "<StockItem>", "gtk-copy" ),
			( _("/Simulations/sep1"),     None, None, 0, "<Separator>" ),
		    ( _("/Simulations/_Run simulation"),     None, self.callback_run_simulation, 0, "<StockItem>", "gtk-media-play" ),
		    ( _("/Advanced/_Build simulation"),     None, self.callback_build_simulation, 0, "<StockItem>", "gtk-cdrom" ),
			( _("/Advanced/_Run (no build)"),     None, self.callback_run_simulation_no_build, 0, "<StockItem>", "gtk-media-play" ),
			( _("/Advanced/_Run nested simulation"),     None, self.callback_nested_simulation, 0, "<StockItem>", "gtk-media-play" ),
			( _("/Advanced/_Clean simulation"),     None, self.callback_clean_simulation, 0, "<StockItem>", "gtk-clear" ),
			( _("/Advanced/_Clean unconverged simulation"),     None, self.callback_clean_unconverged_simulation, 0, "<StockItem>", "gtk-clear" ),
			( _("/Advanced/_Clean simulation output"),     None, self.callback_clean_simulation_output, 0, "<StockItem>", "gtk-clear" ),
			( _("/Advanced/sep2"),     None, None, 0, "<Separator>" ),
			( _("/Advanced/_Import from hpc"),     None, self.callback_import_from_hpc, 0, "<StockItem>", "gtk-open" ),
			( _("/Advanced/_Push to hpc"),     None, self.callback_push_to_hpc, 0, "<StockItem>", "gtk-save" ),
			( _("/Advanced/_Push unconverged to hpc"),     None, self.callback_push_unconverged_to_hpc, 0, "<StockItem>", "gtk-save" ),
			( _("/Advanced/_Set hpc dir"),     None, self.callback_set_hpc_dir, 0, "<StockItem>", "gtk-open" ),

		    ( _("/Advanced/_Cluster sleep"),     None, self.callback_cluster_sleep, 0, "<StockItem>", "gtk-copy" ),
		    ( _("/Advanced/_Cluster poweroff"),     None, self.callback_cluster_poweroff, 0, "<StockItem>", "gtk-copy" ),
		    ( _("/Advanced/_Cluster wake"),     None, self.callback_wol, 0, "<StockItem>", "gtk-copy" ),

		    ( _("/Advanced/_Remove all results"),     None, self.callback_remove_all_results, 0, "<StockItem>", "gtk-copy" ),
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

		#image = gtk.Image()
		#image.set_from_file(find_data_file(os.path.join("gui","new-tab.png")))
		tb_new_scan = gtk.MenuToolButton(gtk.STOCK_NEW)
		tb_new_scan.connect("clicked", self.callback_add_page)
		self.tooltips.set_tip(tb_new_scan, _("New simulation"))

		self.tab_menu=gtk.Menu()
		tb_new_scan.set_menu(self.tab_menu)

		toolbar.insert(tb_new_scan, pos)
		pos=pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(True)
		sep.set_expand(False)
		toolbar.insert(sep, pos)
		pos=pos+1

		delete = gtk.ToolButton(gtk.STOCK_DELETE)
		delete.connect("clicked", self.callback_delete_page,None)
		self.tooltips.set_tip(delete, _("Delete simulation"))
		toolbar.insert(delete, pos)
		pos=pos+1

		copy = gtk.ToolButton(gtk.STOCK_COPY)
		copy.connect("clicked", self.callback_copy_page,None)
		self.tooltips.set_tip(copy, _("Clone simulation"))
		toolbar.insert(copy, pos)
		pos=pos+1


		rename = gtk.ToolButton(gtk.STOCK_EDIT)
		rename.connect("clicked", self.callback_rename_page,None)
		self.tooltips.set_tip(rename, _("Rename simulation"))
		toolbar.insert(rename, pos)
		pos=pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(True)
		sep.set_expand(False)
		toolbar.insert(sep, pos)
		pos=pos+1

		image = gtk.Image()
		image.set_from_file(find_data_file(os.path.join("gui","forward2.png")))
		tb_simulate = gtk.ToolButton(image)
		tb_simulate.connect("clicked", self.callback_run_all_simulations)
		self.tooltips.set_tip(tb_simulate, _("Run all simulation"))
		toolbar.insert(tb_simulate, pos)
		pos=pos+1

		if debug_mode()==True:
			sep = gtk.SeparatorToolItem()
			sep.set_draw(True)
			sep.set_expand(False)
			toolbar.insert(sep, pos)
			pos=pos+1

			image = gtk.Image()
	   		image.set_from_file(find_data_file(os.path.join("gui","server.png")))
			cluster = gtk.ToolButton(image)
			cluster.connect("clicked", self.callback_cluster)
			self.tooltips.set_tip(cluster, _("Configure cluster"))
			toolbar.insert(cluster, pos)
			cluster.show()
			pos=pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(False)
		sep.set_expand(True)
		toolbar.insert(sep, pos)
		pos=pos+1

		tb_help = gtk.ToolButton(gtk.STOCK_HELP)
		tb_help.connect("clicked", self.callback_help)
		self.tooltips.set_tip(tb_help, _("Help"))
		toolbar.insert(tb_help, pos)
		pos=pos+1


		toolbar.show_all()
		main_vbox.pack_start(toolbar, False, False, 0)
		#main_vbox.add(toolbar)

		main_vbox.set_border_width(1)
		self.add(main_vbox)
		main_vbox.show()
		self.myserver=my_server
		

		self.notebook = gtk.Notebook()
		self.notebook.show()
		self.notebook.set_tab_pos(gtk.POS_LEFT)

		self.load_tabs()
		main_vbox.pack_start(self.notebook, True, True, 0)
		main_vbox.pack_start(box, False, False, 0)

		self.connect("delete-event", self.callback_close)
		self.notebook.connect("switch-page",self.switch_page)
		self.set_icon_from_file(find_data_file(os.path.join("gui","image.jpg")))

		self.hide()

