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
from numpy import arange, sin, pi
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
import gobject
import os, fnmatch
from plot_gen import plot_gen
import zipfile
import glob
from scan_item import scan_item_add
from util import lines_to_xyz
from tab import tab_class
from win_lin import running_on_linux
from photon_dist import photon_dist_class
from plot_widget import plot_widget
from plot_state import plot_state
from plot_io import plot_load_info
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
from matplotlib.figure import Figure
from plot_io import get_plot_file_info

class band_graph(gtk.VBox):
	def init(self):

		toolbar = gtk.Toolbar()

		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 50)
		self.pack_start(toolbar, False, False, 0)

		tool_bar_pos=0
		save = gtk.ToolButton(gtk.STOCK_SAVE)
		save.connect("clicked", self.callback_save_image)
		toolbar.insert(save, tool_bar_pos)
		toolbar.show_all()
		tool_bar_pos=tool_bar_pos+1

		self.my_figure=Figure(figsize=(5,4), dpi=100)
		self.canvas = FigureCanvas(self.my_figure)  # a gtk.DrawingArea
		self.canvas.figure.patch.set_facecolor('white')
		self.canvas.set_size_request(600, 400)
		self.canvas.show()
		self.pack_start(self.canvas, False, False, 0)

		self.canvas.connect('key_press_event', self.on_key_press_event)

		self.show_all()

	def on_key_press_event(self,widget, event):
		keyname = gtk.gdk.keyval_name(event.keyval)
		if keyname == "c":
			if event.state == gtk.gdk.CONTROL_MASK:
				self.do_clip()

		self.canvas.draw()

	def do_clip(self):
		print "doing clip"
		snap = self.my_figure.canvas.get_snapshot()
		pixbuf = gtk.gdk.pixbuf_get_from_drawable(None, snap, snap.get_colormap(),0,0,0,0,snap.get_size()[0], snap.get_size()[1])
		clip = gtk.Clipboard()
		clip.set_image(pixbuf)

	def callback_save_image(self, widget):
		dialog = gtk.FileChooserDialog("Save plot",
                               None,
                               gtk.FILE_CHOOSER_ACTION_SAVE,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_SAVE, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)
		dialog.set_action(gtk.FILE_CHOOSER_ACTION_CREATE_FOLDER)

		filter = gtk.FileFilter()
		filter.set_name("png")
		filter.add_pattern("*.png")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			self.my_figure.savefig(dialog.get_filename())

		elif response == gtk.RESPONSE_CANCEL:
		    print 'Closed'
		dialog.destroy()

	def set_data_file(self,file):
		self.optical_mode_file=os.path.join(os.getcwd(),"light_dump",file)

	def draw_graph(self):
		self.layer_end=[]
		self.layer_name=[]

		n=0
		self.my_figure.clf()
		ax1 = self.my_figure.add_subplot(111)
		ax2 = ax1.twinx()
		x_pos=0.0
		layer=0
		color =['r','g','b','y','o','r','g','b','y','o']
		start=0.0

		for i in range(0,epitaxy_get_layers()):
			if epitaxy_get_electrical_layer(i)=="none":
				start=start-epitaxy_get_width(i)
			else:
				break
		print "START=",start
		start=start*1e9

		x_pos=start
		for i in range(0,epitaxy_get_layers()):

			label=epitaxy_get_mat_file(i)
			layer_ticknes=epitaxy_get_width(i)
			layer_material=epitaxy_get_mat_file(i)

			delta=float(layer_ticknes)*1e9
			if epitaxy_get_electrical_layer(i)=="none":
				mat_file=os.path.join(os.getcwd(),'materials',layer_material,'mat.inp')
				myfile = open(mat_file)
				self.mat_file_lines = myfile.readlines()
				myfile.close()
			
				for ii in range(0, len(self.mat_file_lines)):
					self.mat_file_lines[ii]=self.mat_file_lines[ii].rstrip()

				lumo=-float(self.mat_file_lines[1])
				Eg=float(self.mat_file_lines[3])
			else:
				lines=[]
				if inp_load_file(lines,epitaxy_get_electrical_layer(i)+".inp")==True:
					lumo=-float(inp_search_token_value(lines, "#Xi"))
					Eg=float(inp_search_token_value(lines, "#Eg"))

			x = [x_pos,x_pos+delta,x_pos+delta,x_pos]

			lumo_delta=lumo-0.1
			homo=lumo-Eg
			homo_delta=homo-0.1
			if Eg==0.0:
				lumo_delta=-7.0
				homo=0.0
			lumo_shape = [lumo,lumo,lumo_delta,lumo_delta]
			x_pos=x_pos+delta
			self.layer_end.append(x_pos)
			self.layer_name.append(layer_material)
			ax2.fill(x,lumo_shape, color[layer],alpha=0.4)
			ax2.text(x_pos-delta/1.5, lumo-0.4, epitaxy_get_name(i))

			if homo!=0.0:
				homo_shape = [homo,homo,homo_delta,homo_delta]
				ax2.fill(x,homo_shape, color[layer],alpha=0.4)

			layer=layer+1

			n=n+1

		state=plot_state()
		get_plot_file_info(state,self.optical_mode_file)
		#summary="<big><b>"+self.store[path[0]][0]+"</b></big>\n"+"\ntitle: "+state.title+"\nx axis: "+state.x_label+" ("+latex_to_pygtk_subscript(state.x_units)+")\ny axis: "++" ("+latex_to_pygtk_subscript(state.y_units)+")\n\n<big><b>Double click to open</b></big>"

		print "ROD!!!!",state.y_label,self.optical_mode_file
		ax1.set_ylabel(state.y_label)
		ax1.set_xlabel('Position (nm)')
		ax2.set_ylabel('Energy (eV)')
		ax2.set_xlim([start, x_pos])
		#ax2.axis(max=)#autoscale(enable=True, axis='x', tight=None)
		loaded=False

		if os.path.isfile("light_dump.zip"):
			zf = zipfile.ZipFile("light_dump.zip", 'r')
			lines = zf.read(self.optical_mode_file).split("\n")
			zf.close()
			loaded=True
		elif os.path.isfile(self.optical_mode_file):
			print "I want to load",self.optical_mode_file
			f = open(self.optical_mode_file)
			lines = f.readlines()
			f.close()
			loaded=True
		
		if loaded==True:
			xx=[]
			yy=[]
			zz=[]
			lines_to_xyz(xx,yy,zz,lines)
			t = asarray(xx)
			s = asarray(yy)

			t=t*1e9
			ax1.plot(t,s, 'black', linewidth=3 ,alpha=0.5)

			

		self.my_figure.tight_layout()


