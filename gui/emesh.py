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
from cal_path import get_exe_command
from numpy import *
from matplotlib.figure import Figure
from numpy import arange, sin, pi
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg as NavigationToolbar
import gobject
from cal_path import get_image_file_path
from scan_item import scan_item_add
import webbrowser
from electrical_mesh_editor import electrical_mesh_editor
from inp import inp_load_file
from inp import inp_sum_items
from inp import inp_load_file
from inp import inp_search_token_value
from matplotlib.patches import Ellipse, PathPatch
from epitaxy import epitaxy_get_dos_files

class tab_electrical_mesh(gtk.Window):
	lines=[]
	edit_list=[]

	line_number=[]

	file_name=""
	name=""
	visible=1

	def run_simulation(self,data):
		cmd = get_exe_command()+' --onlypos'
		os.system(cmd)
		self.update_graph()

	def callback_update(self,data):
		self.update_graph()

	def update_graph(self):
		self.fig.clf()
		self.draw_graph()
		self.fig.canvas.draw()

	def draw_graph(self):
		lines=[]
		total=10
		pos=0
		bands=10

		#circle1=plt.Circle((10,10),4,color='r')

		if inp_load_file(lines,"mesh.inp")==True:
			total=inp_sum_items(lines, "#mesh_layer_points0")

		if inp_load_file(lines,"dump.inp")==True:
			pos=int(inp_search_token_value(lines, "#dump_energy_slice_pos"))

		files=epitaxy_get_dos_files()
		if len(files)>0:
			if inp_load_file(lines,files[0])==True:
				bands=int(inp_search_token_value(lines, "#srh_bands"))

		n=0
		
		self.fig.clf()
		
		self.fig.subplots_adjust(bottom=0.2)
		self.fig.subplots_adjust(left=0.1)
		self.ax1 = self.fig.add_subplot(111)
		#ax2 = ax1.twinx()
		x_pos=0.0
		layer=0
		color =['r','g','b','y','o','r','g','b','y','o']

		self.ax1.set_ylabel('Energy (eV)')
		#ax2.set_ylabel('Energy (eV)')
		self.ax1.set_xlabel('Position (nm)')
		try:

			t,Ec_data = loadtxt(os.path.join("equilibrium","Ec.dat"), unpack=True)
			t=t*1e9
			Ec, = self.ax1.plot(t,Ec_data, 'ro-', linewidth=3 ,alpha=0.5)

			t,Ev_data = loadtxt(os.path.join("equilibrium","Ev.dat"), unpack=True)
			t=t*1e9
			Ev,=self.ax1.plot(t,Ev_data, 'go-', linewidth=3 ,alpha=0.5)

			if len(files)>0:
				if self.emesh_editor.mesh_dump_ctl.enable==True:

					Ec_max=max(Ec_data)
					Ev_min=min(Ev_data)

					x_len=t[len(t)-1]-t[0]
					y_len=Ec_max-Ev_min

					x_pos=t[0]+(x_len)*(float(pos)/float(total))
					x_size=(x_len)/float(total)/4
					start=float(Ev_data[pos])
					stop=float(Ec_data[pos])
					y_pos=start
					dy=(stop-start)/(bands*2)
					while y_pos<stop:
						self.ax1.add_patch(Ellipse((x_pos, y_pos), x_size,x_size*(y_len/x_len), facecolor='red', edgecolor=(0,0.0,0.0), linewidth=0, alpha=1.0))
						y_pos=y_pos+dy


			t,s = loadtxt(os.path.join("equilibrium","Fi.dat"), unpack=True)
			t=t*1e9
			Fi,=self.ax1.plot(t,s, 'bo-', linewidth=3 ,alpha=0.5)

			if self.show_key==True:
				self.fig.legend((Ec, Ev, Fi), ('LUMO', 'HOMO', 'Fi'), 'upper right')
			else:
				self.ax1.legend_ = None
			#self.fig.canvas.draw()
		
		except:
			print "No mode file\n"

		
	def save_image(self,file_name):
		self.fig.savefig(file_name)	

	def refresh(self):
		self.emesh_editor.refresh()
		self.update_graph()
		

	def callback_close(self, widget, data=None):
		self.hide()
		return True

	def callback_hide_key(self, widget, data=None):
		self.show_key=not self.show_key
		self.draw_graph()

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

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.opvdm.com/man/index.html')

	def init(self):
		print "INIT!!"

		self.emesh_editor=electrical_mesh_editor()
		self.emesh_editor.init()

		self.fig = Figure(figsize=(5,4), dpi=100)
		self.ax1=None
		self.show_key=True
		self.hbox=gtk.HBox()
		self.edit_list=[]
		self.line_number=[]
		gui_pos=0

		gui_pos=gui_pos+1

		self.draw_graph()
		canvas = FigureCanvas(self.fig)  # a gtk.DrawingArea
		#canvas.set_background('white')
		#canvas.set_facecolor('white')
		canvas.figure.patch.set_facecolor('white')
		canvas.set_size_request(500, 150)
		canvas.show()

		tooltips = gtk.Tooltips()

		toolbar = gtk.Toolbar()
		#toolbar.set_orientation(gtk.ORIENTATION_VERTICAL)
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 50)

		tool_bar_pos=0
		save = gtk.ToolButton(gtk.STOCK_SAVE)
		tooltips.set_tip(save, "Save image")
		save.connect("clicked", self.callback_save)
		toolbar.insert(save, tool_bar_pos)
		tool_bar_pos=tool_bar_pos+1

		hide_key = gtk.ToolButton(gtk.STOCK_INFO)
		tooltips.set_tip(hide_key, "Hide key")
		hide_key.connect("clicked", self.callback_hide_key)
		toolbar.insert(hide_key, tool_bar_pos)
		tool_bar_pos=tool_bar_pos+1

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"play.png"))
		save = gtk.ToolButton(image)
		tooltips.set_tip(save, "Run simulation")
		save.connect("clicked", self.run_simulation)
		toolbar.insert(save, tool_bar_pos)
		tool_bar_pos=tool_bar_pos+1

		plot_toolbar = NavigationToolbar(canvas, self)
		plot_toolbar.show()
		box=gtk.HBox(True, 1)
		box.set_size_request(500,-1)
		box.show()
		box.pack_start(plot_toolbar, True, True, 0)
		tb_comboitem = gtk.ToolItem();
		tb_comboitem.add(box);
		tb_comboitem.show()
		toolbar.insert(tb_comboitem, tool_bar_pos)
		tool_bar_pos=tool_bar_pos+1

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

		toolbar.show_all()
		window_main_vbox=gtk.VBox()
		window_main_vbox.pack_start(toolbar, False, True, 0)
		#self.attach(toolbar, 0, 1, 0, 1)
		tool_bar_pos=tool_bar_pos+1


		self.hbox.pack_start(canvas, True, True, 0)
	
		self.emesh_editor.show()
		self.hbox.pack_start(self.emesh_editor, True, True, 0)
		self.emesh_editor.mesh_dump_ctl.connect("update", self.callback_update)
		window_main_vbox.add(self.hbox)

		self.add(window_main_vbox)
		self.set_title("Electrical Mesh Editor - (www.opvdm.com)")
		self.set_icon_from_file(os.path.join(get_image_file_path(),"mesh.png"))
		self.connect("delete-event", self.callback_close)
		self.set_position(gtk.WIN_POS_CENTER)


