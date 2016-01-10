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
import random
import gobject
import os
from optics import find_materials
from inp import inp_write_lines_to_file
from inp import inp_load_file
from util import str2bool
from inp import inp_search_token_value
from inp import inp_update_token_value
from scan_item import scan_item_add
from cal_path import get_image_file_path
from emesh import tab_electrical_mesh
from plot_gen import plot_gen
from opvdm_open import opvdm_open
from cal_path import get_materials_path
from optics import class_optical
from global_objects import global_object_get
from epitaxy import epitaxy_get_width
from epitaxy import epitaxy_get_mat_file
from epitaxy import epitaxy_get_electrical_layer
from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_save
from epitaxy import epitaxy_load_from_arrays
from epitaxy import epitay_get_next_dos
from inp import inp_isfile
from inp import inp_copy_file
from help import my_help_class
from epitaxy import epitaxy_get_pl_file
from epitaxy import epitay_get_next_pl
from epitaxy import epitaxy_get_name

import i18n
_ = i18n.language.gettext

from i18n import yes_no

(
  COLUMN_NAME,
  COLUMN_THICKNES,
  COLUMN_MATERIAL,
  COLUMN_DEVICE,
  COLUMN_DOS_LAYER,
  COLUMN_PL_FILE
) = range(6)

class layer_widget(gtk.VBox):

	material_files=gtk.ListStore(str)
	active_layer=gtk.ListStore(str)
	def combo_changed(self, widget, path, text, model):
		#print model[path][1] 
		self.model[path][COLUMN_MATERIAL] = text
		self.save_model()
		self.refresh(True)

	def sync_to_electrical_mesh(self):
		tot=0
		for i in range(0,len(self.model)):
			if yes_no(self.model[i][COLUMN_DEVICE])==True:
				tot=tot+float(self.model[i][COLUMN_THICKNES])

		lines=[]
		if inp_load_file(lines,os.path.join(os.getcwd(),"mesh.inp"))==True:
			mesh_layers=int(inp_search_token_value(lines, "#mesh_layers"))
			if mesh_layers==1:
				inp_update_token_value(os.path.join(os.getcwd(),"mesh.inp"), "#mesh_layer_length0", str(tot),1)

	def active_layer_edit(self, widget, path, text, model):
		old_text=self.model[path][COLUMN_DEVICE]
		self.model[path][COLUMN_DEVICE]=text
		if yes_no(old_text)==False and yes_no(text)==True:
			self.model[path][COLUMN_DOS_LAYER]=epitay_get_next_dos()
			self.model[path][COLUMN_PL_FILE]=epitay_get_next_pl()
			new_file=self.model[path][COLUMN_DOS_LAYER]+".inp"
			if inp_isfile(new_file)==False:
				inp_copy_file(new_file,"dos0.inp")

			new_file=self.model[path][COLUMN_PL_FILE]+".inp"
			if inp_isfile(new_file)==False:
				inp_copy_file(new_file,"pl0.inp")

		if yes_no(text)==False:
			self.model[path][COLUMN_DOS_LAYER]="none"
			self.model[path][COLUMN_PL_FILE]="none"

		self.save_model()
		self.refresh(True)



	def rebuild_mat_list(self):
		self.material_files.clear()
		self.active_layer.clear()
		mat=find_materials()
		print mat
		for i in range(0,len(mat)):
			self.material_files.append([mat[i]])

		self.active_layer.append([_("yes")])
		self.active_layer.append([_("no")])

	def callback_view_materials(self, widget, data=None):
		dialog=opvdm_open()

		dialog.init(get_materials_path())
		dialog.show_inp_files=False
		response=dialog.run()


		if response == True:
			full_file_name=dialog.get_filename()
			plot_gen([dialog.get_filename()],[],"auto")

		elif response == gtk.RESPONSE_CANCEL:
		    print _("Closed, no files selected")
		dialog.destroy()

	def callback_move_down(self, widget, data=None):

		selection = self.treeview.get_selection()
		model, iter = selection.get_selected()

		if iter:
			path = model.get_path(iter)[0]
 			model.move_after( iter,model.iter_next(iter))
			self.save_model()
			self.refresh(True)

	def callback_edit_mesh(self, widget, data=None):
		if self.electrical_mesh.get_property("visible")==True:
			self.electrical_mesh.hide_all()
		else:
			self.electrical_mesh.show_all()

	def __init__(self,tooltips):

		self.optics_window=False

		self.electrical_mesh=tab_electrical_mesh()
		self.electrical_mesh.init()

		self.electrical_mesh.emesh_editor.connect("refresh", self.change_active_layer_thickness)

		self.frame=gtk.Frame()

		self.__gobject_init__()

		add_button = gtk.Button(_("Add layer"),gtk.STOCK_ADD)
		add_button.show()

		delete_button = gtk.Button(_("Delete layer"),gtk.STOCK_DELETE)
		delete_button.show()

		# create tree view
		self.model = self.__create_model()

		self.treeview = gtk.TreeView(self.model)
		self.treeview.set_size_request(400, 150)
		self.treeview.set_rules_hint(True)
		self.treeview.get_selection().set_mode(gtk.SELECTION_SINGLE)

		toolbar = gtk.Toolbar()
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 50)
		pos=0

		add = gtk.ToolButton(gtk.STOCK_ADD)
		add.connect("clicked", self.on_add_item_clicked)
		tooltips.set_tip(add, _("Add device layer"))
		toolbar.insert(add, pos)
		pos=pos+1


		remove = gtk.ToolButton(gtk.STOCK_CLEAR)
		remove.connect("clicked", self.on_remove_item_clicked)
		tooltips.set_tip(remove, _("Delete device layer"))
		toolbar.insert(remove, pos)
		pos=pos+1

		move = gtk.ToolButton(gtk.STOCK_GO_DOWN)
		move.connect("clicked", self.callback_move_down)
		tooltips.set_tip(move, _("Move device layer"))
		toolbar.insert(move, pos)
		pos=pos+1


		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"mesh.png"))
		self.mesh = gtk.ToolButton(image)
		tooltips.set_tip(self.mesh, _("Edit the electrical mesh"))
		self.mesh.connect("clicked", self.callback_edit_mesh)
		toolbar.insert(self.mesh, pos)
		pos=pos+1

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"optics.png"))
		self.optics_button = gtk.ToolButton(image)
		tooltips.set_tip(self.optics_button, _("Optical simulation"))
		self.optics_button.connect("clicked", self.callback_optics_sim)
		toolbar.insert(self.optics_button, pos)
		self.optics_button.show_all()
		pos=pos+1

		#image = gtk.Image()
   		#image.set_from_file(os.path.join(get_image_file_path(),"dir_file.png"))
		#self.mesh = gtk.ToolButton(image)
		self.mesh = gtk.ToolButton(gtk.STOCK_OPEN)
		tooltips.set_tip(self.mesh, _("Look at the materials database"))
		self.mesh.connect("clicked", self.callback_view_materials)
		toolbar.insert(self.mesh, pos)
		pos=pos+1

		hbox0=gtk.HBox()

		self.frame.set_label(_("Device layers"))
		self.frame.set_label_align(0.0, 0.0)
		self.frame.set_shadow_type(gtk.SHADOW_ETCHED_OUT)
		hbox0.show()

		self.pack_start(toolbar, False, False, 0)
		self.frame.add(self.treeview)
		self.pack_start(self.frame, True, True, 0)

		self.__add_columns(self.treeview)


		self.show_all()


        

	def __create_model(self):

		# create list store
		model = gtk.ListStore(str,str,str,str,str,str,str)

		# add items

		self.rebuild_mat_list()

		for i in range(0,epitaxy_get_layers()):
			thick=epitaxy_get_width(i)
			material=epitaxy_get_mat_file(i)
			dos_layer=epitaxy_get_electrical_layer(i)
			pl_file=epitaxy_get_pl_file(i)
			name=epitaxy_get_name(i)

			dos_file=""

			if dos_layer=="none":
				dos_file=_("no")
			else:
				dos_file=_("yes")

			scan_item_add("epitaxy.inp","#layer"+str(i),_("Material for ")+str(material),2)
			scan_item_add("epitaxy.inp","#layer"+str(i),_("Layer width ")+str(material),1)

			iter = model.append()

			model.set (iter,
			  COLUMN_NAME, str(name),
			  COLUMN_THICKNES, str(thick),
			  COLUMN_MATERIAL, str(material),
			  COLUMN_DEVICE, str(dos_file),
			  COLUMN_DOS_LAYER, str(dos_layer),
			  COLUMN_PL_FILE, str(pl_file)
			)
		return model

	def on_remove_item_clicked(self, button):

		selection = self.treeview.get_selection()
		model, iter = selection.get_selected()

		if iter:
			path = model.get_path(iter)[0]
			model.remove(iter)

			self.save_model()
			self.refresh(True)



	def __add_columns(self, treeview):

		model = treeview.get_model()

		# Name
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited, model)
		renderer.set_data("column", COLUMN_NAME)
		renderer.set_property("editable", True)
		column = gtk.TreeViewColumn(_("Layer name"), renderer, text=COLUMN_NAME,editable=True)
		treeview.append_column(column)

		# Thicknes
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited, model)
		renderer.set_data("column", COLUMN_THICKNES)
		renderer.set_property("editable", True)
		column = gtk.TreeViewColumn(_("Thicknes"), renderer, text=COLUMN_THICKNES,editable=True)
		treeview.append_column(column)

		# Material file
		column = gtk.TreeViewColumn(_("Optical material"))
		cellrenderer_combo = gtk.CellRendererCombo()
		cellrenderer_combo.set_property("editable", True)
		cellrenderer_combo.set_property("model", self.material_files)
		cellrenderer_combo.set_property("text-column", 0)
		cellrenderer_combo.connect("edited", self.combo_changed, self.material_files)
		column.pack_start(cellrenderer_combo, False)
		column.add_attribute(cellrenderer_combo, "text", COLUMN_MATERIAL)
		treeview.append_column(column)

		# Device
		#renderer = gtk.CellRendererText()
		#renderer.connect("edited", self.on_cell_edited, model)
		#renderer.set_data("column", COLUMN_DEVICE)
		#renderer.set_property("editable", True)
		#column = gtk.TreeViewColumn("Active layer", renderer, text=COLUMN_DEVICE,editable=True)

		column = gtk.TreeViewColumn(_("Active layer"))
		render = gtk.CellRendererCombo()
		render.set_property("editable", True)
		render.set_property("model", self.active_layer)
		render.set_property("text-column", 0)
		render.connect("edited", self.active_layer_edit, self.active_layer)
		column.pack_start(render, False)
		column.add_attribute(render, "text", COLUMN_DEVICE)
		treeview.append_column(column)

		#column = gtk.TreeViewColumn("DoS Layer")
		#render = gtk.CellRendererCombo()
		#render.set_property("editable", True)
		#render.set_property("text-column", 0)
		#renderer.connect("edited", self.on_dos_layer_edited, model)
		#column.pack_start(render, False)
		#column.add_attribute(render, "text", COLUMN_DOS_LAYER)
		#treeview.append_column(column)


		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_dos_layer_edited, model)
		renderer.set_data("column", COLUMN_DOS_LAYER)
		renderer.set_property("editable", True)
		column = gtk.TreeViewColumn(_("DoS Layer"), renderer, text=COLUMN_DOS_LAYER,editable=True)
		column.set_visible(False)
		treeview.append_column(column)

		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_dos_layer_edited, model)
		renderer.set_data("column", COLUMN_PL_FILE)
		renderer.set_property("editable", True)
		column = gtk.TreeViewColumn(_("DoS Layer"), renderer, text=COLUMN_PL_FILE,editable=True)
		column.set_visible(False)
		treeview.append_column(column)

	def on_cell_edited(self, cell, path_string, new_text, model):
		iter = model.get_iter_from_string(path_string)
		path = model.get_path(iter)[0]
		column = cell.get_data("column")

		model.set(iter, column, new_text)

		self.save_model()
		self.refresh(True)

	def on_dos_layer_edited(self, cell, path_string, new_text, model):

		iter = model.get_iter_from_string(path_string)
		path = model.get_path(iter)[0]
		column = cell.get_data("column")

		model.set(iter, column, new_text)

		self.save_model()
		self.refresh(True)

	def change_active_layer_thickness(self,obj):
		thickness=obj.get_data("refresh")
		print thickness
		count=0
		for item in self.model:
			if str2bool(item[COLUMN_DEVICE])==True:
				count=count+1

		if count==1:
			for item in self.model:
				if str2bool(item[COLUMN_DEVICE])==True:
					item[COLUMN_THICKNES]=str(thickness)
					self.save_model()
					self.refresh(False)
					return

	def on_add_item_clicked(self, button):
		new_item = [_("layer name"),"100e-9", "pcbm",_("no"),"none",False]

		selection = self.treeview.get_selection()
		model, iter = selection.get_selected()

		path = model.get_path(iter)[0]

		iter = model.insert(path)
		model.set (iter,
		    COLUMN_NAME, new_item[COLUMN_NAME],
		    COLUMN_THICKNES, new_item[COLUMN_THICKNES],
		    COLUMN_MATERIAL, new_item[COLUMN_MATERIAL],
		    COLUMN_DEVICE, new_item[COLUMN_DEVICE],
		    COLUMN_DOS_LAYER, new_item[COLUMN_DOS_LAYER],
		    COLUMN_PL_FILE, new_item[COLUMN_PL_FILE],

		)
		self.save_model()
		self.refresh(True)


	def refresh(self,emit):
		self.electrical_mesh.refresh()
		if emit==True:
			self.emit("refresh")

		global_object_get("dos-update")()
		global_object_get("pl-update")()


	def save_model(self):

		thick=[]
		mat_file=[]
		dos_file=[]
		pl_file=[]
		name=[]

		for item in self.model:
			name.append(item[COLUMN_NAME])
			thick.append(item[COLUMN_THICKNES])
			mat_file.append(item[COLUMN_MATERIAL])
			dos_file.append(item[COLUMN_DOS_LAYER])
			pl_file.append(item[COLUMN_PL_FILE])
		epitaxy_load_from_arrays(name,thick,mat_file,dos_file,pl_file)

		epitaxy_save()
		self.sync_to_electrical_mesh()

	def callback_optics_sim(self, widget, data=None):
		my_help_class.help_set_help(["optics.png",_("<big><b>The optical simulation window</b></big>\nUse this window to perform optical simulations.  Click on the play button to run a simulation."),"play.png",_("Click on the play button to run an optical simulation.  The results will be displayed in the tabs to the right.")])

		if self.optics_window==False:
			self.optics_window=class_optical()
			self.optics_window.init()

		if self.optics_window.get_property("visible")==True:
			self.optics_window.hide()
		else:
			self.optics_window.show()


gobject.type_register(layer_widget)
gobject.signal_new("refresh", layer_widget, gobject.SIGNAL_RUN_FIRST,gobject.TYPE_NONE, ())

