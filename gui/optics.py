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
from inp import inp_update_token_value
from inp import inp_get_token_value
import gobject
import os, fnmatch
from plot_gen import plot_gen
from cal_path import get_image_file_path
import zipfile
import glob
from scan_item import scan_item_add
from tab import tab_class
from win_lin import running_on_linux
from photon_dist import photon_dist_class
from plot_widget import plot_widget
from plot_state import plot_state
from plot_io import plot_load_info
import webbrowser
from progress import progress_class
from cal_path import get_materials_path
from cal_path import get_light_dll_path
from cal_path import get_exe_command
from inp import inp_load_file
from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_get_mat_file
from epitaxy import epitaxy_get_electrical_layer
from epitaxy import epitaxy_get_width
from epitaxy import epitaxy_get_name
from inp import inp_search_token_value
from band_graph import band_graph

def find_modes(path):
	result = []
	file_names=[]
	pwd=os.getcwd()

	if os.path.isfile(os.path.join(pwd,"light_dump.zip")):
		zf = zipfile.ZipFile("light_dump.zip", 'r')

		for file in zf.filelist:
			file_names.append(file.filename)
		zf.close()
	else:
		for file in glob.glob(os.path.join(pwd,"light_dump","*.dat")):
			file_names.append(os.path.basename(file))

	for i in range(0,len(file_names)-1):
		if file_names[i].startswith("light_1d_"):
			if file_names[i].endswith("_photons_norm.dat"):
				store = file_names[i][:-17]
				s=store.split("light_1d_")
				store = s[1]
				result.append(store)


	return result

def find_models():
	ret=[]
	if running_on_linux()==True:
		ext="so"
	else:
		ext="dll"

	path=get_light_dll_path()

	
	for file in glob.glob(os.path.join(path,"*."+ext)):
		ret.append(os.path.splitext(os.path.basename(file))[0])

	return ret

def find_light_source():
	ret=[]

	path=get_materials_path()

	
	for file in glob.glob(os.path.join(path,"*.spectra")):
		ret.append(os.path.splitext(os.path.basename(file))[0])

	return ret

def find_materials():
	ret=[]

	path=get_materials_path()

	for file in glob.glob(os.path.join(path,"*")):
		if os.path.isdir(file)==True:
			ret.append(os.path.splitext(os.path.basename(file))[0])

	return ret

class class_optical(gtk.Window):

	icon_theme = gtk.icon_theme_get_default()
	

	edit_list=[]

	line_number=[]

	file_name=""
	name=""
	visible=1

	def init(self):
		self.progress_window=progress_class()
		self.progress_window.init()

		self.articles=[]
		self.dump_dir=os.path.join(os.getcwd(),"light_dump")
		find_models()
		self.main_vbox=gtk.VBox()
		self.gen_main_menu(self,self.main_vbox)
		toolbar = gtk.Toolbar()

		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 50)
		self.main_vbox.pack_start(toolbar, False, False, 0)
		
		self.edit_list=[]
		self.line_number=[]


		self.cb = gtk.combo_box_new_text()
		self.cb.set_wrap_width(5)
		self.cb_id=self.cb.connect("changed", self.on_changed)
		self.update_cb()


		self.cb_model = gtk.combo_box_new_text()
		self.cb_model.set_wrap_width(5)
		self.cb_model.connect("changed", self.on_cb_model_changed)
		self.update_cb_model()

		self.light_source_model = gtk.combo_box_new_text()
		self.light_source_model.set_wrap_width(5)
		self.light_source_model.connect("changed", self.on_light_source_model_changed)
		self.update_light_source_model()
		self.light_source_model.show()

		self.fig_photon_density = band_graph()
		self.fig_photon_density.set_data_file("light_1d_photons_tot_norm.dat")
		self.fig_photon_density.init()

		self.fig_photon_abs = band_graph()
		self.fig_photon_abs.set_data_file("light_1d_photons_tot_abs_norm.dat")
		self.fig_photon_abs.init()

		tool_bar_pos=0

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"play.png"))
		self.play = gtk.ToolButton(image)
   		#image.set_from_file(self.icon_theme.lookup_icon("media-playback-start", 32, 0).get_filename())
		refresh = gtk.ToolButton(image)
		refresh.connect("clicked", self.callback_refresh)
		toolbar.insert(refresh, tool_bar_pos)
		toolbar.show_all()
		tool_bar_pos=tool_bar_pos+1

		ti_light = gtk.ToolItem()
		lable=gtk.Label("Optical mode:")
		lable.show()
		ti_hbox = gtk.HBox(False, 2)
		ti_hbox.show()
        
		ti_hbox.pack_start(lable, False, False, 0)
		ti_hbox.pack_start(self.cb, False, False, 0)
		self.cb.show()

		lable=gtk.Label("Optical model:")
		lable.show()
	        ti_hbox.pack_start(lable, False, False, 0)
		ti_hbox.pack_start(self.cb_model, False, False, 0)
		self.cb_model.show()
		

		ti_light.add(ti_hbox);
		toolbar.insert(ti_light, tool_bar_pos)
		ti_light.show()

		tool_bar_pos=tool_bar_pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(False)
		sep.set_expand(False)
		toolbar.insert(sep, tool_bar_pos)
		sep.show()
		tool_bar_pos=tool_bar_pos+1

		lable=gtk.Label("Light source:")
		lable.show()
		ti_hbox.pack_start(lable, False, False, 0)
		ti_hbox.pack_start(self.light_source_model, False, False, 0)
		self.cb_model.show()

		sep = gtk.SeparatorToolItem()
		sep.set_draw(False)
		sep.set_expand(True)
		toolbar.insert(sep, tool_bar_pos)
		sep.show()
		tool_bar_pos=tool_bar_pos+1

		help = gtk.ToolButton(gtk.STOCK_HELP)
		toolbar.insert(help, tool_bar_pos)
		help.connect("clicked", self.callback_help)
		help.show()
		tool_bar_pos=tool_bar_pos+1


		self.notebook.append_page(self.fig_photon_density,gtk.Label("Photon density") )
		self.notebook.append_page(self.fig_photon_abs,gtk.Label("Photon absorbed") )

		self.main_vbox.pack_start(self.notebook, False, False, 0)

		optics_config=tab_class()
		optics_config.show()
		self.notebook.append_page(optics_config,gtk.Label("Optical setup"))
		optics_config.visible=True
		optics_config.init("optics.inp","Config")
		optics_config.label_name="Optics config"
		optics_config.file_name="optics.inp"

		#Photon distribution
		photon_dist=photon_dist_class()
		photon_dist.show()

##################
		input_files=[]
		input_files.append("./light_dump/light_2d_photons.dat")
		input_files.append("./light_dump/light_2d_photons_asb.dat")
		input_files.append("./light_dump/reflect.dat")

		plot_labels=[]
		plot_labels.append("Photon distribution")
		plot_labels.append("Photon distribution absorbed")
		plot_labels.append("Reflection")

		self.plot_widgets=[]
		self.progress_window.start()
		for i in range(0,len(input_files)):
			self.plot_widgets.append(plot_widget())
			self.plot_widgets[i].init(self)
			self.plot_widgets[i].set_labels([plot_labels[0]])
			self.plot_widgets[i].load_data([input_files[i]],os.path.splitext(input_files[i])[0]+".oplot")

			self.plot_widgets[i].do_plot()
			self.plot_widgets[i].show()

			self.notebook.append_page(self.plot_widgets[i],gtk.Label(plot_labels[i]))

		self.connect("delete-event", self.callback_close) 

		self.add(self.main_vbox)
		self.set_size_request(850,-1)
		self.main_vbox.show()

		self.fig_photon_density.draw_graph()
		self.fig_photon_abs.draw_graph()

		self.set_icon_from_file(os.path.join(get_image_file_path(),"image.jpg"))
		self.set_title("Optical Model - (www.opvdm.com)")
		self.set_position(gtk.WIN_POS_CENTER)
		self.progress_window.stop()

		
	def onclick(self, event):
		print 'button=%d, x=%d, y=%d, xdata=%f, ydata=%f'%(
		event.button, event.x, event.y, event.xdata, event.ydata)
		for i in range(0,len(self.layer_end)):
			if (self.layer_end[i]>event.xdata):
				break
		pwd=os.getcwd()
		plot_gen([os.path.join(pwd,"materials",self.layer_name[i],"alpha.omat")],[],None,"")

	def update_cb(self):
		self.cb.handler_block(self.cb_id)
		thefiles=find_modes(self.dump_dir)
		thefiles.sort()
		files_short=thefiles[::2]
		model=self.cb.get_model()
		model.clear()
		#self.cb.clear()
		self.cb.append_text("all")
		for i in range(0, len(files_short)):
			self.cb.append_text(str(files_short[i])+" nm")
		self.cb.set_active(0)
		self.cb.handler_unblock(self.cb_id)

	def update_cb_model(self):
		models=find_models()
		for i in range(0, len(models)):
			self.cb_model.append_text(models[i])

		used_model=inp_get_token_value("light.inp", "#light_model")
		if models.count(used_model)==0:
			used_model="exp"
			inp_update_token_value("light.inp", "#light_model","exp",1)

		self.cb_model.set_active(models.index(used_model))
		scan_item_add("light.inp","#light_model","Optical model",1)

	def update_light_source_model(self):
		models=find_light_source()
		for i in range(0, len(models)):
			self.light_source_model.append_text(models[i])

		used_model=inp_get_token_value("optics.inp", "#sun")
		if models.count(used_model)==0:
			used_model="sun"
			inp_update_token_value("optics.inp", "#sun","sun",1)

		self.light_source_model.set_active(models.index(used_model))
		scan_item_add("optics.inp","#sun","Light source",1)

	def callback_close(self, widget, data=None):
		self.hide()
		return True


	def callback_refresh(self, button):

		self.update_graph()
		self.update_cb()


	def gen_main_menu(self, window,vbox):
		self.notebook = gtk.Notebook()
		self.notebook.show()
		accel_group = gtk.AccelGroup()


		item_factory = gtk.ItemFactory(gtk.MenuBar, "<main>", accel_group)

		menu_items = (
		    ( "/_File",         None,         None, 0, "<Branch>" ),
		    ( "/File/Refresh",     None, self.callback_refresh, 0 , "<ImageItem>"),
		    ( "/File/Close",     "<control>Q", self.callback_close, 0, "<StockItem>", "gtk-quit" ),

		    )

		item_factory.create_items(menu_items)


		window.add_accel_group(accel_group)

		menubar=item_factory.get_widget("<main>")
		menubar.show_all()
		vbox.pack_start(menubar, False, True, 0)


	def update_graph(self):
		cmd = get_exe_command()+' --optics'
		print cmd
		ret= os.system(cmd)
		self.fig_photon_density.my_figure.clf()
		self.fig_photon_density.draw_graph()
		self.fig_photon_density.canvas.draw()

		self.fig_photon_abs.my_figure.clf()
		self.fig_photon_abs.draw_graph()
		self.fig_photon_abs.canvas.draw()

		for i in range(0,len(self.plot_widgets)):
			self.plot_widgets[i].update()


	def on_changed(self, widget):
		cb_text=widget.get_active_text()
		if cb_text=="all":
			self.fig_photon_density.set_data_file("light_1d_photons_tot_norm.dat")
			self.fig_photon_abs.set_data_file("light_1d_photons_tot_abs_norm.dat")
		else:
			self.fig_photon_density.set_data_file("light_1d_"+cb_text[:-3]+"_photons_norm.dat")
			self.fig_photon_abs.set_data_file("light_1d_"+cb_text[:-3]+"_photons_abs.dat")

		print "drawing"
		self.fig_photon_density.draw_graph()
		self.fig_photon_density.canvas.draw()

		print "drawing"
		self.fig_photon_abs.draw_graph()
		self.fig_photon_abs.canvas.draw()

	def on_cb_model_changed(self, widget):
		cb_text=widget.get_active_text()
		inp_update_token_value("light.inp", "#light_model", cb_text,1)

	def on_light_source_model_changed(self, widget):
		cb_text=widget.get_active_text()
		cb_text=cb_text+".spectra"
		inp_update_token_value("optics.inp", "#sun", cb_text,1)

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.opvdm.com/man/index.html')

