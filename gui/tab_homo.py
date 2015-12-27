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
from inp import inp_isfile
from inp import inp_load_file
from inp import inp_write_lines_to_file
from numpy import *
from matplotlib.figure import Figure
from numpy import arange, sin, pi
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
import gobject
from scan_item import scan_item_add
from tab_base import tab_base

(
  LUMO_FUNCTION,
  LUMO_ENABLE,
  LUMO_A,
  LUMO_B,
  LUMO_C,
  LUMO_EDITABLE
) = range(6)

(
  HOMO_FUNCTION,
  HOMO_ENABLE,
  HOMO_A,
  HOMO_B,
  HOMO_C,
  HOMO_EDITABLE
) = range(6)


articles = []
HOMO_articles = []

class tab_bands(gtk.HBox,tab_base):
	lines=[]
	edit_list=[]

	line_number=[]
	save_file_name=""

	def update(self):
		self.enabled=inp_isfile("./lumo0.inp")

	def __create_model(self):

		# create list store
		model = gtk.ListStore(
		    gobject.TYPE_STRING,
		    gobject.TYPE_STRING,
		    gobject.TYPE_STRING,
		    gobject.TYPE_STRING,
		    gobject.TYPE_STRING,
		    gobject.TYPE_BOOLEAN
		)

		# add items
		for item in articles:
			iter = model.append()

			model.set (iter,
			  LUMO_FUNCTION, item[LUMO_FUNCTION],
			  LUMO_ENABLE, item[LUMO_ENABLE],
			  LUMO_A, item[LUMO_A],
			  LUMO_B, item[LUMO_B],
			  LUMO_C, item[LUMO_C],
			  LUMO_EDITABLE, item[LUMO_EDITABLE]
			)

		return model

	def __create_model_mesh(self):

		model_mesh = gtk.ListStore(
		    gobject.TYPE_STRING,
		    gobject.TYPE_STRING,
		    gobject.TYPE_STRING,
		    gobject.TYPE_STRING,
		    gobject.TYPE_STRING,
		    gobject.TYPE_BOOLEAN
		)


		for item in HOMO_articles:
			iter = model_mesh.append()

			model_mesh.set (iter,
			  HOMO_FUNCTION, item[HOMO_FUNCTION],
			  HOMO_ENABLE, item[HOMO_ENABLE],
			  HOMO_A, item[HOMO_A],
			  HOMO_B, item[HOMO_B],
			  HOMO_C, item[HOMO_C],
			  HOMO_EDITABLE, item[HOMO_EDITABLE]
			)
		return model_mesh

	def __add_columns(self, treeview):

		model = treeview.get_model()

		# Function
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited, model)
		renderer.set_data("column", LUMO_FUNCTION)

		column = gtk.TreeViewColumn("Function", renderer, text=LUMO_FUNCTION,
				       editable=LUMO_EDITABLE)
		treeview.append_column(column)

		# Enable
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited, model)
		renderer.set_data("column", LUMO_ENABLE)

		column = gtk.TreeViewColumn("enabled", renderer, text=LUMO_ENABLE,
				       editable=LUMO_EDITABLE)
		treeview.append_column(column)

		# A
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited, model)
		renderer.set_data("column", LUMO_A)

		column = gtk.TreeViewColumn("a", renderer, text=LUMO_A,
				       editable=LUMO_EDITABLE)
		treeview.append_column(column)

		# B
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited, model)
		renderer.set_data("column", LUMO_B)

		column = gtk.TreeViewColumn("b", renderer, text=LUMO_B,
				       editable=LUMO_EDITABLE)
		treeview.append_column(column)

		# C
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited, model)
		renderer.set_data("column", LUMO_C)

		column = gtk.TreeViewColumn("c", renderer, text=LUMO_C,
				       editable=LUMO_EDITABLE)
		treeview.append_column(column)

	def __add_columns_mesh(self, treeview):

		model = treeview.get_model()

		# Function
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_HOMO_edited, model)
		renderer.set_data("column", HOMO_FUNCTION)

		column = gtk.TreeViewColumn("Function", renderer, text=HOMO_FUNCTION,
				       editable=HOMO_EDITABLE)
		treeview.append_column(column)

		# Enable
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_HOMO_edited, model)
		renderer.set_data("column", HOMO_ENABLE)

		column = gtk.TreeViewColumn("Enable", renderer, text=HOMO_ENABLE,
				       editable=HOMO_EDITABLE)
		treeview.append_column(column)

		# A
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_HOMO_edited, model)
		renderer.set_data("column", HOMO_A)

		column = gtk.TreeViewColumn("a", renderer, text=HOMO_A,
				       editable=HOMO_EDITABLE)
		treeview.append_column(column)

		# B
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_HOMO_edited, model)
		renderer.set_data("column", HOMO_B)

		column = gtk.TreeViewColumn("b", renderer, text=HOMO_B,
				       editable=HOMO_EDITABLE)
		treeview.append_column(column)

		# C
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_HOMO_edited, model)
		renderer.set_data("column", HOMO_C)

		column = gtk.TreeViewColumn("c", renderer, text=HOMO_C,
				       editable=HOMO_EDITABLE)
		treeview.append_column(column)

	def on_add_item_clicked(self, button, model):
		new_item = ["exp","0" ,"A","B","C",True]
		articles.append(new_item)

		iter = model.append()
		model.set (iter,
		    LUMO_FUNCTION, new_item[LUMO_FUNCTION],
		    LUMO_ENABLE, new_item[LUMO_ENABLE],
		    LUMO_A, new_item[LUMO_A],
		    LUMO_B, new_item[LUMO_B],
		    LUMO_C, new_item[LUMO_C],
		    LUMO_EDITABLE, new_item[LUMO_EDITABLE]
		)

	def on_add_HOMO_clicked(self, button, model):
		new_item = ["exp","0" ,"A","B","C",True]
		HOMO_articles.append(new_item)

		iter = model.append()
		model.set (iter,
		    HOMO_FUNCTION, new_item[HOMO_FUNCTION],
		    HOMO_ENABLE, new_item[HOMO_ENABLE],
		    HOMO_A, new_item[HOMO_A],
		    HOMO_B, new_item[HOMO_B],
		    HOMO_C, new_item[HOMO_C],
		    HOMO_EDITABLE, new_item[HOMO_EDITABLE]
		)
	def save_model(self, ):
		lines=[]
		function=0
		for item in self.LUMO_model:
			lines.append("#function_"+item[HOMO_FUNCTION]+str(function))
			lines.append(item[LUMO_FUNCTION])
			lines.append("#function_"+item[HOMO_FUNCTION]+str(function)+"_enable")
			lines.append(item[LUMO_ENABLE])
			lines.append("#function_"+item[HOMO_FUNCTION]+str(function)+"_a")
			lines.append(item[LUMO_A])
			lines.append("#function_"+item[HOMO_FUNCTION]+str(function)+"_b")
			lines.append(item[LUMO_B])
			lines.append("#function_"+item[HOMO_FUNCTION]+str(function)+"_c")
			lines.append(item[LUMO_C])
			function=function+1
		lines.append("#ver")
		lines.append("#1.0")
		lines.append("#end")
		inp_write_lines_to_file("./lumo0.inp",lines)

		lines=[]
		function=0
		for item in self.HOMO_model:
			lines.append("#function_"+item[HOMO_FUNCTION]+str(function))
			lines.append(item[HOMO_FUNCTION])
			lines.append("#function_"+item[HOMO_FUNCTION]+str(function)+"_enable")
			lines.append(item[HOMO_ENABLE])
			lines.append("#function_"+item[HOMO_FUNCTION]+str(function)+"_a")
			lines.append(item[HOMO_A])
			lines.append("#function_"+item[HOMO_FUNCTION]+str(function)+"_b")
			lines.append(item[HOMO_B])
			lines.append("#function_"+item[HOMO_FUNCTION]+str(function)+"_c")
			lines.append(item[HOMO_C])
			function=function+1
		lines.append("#ver")
		lines.append("#1.0")		
		lines.append("#end")			
		inp_write_lines_to_file("./homo0.inp",lines)

	def on_remove_item_from_lumo_clicked(self, button, treeview):

		selection = treeview.get_selection()
		model, iter = selection.get_selected()

		if iter:
			path = model.get_path(iter)[0]
			model.remove(iter)

			del articles[ path ]

		self.save_model()
		self.update_graph()

	def on_remove_item_from_homo_clicked(self, button, treeview):

		selection = treeview.get_selection()
		model, iter = selection.get_selected()

		if iter:
			path = model.get_path(iter)[0]
			model.remove(iter)

			del HOMO_articles[ path ]

		self.save_model()
		self.update_graph()

	def on_cell_edited(self, cell, path_string, new_text, model):

		iter = model.get_iter_from_string(path_string)
		path = model.get_path(iter)[0]
		column = cell.get_data("column")

		if column == LUMO_FUNCTION:
			articles[path][LUMO_FUNCTION] = new_text

			model.set(iter, column, articles[path][LUMO_FUNCTION])

		if column == LUMO_ENABLE:
			#old_text = model.get_value(iter, column)
			articles[path][LUMO_ENABLE] = new_text
			model.set(iter, column, articles[path][LUMO_ENABLE])

		if column == LUMO_A:
			#old_text = model.get_value(iter, column)
			articles[path][LUMO_A] = new_text
			model.set(iter, column, articles[path][LUMO_A])

		if column == LUMO_B:
			#old_text = model.get_value(iter, column)
			articles[path][LUMO_B] = new_text
			model.set(iter, column, articles[path][LUMO_B])

		if column == LUMO_C:
			#old_text = model.get_value(iter, column)
			articles[path][LUMO_C] = new_text
			model.set(iter, column, articles[path][LUMO_C])

		self.save_model()
		self.update_graph()

	def on_HOMO_edited(self, cell, path_string, new_text, model):

		iter = model.get_iter_from_string(path_string)
		path = model.get_path(iter)[0]
		column = cell.get_data("column")

		if column == HOMO_FUNCTION:
			HOMO_articles[path][HOMO_FUNCTION] = new_text

			model.set(iter, column, HOMO_articles[path][HOMO_FUNCTION])

		if column == HOMO_ENABLE:
			#old_text = model.get_value(iter, column)
			HOMO_articles[path][HOMO_ENABLE] = new_text
			model.set(iter, column, HOMO_articles[path][HOMO_ENABLE])

		if column == HOMO_A:
			#old_text = model.get_value(iter, column)
			HOMO_articles[path][HOMO_A] = new_text
			model.set(iter, column, HOMO_articles[path][HOMO_A])

		if column == HOMO_B:
			#old_text = model.get_value(iter, column)
			HOMO_articles[path][HOMO_B] = new_text
			model.set(iter, column, HOMO_articles[path][HOMO_B])

		if column == HOMO_C:
			#old_text = model.get_value(iter, column)
			HOMO_articles[path][HOMO_C] = new_text
			model.set(iter, column, HOMO_articles[path][HOMO_C])

		self.save_model()
		self.update_graph()

	def update_graph(self):
		#cmd = './go.o --onlypos'
		#ret= os.system(cmd)
		self.LUMO_fig.clf()
		self.draw_graph_lumo()
		self.LUMO_fig.canvas.draw()

	def draw_graph_lumo(self):

		n=0

		ax1 = self.LUMO_fig.add_subplot(111)

		ax1.set_ylabel('$DoS (m^{-3} eV^{-1})$')
		ax1.set_xlabel('Energy (eV)')

		#ax2 = ax1.twinx()
		x_pos=0.0
		layer=0
		color =['r','g','b','y','o','r','g','b','y','o']
		ax1.set_yscale('log')
		ax1.set_ylim(ymin=1e17,ymax=1e28)
		pos=0
		Eg=2.0
		ax1.set_xlim([0,-Eg])
		x = linspace(0, -Eg, num=40)
		for item in self.LUMO_model:
			if item[LUMO_FUNCTION]=="exp":
				y = float(item[LUMO_A])*exp(x/float(item[LUMO_B]))

				line, = ax1.plot(x,y , '-', linewidth=3)
			if item[LUMO_FUNCTION]=="gaus":
	
				y = float(item[LUMO_A])*exp(-pow(((float(item[LUMO_B])+x)/(sqrt(2.0)*float(item[LUMO_C])*1.0)),2.0))

				line, = ax1.plot(x,y , color[pos], linewidth=3)
				pos=pos+1

		pos=0

		x_homo = linspace(-Eg, 0, num=40)
		for item in self.HOMO_model:
			if item[HOMO_FUNCTION]=="exp":

				y = float(item[HOMO_A])*exp(x/float(item[HOMO_B]))

				line, = ax1.plot(x_homo,y , '-', linewidth=3)
			if item[LUMO_FUNCTION]=="gaus":
				y = float(item[HOMO_A])*exp(-pow(((float(item[HOMO_B])+x)/(sqrt(2.0)*float(item[HOMO_C])*1.0)),2.0))

				line, = ax1.plot(x_homo,y , color[pos], linewidth=3)
				pos=pos+1

	def save_image(self,file_name):
		data=os.path.splitext(file_name)[0]
		lumo=data+'_bands.jpg'
		self.canvas_lumo.figure.savefig(lumo)



	def callback_save(self, widget, data=None):
		dialog = gtk.FileChooserDialog("Save as..",
                               None,
                               gtk.FILE_CHOOSER_ACTION_SAVE,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_SAVE, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)

		filter = gtk.FileFilter()
		filter.set_name(".jpg")
		filter.add_pattern("*.jpg")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			file_name=dialog.get_filename()

			if os.path.splitext(file_name)[1]:
				self.save_image(file_name)
			else:
				filter=dialog.get_filter()
				self.save_image(file_name+filter.get_name())
			
		elif response == gtk.RESPONSE_CANCEL:
		    print 'Closed, no files selected'
		dialog.destroy()



	def wow(self):
		self.edit_list=[]
		self.line_number=[]
		self.lines=[]
		self.save_file_name="lumo0.inp"

		inp_load_file(self.lines,"./lumo0.inp")

		n=0
		pos=0

		while True:
			if self.lines[pos]=="#end":
				break
			if self.lines[pos]=="#ver":
				break

			tag=self.lines[pos]
			scan_item_add("lumo0.inp",tag,tag,1)
			pos=pos+1	#skip hash tag

			function=self.lines[pos]	#read label
			pos=pos+1

			tag=self.lines[pos]
			scan_item_add("lumo0.inp",tag,tag,1)
			pos=pos+1	#skip hash tag

			enabled=self.lines[pos] 	#read value
			pos=pos+1
			
			tag=self.lines[pos]
			scan_item_add("lumo0.inp",tag,tag,1)
			pos=pos+1	#skip hash tag

			a=self.lines[pos] 	#read value
			pos=pos+1

			tag=self.lines[pos]
			scan_item_add("lumo0.inp",tag,tag,1)
			pos=pos+1	#skip hash tag

			b=self.lines[pos] 	#read value
			pos=pos+1

			tag=self.lines[pos]
			scan_item_add("lumo0.inp",tag,tag,1)
			pos=pos+1	#skip hash tag
			c=self.lines[pos] 	#read value
			pos=pos+1

			articles.append([ str(function), str(enabled), str(a), str(b), str(c), True ])


		self.save_file_name="homo0.inp"

		inp_load_file(self.lines,"./homo0.inp")
		n=0
		pos=0

		while True:
			if self.lines[pos]=="#end":
				break
			if self.lines[pos]=="#ver":
				break

			tag=self.lines[pos]
			scan_item_add("homo0.inp",tag,tag,1)
			pos=pos+1	#skip hash tag


			function=self.lines[pos]	#read label
			pos=pos+1

			tag=self.lines[pos]
			scan_item_add("homo0.inp",tag,tag,1)
			pos=pos+1	#skip hash tag

			enabled=self.lines[pos] 	#read value
			pos=pos+1

			tag=self.lines[pos]
			scan_item_add("homo0.inp",tag,tag,1)			
			pos=pos+1	#skip hash tag

			a=self.lines[pos] 	#read value
			pos=pos+1

			tag=self.lines[pos]
			scan_item_add("homo0.inp",tag,tag,1)
			pos=pos+1	#skip hash tag

			b=self.lines[pos] 	#read  value
			pos=pos+1

			tag=self.lines[pos]
			scan_item_add("homo0.inp",tag,tag,1)
			pos=pos+1	#skip hash tag
			c=self.lines[pos] 	#read  value
			pos=pos+1

			HOMO_articles.append([ str(function), str(enabled), str(a), str(b), str(c), True ])


		tooltips = gtk.Tooltips()

		toolbar = gtk.Toolbar()
		toolbar.set_orientation(gtk.ORIENTATION_VERTICAL)
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(50, -1)

		save = gtk.ToolButton(gtk.STOCK_SAVE)
		tooltips.set_tip(save, "Save image")
		pos=0
		toolbar.insert(save, pos)
		save.connect("clicked", self.callback_save)
		toolbar.show_all()
		self.pack_start(toolbar, False, True, 0)


 		vbox = gtk.VBox(False, 2)
		self.LUMO_model = self.__create_model()
		self.HOMO_model = self.__create_model_mesh()
		self.LUMO_fig = Figure(figsize=(5,4), dpi=100)


		self.draw_graph_lumo()
		self.canvas_lumo = FigureCanvas(self.LUMO_fig)  # a gtk.DrawingArea
		self.canvas_lumo.figure.patch.set_facecolor('white')
		self.canvas_lumo.show()
		vbox.pack_start(self.canvas_lumo, True, True, 1)
		
		self.pack_start(vbox, True, True, 0)
		#self.attach(vbox, 0, 3, 0, 2)
		vbox.show()
		#Layer editor

		self.LUMO_fig.tight_layout(pad=0.5)

	        vbox = gtk.VBox(False, 2)
		

		frame = gtk.Frame()
		frame.set_label("LUMO")
		vbox_layers = gtk.VBox(False, 2)
		treeview = gtk.TreeView(self.LUMO_model)
		treeview.set_size_request(400, 100)
		treeview.set_rules_hint(True)
		treeview.get_selection().set_mode(gtk.SELECTION_SINGLE)
		self.__add_columns(treeview)
	        vbox_layers.pack_start(treeview, False, False, 0)
		treeview.show()

		add_button = gtk.Button("Add",gtk.STOCK_ADD)
		add_button.connect("clicked", self.on_add_item_clicked, self.LUMO_model)
		add_button.show()

		delete_button = gtk.Button("Delete",gtk.STOCK_DELETE)
		delete_button.connect("clicked", self.on_remove_item_from_lumo_clicked, treeview)
		delete_button.show()

	        hbox = gtk.HBox(False, 2)
        
	        hbox.pack_start(add_button, False, False, 0)
	        hbox.pack_start(delete_button, False, False, 0)
		hbox.show()

		vbox_layers.pack_start(hbox, False, False, 0)
		vbox_layers.show()

		frame.add(vbox_layers)
		frame.show()
	        vbox.pack_start(frame, False, False, 0)

		#spacer
		label=gtk.Label(" \n\n    ")
		#self.attach(label, 4, 5, 1, 2,gtk.SHRINK ,gtk.SHRINK)
		vbox.pack_start(label, False, False, 0)

		label.show()



		#mesh editor
		frame = gtk.Frame()
		frame.set_label("HOMO")
		vbox_mesh = gtk.VBox(False, 2)
		treeview = gtk.TreeView(self.HOMO_model)
		treeview.set_size_request(400, 100)
		treeview.set_rules_hint(True)
		treeview.get_selection().set_mode(gtk.SELECTION_SINGLE)
		self.__add_columns_mesh(treeview)
		vbox_mesh.pack_start(treeview, False, False, 0)
		treeview.show()

		add_button = gtk.Button("Add",gtk.STOCK_ADD)
		add_button.connect("clicked", self.on_add_HOMO_clicked, self.HOMO_model)
		add_button.show()

		delete_button = gtk.Button("Delete",gtk.STOCK_DELETE)
		delete_button.connect("clicked", self.on_remove_item_from_homo_clicked, treeview)
		delete_button.show()

	        hbox = gtk.HBox(False, 2)
        
	        hbox.pack_start(add_button, False, False, 0)
	        hbox.pack_start(delete_button, False, False, 0)
		vbox_mesh.pack_start(hbox, False, False, 0)
		frame.add(vbox_mesh)
		vbox.pack_start(frame, False, False, 0)
		frame.show()
		vbox_mesh.show()
		hbox.show()
		self.pack_start(vbox, False, False, 0)
		#self.attach(vbox, 3, 4, 0, 1,gtk.SHRINK ,gtk.SHRINK)
		vbox.show()


