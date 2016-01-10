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

class qe_window(gtk.Window):
	visible=1


	def update_graph(self):
		cmd = self.exe_command+' --onlypos'
		ret= os.system(cmd)
		self.fig.clf()
		self.draw_graph()
		self.fig.canvas.draw()

	def draw_graph(self):

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
			t,s = loadtxt("Ec.dat", unpack=True)
			t=t*1e9
			Ec, = self.ax1.plot(t,s, 'ro-', linewidth=3 ,alpha=0.5)

			t,s = loadtxt("Ev.dat", unpack=True)
			t=t*1e9
			Ev,=self.ax1.plot(t,s, 'go-', linewidth=3 ,alpha=0.5)

			t,s = loadtxt("Fi.dat", unpack=True)
			t=t*1e9
			Fi,=self.ax1.plot(t,s, 'bo-', linewidth=3 ,alpha=0.5)
			
			if self.show_key==True:
				self.fig.legend((Ec, Ev, Fi), ('LUMO', 'HOMO', 'Fi'), 'upper right')
			else:
				self.ax1.legend_ = None
			self.fig.canvas.draw()
			
		except:
			print "No mode file\n"

		
	def save_image(self,file_name):
		self.fig.savefig(file_name)	

	def callback_refresh(self, widget, data=None):
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


	def init(self):
		self.fig = Figure(figsize=(5,4), dpi=100)
		self.ax1=None
		self.show_key=True
		self.hbox=gtk.HBox()
		self.exe_command  =  get_exe_command()
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
		save.connect("clicked", self.callback_refresh)
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

		toolbar.show_all()
		window_main_vbox=gtk.VBox()
		window_main_vbox.pack_start(toolbar, False, True, 0)
		#self.attach(toolbar, 0, 1, 0, 1)
		tool_bar_pos=tool_bar_pos+1




		self.hbox.pack_start(canvas, True, True, 0)
		#self.attach(canvas, 1, 3, 0, 1)



		vbox = gtk.VBox(False, 2)
	

		#spacer
		label=gtk.Label(" \n\n    ")
		#self.attach(label, 4, 5, 1, 2,gtk.SHRINK ,gtk.SHRINK)
		vbox.pack_start(label, False, False, 0)

		label.show()


		hbox = gtk.HBox(False, 2)
	    
		hbox.show()
		self.hbox.pack_start(vbox, False, False, 0)
		#self.attach(vbox, 3, 4, 0, 1,gtk.SHRINK ,gtk.SHRINK)
		vbox.show()
		window_main_vbox.add(self.hbox)
		self.add(window_main_vbox)
		self.set_title("Quantum Efficency calculator - (www.opvdm.com)")
		self.set_icon_from_file(os.path.join(get_image_file_path(),"qe.png"))
		self.connect("delete-event", self.callback_close)
		self.set_position(gtk.WIN_POS_CENTER)


