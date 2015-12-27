#!/usr/bin/env python
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



import sys

lib_dir='/usr/lib64/opvdm/'
sys.path.append('./gui/')
sys.path.append(lib_dir)

from win_lin import running_on_linux
from cal_path import get_exe_command
from cal_path import get_exe_name
from cal_path import find_data_file
from cal_path import calculate_paths
calculate_paths()

from ver import ver_load_info
from ver import ver_error
ver_load_info()

from command_args import command_args
command_args(len(sys.argv),sys.argv)


import pdb
import pygtk
import gtk
pygtk.require('2.0')
import os
import shutil
import signal
import subprocess
from inp import inp_get_token_value

from scan_item import scan_items_clear
from scan import scan_class 
from search import find_fit_error
from util import opvdm_clone
from export_as import export_as
from experiment import experiment
from copying import copying
from plot_gen import plot_gen
from plot_gen import set_plot_auto_close
from import_archive import import_archive
from about import about_dialog_show
from help import my_help_class
from notice import notice
import os
import gobject
from used_files_menu import used_files_menu
from plot import load_graph
from scan_item import scan_item_add
from cmp_class import cmp_class
from plot_state import plot_state
from window_list import windows
from config import config
import random
from undo import undo_list_class
from splash import splash_window
from ver import ver
import webbrowser
from debug import debug_mode
from qe import qe_window
from tb_item_sun import tb_item_sun
from tb_item_sim_mode import tb_item_sim_mode
from opvdm_open import opvdm_open

import glib
from server import server
from opvdm_notebook import opvdm_notebook
from gui_util import process_events
from epitaxy import epitaxy_load
from global_objects import global_object_register

from device_lib import device_lib_class

if running_on_linux()==True:
	import dbus
	from dbus.mainloop.glib import DBusGMainLoop
	import pynotify

	if os.geteuid() == 0:
		exit("Don't run me as root!!")
	
else:
	from windows_pipe import win_pipe



print notice()

gobject.threads_init()


def set_active_name(combobox, name):
    liststore = combobox.get_model()
    for i in xrange(len(liststore)):
        if liststore[i][0] == name:
            combobox.set_active(i)

class opvdm_main_window(gobject.GObject):

	icon_theme = gtk.icon_theme_get_default()
	plot_after_run=False
	plot_after_run_file=""
	scan_window=None

	notebook=opvdm_notebook()

	def adbus(self,bus, message):
		data=message.get_member()
		if data!=None:
			self.my_server.callback_dbus(data)

	def win_dbus(self, widget, data):
		self.my_server.callback_dbus(data)


	def gui_sim_start(self):
		self.notebook_active_page=self.notebook.get_current_page()
		self.notebook.goto_page("Terminal")
		self.spin.start()

	def gui_sim_stop(self,text):
		message=""
		self.notebook.set_current_page(self.notebook_active_page)
		self.spin.stop()

		if os.path.isfile("signal_stop.dat")==True:
			f = open('signal_stop.dat')
			lines = f.readlines()
			f.close()
			message=lines[0].rstrip()

		if text!="":
			message=text

		if running_on_linux()==True:
			if message!="":
				pynotify.init ("opvdm")
				Hello=pynotify.Notification ("opvdm:",message,find_data_file(os.path.join("gui","application-opvdm.svg")))
				Hello.set_timeout(2000)
				Hello.show ()


	def make_menu(self,event_button, event_time, data=None):
		menu = gtk.Menu()
		#open_item = gtk.MenuItem("Open App")
		close_item = gtk.MenuItem("Quit")
		#Append the menu items
		#menu.append(open_item)
		menu.append(close_item)
		#add callbacks
		#open_item.connect_object("activate", open_app, "Open App")
		close_item.connect_object("activate", self.callback_close_window, "Quit")
		#Show the menu items
		#open_item.show()
		close_item.show()
		#Popup the menu
		menu.popup(None, None, None, event_button, event_time)

	def on_status_icon_right_click(self,data, event_button, event_time):
		self.make_menu(event_button, event_time)


	def callback_plot_after_run_toggle(self, widget, data):
		self.plot_after_run=data.get_active()
		self.config.set_value("#plot_after_simulation",data.get_active())

	def callback_qe_window(self, widget):
		if self.qe_window==None:
			self.qe_window=qe_window()
			self.qe_window.init()

		if self.qe_window.get_property("visible")==True:
			self.qe_window.hide_all()
		else:
			self.qe_window.show_all()

	def callback_set_plot_auto_close(self, widget, data):
		set_plot_auto_close(data.get_active())
		self.config.set_value("#one_plot_window",data.get_active())


	def callback_run_scan(self, widget, data=None):
		if self.scan_window!=None:
			self.scan_window.callback_run_simulation(None)

	def callback_simulate(self, widget, data=None):

		if self.plot_after_run==True:
			try:
				ret= os.system("cp "+self.plot_after_run_file+" old.dat")
			except:
				pass

		self.my_server.clear_cache()
		self.my_server.add_job(os.getcwd())
		self.my_server.start()
	
	def callback_start_cluster_server(self, widget, data=None):
		self.goto_page("Terminal")

		cmd = "cd "+os.getcwd()+" \n"
		#self.terminal.feed_child(cmd)

		cmd = "./opvdm --server &\n"

		#self.terminal.feed_child(cmd)


		#self.spin.stop()

	def callback_simulate_stop(self, widget, data=None):
		cmd = 'killall '+get_exe_name()
		ret= os.system(cmd)
		self.spin.stop()

	def callback_run_fit(self, widget, data=None):
		if running_on_linux()==True:
			cmd = "cd "+os.getcwd()+" \n"
			self.terminal.feed_child(cmd)

			cmd = get_exe_command()+" --1fit&\n"
			self.terminal.feed_child(cmd)
		else:
			cmd = get_exe_command()+" --1fit"
			subprocess.Popen([cmd])


	def callback_scan(self, widget, data=None):
		my_help_class.help_set_help(["scan.png","<big><b>The scan window</b></big>\n Very often it is useful to be able to systematically very a device parameter such as mobility or density of trap states.  This window allows you to do just that.","add.png","Use the plus icon to add a new scan line to the list."])
		self.tb_run_scan.set_sensitive(True)

		if self.scan_window==None:
			self.scan_window=scan_class(gtk.WINDOW_TOPLEVEL)
			self.scan_window.init(self.my_server)


		if self.scan_window.get_property("visible")==True:
			self.scan_window.hide()
		else:
			self.scan_window.show()



	def callback_plot_select(self, widget, data=None):
		my_help_class.help_set_help(["dat_file.png","<big>Select a file to plot</big>\nSingle clicking shows you the content of the file"])

		dialog=opvdm_open()
		dialog.show_inp_files=False
		dialog.show_directories=False

		dialog.init(os.getcwd())
		response=dialog.run()

		if response == True:
			full_file_name=dialog.get_filename()
			#self.plot_open.set_sensitive(True)

			plot_gen([dialog.get_filename()],[],"auto")

			self.plotted_graphs.refresh()
			self.plot_after_run_file=dialog.get_filename()
		elif response == gtk.RESPONSE_CANCEL:
		    print 'Closed, no files selected'
		dialog.destroy()

	def callback_plot_open(self, widget, data=None):
		plot_gen([self.plot_after_run_file],[],"")

	def callback_last_menu_click(self, widget, data):
		#self.plot_open.set_sensitive(True)
		file_to_load=os.path.join(os.getcwd(),data.file0)
		plot_gen([file_to_load],[],"auto")
		self.plot_after_run_file=file_to_load


	def callback_import(self, widget, data=None):
		dialog = gtk.FileChooserDialog("Import an old opvdm simulation",
                               None,
                               gtk.FILE_CHOOSER_ACTION_OPEN,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_OPEN, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)

		filter = gtk.FileFilter()
		filter.set_name(".opvdm")
		filter.add_pattern("*.opvdm")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			import_archive(dialog.get_filename(),os.path.join(os.getcwd(),"sim.opvdm"),False)
			self.change_dir_and_refresh_interface(os.getcwd())
		elif response == gtk.RESPONSE_CANCEL:
		    print 'Closed, no files selected'
		dialog.destroy()

	def callback_import_from_lib(self, widget, data=None):
		device_lib=device_lib_class()
		device_lib.init()
		response=device_lib.run()
		path=device_lib.file_path
		if response == True:
			device_lib.destroy()
			import_archive(path,os.path.join(os.getcwd(),"sim.opvdm"),False)
			self.change_dir_and_refresh_interface(os.getcwd())
			print "file opened",path
		elif response == False:
			print "Closed, no files selected"
			device_lib.destroy()

		


	def callback_new(self, widget, data=None):
		dialog = gtk.FileChooserDialog("Make new opvdm simulation",
                               None,
                               gtk.FILE_CHOOSER_ACTION_OPEN,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_NEW, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)
		dialog.set_action(gtk.FILE_CHOOSER_ACTION_CREATE_FOLDER)

		filter = gtk.FileFilter()
		filter.set_name("All files")
		filter.add_pattern("*")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			if not os.path.exists(dialog.get_filename()):
				os.makedirs(dialog.get_filename())

			os.chdir(dialog.get_filename())
			opvdm_clone()

			self.change_dir_and_refresh_interface(dialog.get_filename())

		elif response == gtk.RESPONSE_CANCEL:
		    print 'Closed, no dir selected'
		dialog.destroy()

	def change_dir_and_refresh_interface(self,new_dir):
		print "rod",os.getcwd(),new_dir
 		scan_items_clear()
		os.chdir(new_dir)
		calculate_paths()
		epitaxy_load()
		self.config.load(os.getcwd())
		print "rod",os.getcwd(),new_dir
		self.status_bar.push(self.context_id, os.getcwd())
		#self.plot_open.set_sensitive(False)

		self.notebook.set_item_factory(self.item_factory)
		if self.notebook.load()==True:
			self.sim_mode.update()
			self.ti_light.connect('refresh', self.notebook.main_tab.update)
			self.play.set_sensitive(True)
			self.stop.set_sensitive(True)
			self.examine.set_sensitive(True)
			self.param_scan.set_sensitive(True)
			self.plot_select.set_sensitive(True)
			self.undo.set_sensitive(True)
			#self.save_sim.set_sensitive(True)
			self.experiment_window_button.set_sensitive(True)
			my_help_class.help_set_help(["play.png","<big><b>Now run the simulation</b></big>\n Click on the play icon to start a simulation."])

			my_item=self.item_factory.get_item("/File/Import data")
			if my_item!=None:
				my_item.set_sensitive(True)
			my_item=self.item_factory.get_item("/File/Export data")
			if my_item!=None:
				my_item.set_sensitive(True)
			my_item=self.item_factory.get_item("/File/Import data")
			if my_item!=None:
				my_item.set_sensitive(True)
			my_item=self.item_factory.get_item("/File/Import from library")
			if my_item!=None:
				my_item.set_sensitive(True)
			my_item=self.item_factory.get_item("/Simulate/Run")
			if my_item!=None:
				my_item.set_sensitive(True)
			my_item=self.item_factory.get_item("/Simulate/Parameter scan")
			if my_item!=None:
				my_item.set_sensitive(True)

		else:
			self.play.set_sensitive(False)
			self.stop.set_sensitive(False)
			self.examine.set_sensitive(False)
			self.param_scan.set_sensitive(False)
			self.plot_select.set_sensitive(False)
			self.undo.set_sensitive(False)
			#self.save_sim.set_sensitive(False)
			self.experiment_window_button.set_sensitive(False)
			my_help_class.help_set_help(["icon.png","<big><b>Hi!</b></big>\n I'm the on-line help system :).  If you find any bugs please report them to roderick.mackenzie@nottingham.ac.uk.","new.png","Click on the new icon to make a new simulation directory."])

			my_item=self.item_factory.get_item("/File/Import data")
			if my_item!=None:
				my_item.set_sensitive(False)
			my_item=self.item_factory.get_item("/File/Export data")
			if my_item!=None:
				my_item.set_sensitive(False)
			my_item=self.item_factory.get_item("/File/Import data")
			if my_item!=None:
				my_item.set_sensitive(False)
			my_item=self.item_factory.get_item("/File/Import from library")
			if my_item!=None:
				my_item.set_sensitive(False)
			my_item=self.item_factory.get_item("/Simulate/Run")
			if my_item!=None:
				my_item.set_sensitive(False)
			my_item=self.item_factory.get_item("/Simulate/Parameter scan")
			if my_item!=None:
				my_item.set_sensitive(False)

		if self.notebook.terminal!=None:
			self.my_server.set_terminal(self.notebook.terminal)
		self.notebook.show()

		self.plotted_graphs.init(os.getcwd(),self.callback_last_menu_click)

		set_active_name(self.light, inp_get_token_value("light.inp", "#Psun"))

		scan_item_add("sim.inp","#simmode","sim mode",1)
		scan_item_add("light.inp","#Psun","light intensity",1)


		if self.scan_window!=None:
			del self.scan_window
			self.scan_window=None

		if self.experiment_window!=None:
			del self.experiment_window
			self.experiment_window=None

		if self.qe_window!=None:
			del self.qe_window
			self.qe_window=None

		#myitem=self.item_factory.get_item("/Plots/One plot window")
		#myitem.set_active(self.config.get_value("#one_plot_window",False))
		#myitem=self.item_factory.get_item("/Plots/Plot after simulation")
		#myitem.set_active(self.config.get_value("#plot_after_simulation",False))

	def callback_open(self, widget, data=None):
		dialog = gtk.FileChooserDialog("Open an existing opvdm simulation",
                               None,
                               gtk.FILE_CHOOSER_ACTION_OPEN,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_OPEN, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)
		#dialog.set_action(gtk.FILE_CHOOSER_ACTION_SELECT_FILE)

		filter = gtk.FileFilter()
		filter.set_name("opvdm")
		filter.add_pattern("*.opvdm")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			new_path=os.path.dirname(dialog.get_filename())
			self.change_dir_and_refresh_interface(new_path)

		elif response == gtk.RESPONSE_CANCEL:
		    print 'Closed, no files selected'
		dialog.destroy()

	def callback_export(self, widget, data=None):
		dialog = gtk.FileChooserDialog("Export the simulation as", None, gtk.FILE_CHOOSER_ACTION_SAVE,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_SAVE, gtk.RESPONSE_OK))

		dialog.set_default_response(gtk.RESPONSE_OK)

		filter = gtk.FileFilter()
		filter.set_name(".opvdm")
		filter.add_pattern("*.opvdm")
		dialog.add_filter(filter)

		filter = gtk.FileFilter()
		filter.set_name(".pdf")
		filter.add_pattern("*.pdf")
		dialog.add_filter(filter)

		filter = gtk.FileFilter()
		filter.set_name(".jpg")
		filter.add_pattern("*.jpg")
		dialog.add_filter(filter)

		filter = gtk.FileFilter()
		filter.set_name(".tex")
		filter.add_pattern("*.tex")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			file_name=dialog.get_filename()
			mode=self.sim_mode.get_active_text()
			filter=dialog.get_filter()
			dialog.destroy()

			if os.path.splitext(file_name)[1]:
				export_as(file_name)
			else:
				export_as(file_name+filter.get_name())
			
		elif response == gtk.RESPONSE_CANCEL:
			print 'Closed, no files selected'
			dialog.destroy()

	def callback_about_dialog(self, widget, data=None):
		about_dialog_show()

	def callback_help(self, widget, data=None):
		my_help_class.toggle_visible()

	def callback_on_line_help(self, widget, data=None):
		webbrowser.open('www.opvdm.com')

	def callback_new_window(self, widget, data=None):
		if self.window2.get_property("visible")==True:
			self.window2.hide()
		else:
			self.window2.show()

	def callback_close_window2(self, widget, data=None):
		self.window2.hide()
		return True


	def callback_close_window(self, widget, data=None):
		self.win_list.update(self.window,"main_window")
		gtk.main_quit()


	def callback_examine(self, widget, data=None):
		my_help_class.help_set_help(["plot_time.png","<big><b>Examine the results in time domain</b></big>\n After you have run a simulation in time domain, if is often nice to be able to step through the simulation and look at the results.  This is what this window does.  Use the slider bar to move through the simulation.  When you are simulating a JV curve, the slider sill step through voltage points rather than time points."])
		mycmp=cmp_class()
		ret=mycmp.init()
		if ret==False:
			md = gtk.MessageDialog(None, gtk.DIALOG_DESTROY_WITH_PARENT, gtk.MESSAGE_WARNING,  gtk.BUTTONS_CLOSE, "Re-run the simulation with 'dump all slices' set to one to use this tool.")
        		md.run()
        		md.destroy()
			return

	def callback_edit_experiment_window(self, widget, data=None):

		if self.experiment_window==None:
			self.experiment_window=experiment()
			self.experiment_window.init()

		my_help_class.help_set_help(["time.png","<big><b>The time mesh editor</b></big>\n To do time domain simulations one must define how voltage the light vary as a function of time.  This can be done in this window.  Also use this window to define the simulation length and time step."])
		if self.experiment_window.get_property("visible")==True:
			self.experiment_window.hide_all()
		else:
			self.experiment_window.show_all()

	def callback_undo(self, widget, data=None):
		l=self.undo_list.get_list()
		if len(l)>0:
			value=l[len(l)-1][2]
			w_type=l[len(l)-1][3]

			if type(w_type)==gtk.Entry:
				self.undo_list.disable()
				w_type.set_text(value)
				self.undo_list.enable()

			l.pop()


	def callback_make(self, widget, data=None):
		cmd = 'make clean'
		os.system(cmd)

		cmd = 'make'
		os.system(cmd)

	def get_main_menu(self, window):
		accel_group = gtk.AccelGroup()


		item_factory = gtk.ItemFactory(gtk.MenuBar, "<main>", accel_group)

		item_factory.create_items(self.menu_items)


		if debug_mode()==False:
			item_factory.delete_item("/Advanced")
			item_factory.delete_item("/Simulate/Start cluster server")


		window.add_accel_group(accel_group)

		self.item_factory = item_factory

		return item_factory.get_widget("<main>")


	def make_tool_box1(self):
		pos=0
		toolbar = gtk.Toolbar()
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(900, 50)
		toolbar.show()

		if debug_mode()==True:
			image = gtk.Image()
	   		image.set_from_file(find_data_file(os.path.join("gui","qe.png")))
			self.qe_button = gtk.ToolButton(image)
			self.tooltips.set_tip(self.qe_button, "Quantum efficiency")
			self.qe_button.connect("clicked", self.callback_qe_window)
			toolbar.insert(self.qe_button, pos)
			self.qe_button.show_all()
			pos=pos+1

		self.sim_mode=tb_item_sim_mode()
		self.sim_mode.init()
		global_object_register("tb_item_sim_mode_update",self.sim_mode.update)
		toolbar.insert(self.sim_mode, pos)
		pos=pos+1

		self.ti_light=tb_item_sun()
		self.light=self.ti_light.light

		toolbar.insert(self.ti_light, pos)
		pos=pos+1

		ti_progress = gtk.ToolItem()
		hbox = gtk.HBox(False, 2)

		sep = gtk.SeparatorToolItem()

		sep.set_draw(False)
		sep.set_expand(True)
		sep.show_all()

		toolbar.insert(sep, pos)
		pos=pos+1

		self.spin=gtk.Spinner()
		self.spin.set_size_request(32, 32)
		self.spin.show()
		self.spin.stop()


		gap=gtk.Label(" ")
		hbox.add(gap)
		hbox.add(self.spin)	
		hbox.set_child_packing(self.spin, False, False, 0, 0)


		gap.show()
		hbox.show()

		ti_progress.add(hbox)

		toolbar.insert(ti_progress, pos)
		pos=pos+1
		ti_progress.show()
		return toolbar

	def __init__(self):

		gobject.GObject.__init__(self)

		self.my_server=server()
		self.my_server.init(os.getcwd())
		self.my_server.statusicon.connect('popup-menu', self.on_status_icon_right_click)
		self.my_server.setup_gui(self.gui_sim_start,self.gui_sim_stop)

		if running_on_linux()==True:
			DBusGMainLoop(set_as_default=True)
			self.bus = dbus.SessionBus()
			self.bus.add_match_string_non_blocking("type='signal',interface='org.my.opvdm'")
			self.bus.add_message_filter(self.adbus)

		else:
			self.win_pipe=win_pipe()
			self.win_pipe.connect('new-data', self.win_dbus)
			self.win_pipe.start()

		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
		#self.window.set_size_request(-1,1000)
		self.window.set_border_width(10)
		self.window.set_title("Organic Photovoltaic Device Model (www.opvdm.com)")

		splash=splash_window()
		splash.init()

		temp_error=ver_error()
		print temp_error
		if len(temp_error)>0:
				md = gtk.MessageDialog(self.window,gtk.DIALOG_DESTROY_WITH_PARENT, gtk.MESSAGE_ERROR, gtk.BUTTONS_CLOSE, temp_error)
				md.run()
				md.destroy()

		self.undo_list=undo_list_class()
		self.undo_list.init()


		self.experiment_window=None

		self.qe_window=None

		self.win_list=windows()
		self.win_list.load()

		self.config=config()
		#table = gtk.Table(3,6,False)

		self.window.set_icon_from_file(find_data_file("gui/image.jpg"))

		

		self.show_tabs = True
		self.show_border = True

		self.menu_items = (
		    ( "/_File",         None,         None, 0, "<Branch>" ),
			("/File/_New simulation", "<control>N", self.callback_new, 0, "<StockItem>", "gtk-new" ),
			("/File/_Open simulation", "<control>O", self.callback_open, 0, "<StockItem>", "gtk-open" ),
		    ( "/File/_Export data",     None, self.callback_export, 0, "<StockItem>", "gtk-save" ),
		    ( "/File/Import data",     None, self.callback_import, 0 , "<StockItem>", "gtk-harddisk"),
		    ( "/File/Import from library",     None, self.callback_import_from_lib, 0 , "<StockItem>", "gtk-harddisk"),
		    ( "/File/Quit",     "<control>Q", gtk.main_quit, 0, "<StockItem>", "gtk-quit" ),
		    ( "/_Simulate",      None,         None, 0, "<Branch>" ),
		    ( "/Simulate/Run",  None,         self.callback_simulate, 0, "<StockItem>", "gtk-media-play" ),
		    ( "/Simulate/Parameter scan",  None,         self.callback_scan , 0, None ),
		    ( "/Simulate/Start cluster server",  None,         self.callback_start_cluster_server , 0, None ),
		    ( "/_View",      None,         None, 0, "<Branch>" ),
		    ( "/_Plots",      None,         None, 0, "<Branch>" ),
		    ( "/Plots/Plot simulation result",  None,         self.callback_plot_select, 0, "<StockItem>", "gtk-open"),
		    ( "/_Plots/",     None, None, 0, "<Separator>" ),
		    ( "/_Help",         None,         None, 0, "<LastBranch>" ),
			( "/_Help/Help Index",   None,         self.callback_help, 0, "<StockItem>", "gtk-help"  ),
			

		    ( "/_Help/About",   None, self.callback_about_dialog, 0, "<StockItem>", "gtk-about" ),
		    )
		pos=0

		self.menubar = self.get_main_menu(self.window)

		#a = (( "/Plots/Plot after simulation",  None, self.callback_plot_after_run_toggle, 0, "<ToggleItem>" ),   )
		#self.item_factory.create_items( a, )


		#a = (( "/Plots/One plot window",  None, self.callback_set_plot_auto_close, 0, "<ToggleItem>" ),   )
		#self.item_factory.create_items( a, )


		#table.show()
		self.window.connect("destroy", gtk.main_quit)

		self.tooltips = gtk.Tooltips()

		self.window.set_size_request(-1, 780)
		main_vbox = gtk.VBox(False, 5)
		main_vbox.set_border_width(1)
		self.window.add(main_vbox)
		#window.add(table)
		main_vbox.show()


		toolbar = gtk.Toolbar()
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 50)

		open_sim = gtk.ToolButton(gtk.STOCK_OPEN)
		self.tooltips.set_tip(open_sim, "Open a simulation")
		toolbar.insert(open_sim, pos)
		pos=pos+1

		#self.save_sim = gtk.ToolButton(gtk.STOCK_SAVE)
		#self.tooltips.set_tip(self.save_sim, "Save a simulation")
		#toolbar.insert(self.save_sim, pos)
		#pos=pos+1

		new_sim = gtk.ToolButton(gtk.STOCK_NEW)
		self.tooltips.set_tip(new_sim, "Make a new simulation")
		toolbar.insert(new_sim, pos)
		pos=pos+1

		sep_lhs = gtk.SeparatorToolItem()
		sep_lhs.set_draw(True)
		sep_lhs.set_expand(False)
		toolbar.insert(sep_lhs, pos)
		pos=pos+1

		self.undo = gtk.ToolButton(gtk.STOCK_UNDO)
		self.tooltips.set_tip(self.undo, "Undo")
		toolbar.insert(self.undo, pos)
		self.undo.connect("clicked", self.callback_undo)
		pos=pos+1

		sep_lhs = gtk.SeparatorToolItem()
		sep_lhs.set_draw(True)
		sep_lhs.set_expand(False)
		toolbar.insert(sep_lhs, pos)
		pos=pos+1

	        image = gtk.Image()
   		image.set_from_file(find_data_file(os.path.join("gui","play.png")))
		self.play = gtk.ToolButton(image)
		self.tooltips.set_tip(self.play, "Run the simulation")
		toolbar.insert(self.play, pos)
		self.play.connect("clicked", self.callback_simulate)
		pos=pos+1

		image = gtk.Image()
   		image.set_from_file(find_data_file(os.path.join("gui","forward.png")))
		self.tb_run_scan = gtk.ToolButton(image)
		self.tb_run_scan.connect("clicked", self.callback_run_scan)
		self.tooltips.set_tip(self.tb_run_scan, "Run parameter scan")
		toolbar.insert(self.tb_run_scan, pos)
		self.tb_run_scan.set_sensitive(False)
		pos=pos+1

		if debug_mode()==True:
			image = gtk.Image()
	   		image.set_from_file(find_data_file(os.path.join("gui","fit.png")))
			self.tb_run_fit = gtk.ToolButton(image)
			self.tb_run_fit.connect("clicked", self.callback_run_fit)
			self.tooltips.set_tip(self.tb_run_fit, "Run a fit command")
			toolbar.insert(self.tb_run_fit, pos)
			self.tb_run_fit.set_sensitive(True)
			pos=pos+1

	        image = gtk.Image()
   		image.set_from_file(find_data_file(os.path.join("gui","pause.png")))
		self.stop = gtk.ToolButton(image )
		self.tooltips.set_tip(self.stop, "Stop the simulation")
		self.stop.connect("clicked", self.callback_simulate_stop)
		toolbar.insert(self.stop, pos)
		pos=pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(True)
		sep.set_expand(False)
		toolbar.insert(sep, pos)
		pos=pos+1

		image = gtk.Image()
   		image.set_from_file(find_data_file(os.path.join("gui","scan.png")))
		self.param_scan = gtk.ToolButton(image)
		self.param_scan.connect("clicked", self.callback_scan)
		self.tooltips.set_tip(self.param_scan, "Parameter scan")
		toolbar.insert(self.param_scan, pos)
		pos=pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(True)
		sep.set_expand(False)
		toolbar.insert(sep, pos)
		pos=pos+1

	        image = gtk.Image()
   		image.set_from_file(find_data_file(os.path.join("gui","plot.png")))
		self.plot_select = gtk.MenuToolButton(image,"hello")
		self.tooltips.set_tip(self.plot_select, "Find a file to plot")
		self.plotted_graphs = used_files_menu()
		self.plot_select.set_menu(self.plotted_graphs.menu)
		toolbar.insert(self.plot_select, pos)
		self.plot_select.connect("clicked", self.callback_plot_select)
		self.plot_select.set_sensitive(False)
		pos=pos+1

		#image = gtk.Image()
   		#image.set_from_file(find_data_file(os.path.join("gui","refresh.png")))
		#self.plot_open = gtk.ToolButton(image)
		#self.tooltips.set_tip(self.plot_open, "Replot the graph")
		#toolbar.insert(self.plot_open, pos)
		#self.plot_open.set_sensitive(False)
		#os=pos+1

		image = gtk.Image()
   		image.set_from_file(find_data_file(os.path.join("gui","plot_time.png")))
		self.examine = gtk.ToolButton(image)
		self.tooltips.set_tip(self.examine, "Examine results in time domain")
		self.examine.connect("clicked", self.callback_examine)
		toolbar.insert(self.examine, pos)
		pos=pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(True)
		sep.set_expand(False)
		toolbar.insert(sep, pos)
		pos=pos+1



		image = gtk.Image()
	   	image.set_from_file(find_data_file(os.path.join("gui","time.png")))
		self.experiment_window_button = gtk.ToolButton(image)
		self.tooltips.set_tip(self.experiment_window_button, "Edit the time mesh")
		self.experiment_window_button.connect("clicked", self.callback_edit_experiment_window)
		toolbar.insert(self.experiment_window_button, pos)
		pos=pos+1



		sep2 = gtk.SeparatorToolItem()
		sep2.set_draw(False)
		sep2.set_expand(True)
		toolbar.insert(sep2, pos)
		pos=pos+1


		help = gtk.ToolButton(gtk.STOCK_HELP)
		self.tooltips.set_tip(help, "Help")
		help.connect("clicked", self.callback_help)
		toolbar.insert(help, pos)
		pos=pos+1


		#quittb = gtk.ToolButton(gtk.STOCK_QUIT)
		#self.tooltips.set_tip(quittb, "Quit")
		#toolbar.insert(quittb, pos)
		#quittb.connect("clicked", gtk.main_quit)
		#pos=pos+1

		new_sim.connect("clicked", self.callback_new)
		open_sim.connect("clicked", self.callback_open)
		#self.save_sim.connect("clicked", self.callback_export)

		#self.plot_open.connect("clicked", self.callback_plot_open)

		toolbar1=self.make_tool_box1()


		toolbar.show_all()



		main_vbox.pack_start(self.menubar, False, True, 0)
		handlebox = gtk.HandleBox()
		handlebox.set_snap_edge(gtk.POS_LEFT)
		handlebox.show()

		toolbar.set_size_request(1000, -1)

		tb_vbox=gtk.VBox()
		tb_vbox.add(toolbar)
		tb_vbox.add(toolbar1)
		tb_vbox.show()



		handlebox.add(tb_vbox)

		main_vbox.pack_start(handlebox, False, False, 0)

		self.window.connect("delete-event", self.callback_close_window) 

		self.win_list.set_window(self.window,"main_window")



		self.menubar.show()

		self.make_window2(main_vbox)

		self.window.show()


		process_events()


	def make_window2(self,main_vbox):

		box=gtk.HBox()
		self.status_bar = gtk.Statusbar()      
		self.context_id = self.status_bar.get_context_id("Statusbar example")
		self.status_bar.push(self.context_id, os.getcwd())

		box.add(self.status_bar)

		self.window2_box=gtk.VBox()
		self.window2_box.add(self.notebook)
		self.window2_box.add(box)
		self.window2_box.set_child_packing(box, False, False, 0, 0)
		self.window2_box.set_size_request(-1, 550)
		self.window2 = gtk.Window(gtk.WINDOW_TOPLEVEL)
		self.window2.set_border_width(10)

		self.window2.set_title("Organic Photovoltaic Device Model (www.opvdm.com)")
		self.window2.connect("delete-event", self.callback_close_window2)

		self.window2.set_icon_from_file(find_data_file(os.path.join("gui","image.jpg")))
		if main_vbox==None:
			self.window2.add(self.window2_box)
		else:
			main_vbox.add(self.window2_box)			

		box.show()
		self.status_bar.show()
		self.window2_box.show()

		self.change_dir_and_refresh_interface(os.getcwd())
		if main_vbox==None:
			self.window2.show()
		my_help_class.show()

if __name__ == "__main__":
	main=opvdm_main_window()
	random.seed()
	gtk.main()


