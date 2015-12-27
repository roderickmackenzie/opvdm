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
import signal
import subprocess
from tempfile import mkstemp


class plot_dlg_class(gtk.Window):

	def populate_combo_box_using_input_file(self,combobox,input_file):
		try:
			f = open(os.path.join(self.path,input_file))
			lines = f.readlines()
			f.close()

			for i in range(0, len(lines) ,2):
				lines[i]=lines[i].rstrip()
				combobox.append_text(lines[i])

			combobox.set_active(0)
		except:
		
			combobox.get_model().clear()

	def callback_edit0(self, widget):
		print "Edit"
		self.populate_combo_box_using_input_file(self.token0,self.file0.get_text())

	def callback_edit1(self, widget):
		print "Edit"
		self.populate_combo_box_using_input_file(self.token1,self.file1.get_text())

	def callback_edit2(self, widget):
		print "Edit"
		self.populate_combo_box_using_input_file(self.token2,self.file2.get_text())

	def callback_click(self, widget,button,data):
		if button == "ok":
			data.file0=self.file0.get_text().decode('utf8')
			data.tag0=self.token0.get_active_text().decode('utf8')
			data.file1=self.file1.get_text().decode('utf8')
			data.tag1=self.token1.get_active_text().decode('utf8')
			data.file2=self.file2.get_text().decode('utf8')
			data.tag2=self.token2.get_active_text().decode('utf8')
			self.ret=True
		else:
			self.ret=False

		gtk.main_quit()

	def my_init(self,data):
		self.path=os.path.dirname(data.example_file0)
		self.ret=False
		self.set_title("xy plot www.opvdm.com")
		self.set_keep_above(True)

		#x-axis
		l=gtk.Label("x-axis:")
		l.show()
		vbox=gtk.VBox()
		hbox=gtk.HBox()
		self.file0 = gtk.Entry()
		self.file0.set_text(data.file0)
		self.file0.connect("changed", self.callback_edit0)
		self.file0.show()
		hbox.add(self.file0)


		self.token0 = gtk.combo_box_new_text()
		self.token0.set_wrap_width(5)
		self.populate_combo_box_using_input_file(self.token0, os.path.basename(data.example_file0))
		self.token0.show()
		hbox.add(self.token0)

		hbox.show()

		vbox.add(l)
		vbox.add(hbox)

		#y-axis
		l=gtk.Label("y-axis:")
		l.show()

		hbox=gtk.HBox()
		self.file1 = gtk.Entry()
		self.file1.set_text(data.file0)
		self.file1.connect("changed", self.callback_edit1)
		self.file1.show()
		hbox.add(self.file1)

		self.token1 = gtk.combo_box_new_text()
		self.token1.set_wrap_width(5)
		self.populate_combo_box_using_input_file(self.token1,os.path.basename(data.example_file1))
		self.token1.show()
		hbox.add(self.token1)

		hbox.show()
		vbox.add(l)
		vbox.add(hbox)

		#label
		l=gtk.Label("label:")
		l.show()

		hbox=gtk.HBox()
		self.file2 = gtk.Entry()
		self.file2.set_text(data.file0)
		self.file2.connect("changed", self.callback_edit2)
		self.file2.show()
		hbox.add(self.file2)

		self.token2 = gtk.combo_box_new_text()
		self.token2.set_wrap_width(5)
		self.populate_combo_box_using_input_file(self.token2,os.path.basename(data.example_file1))
		self.token2.show()
		hbox.add(self.token2)

		hbox.show()
		vbox.add(l)
		vbox.add(hbox)

		button_box=gtk.HBox()

		button_cancel=gtk.Button(stock=gtk.STOCK_CANCEL)
		button_cancel.show()
		button_box.add(button_cancel)
		button_cancel.connect("clicked",self.callback_click,"cancel",data)

		button_ok=gtk.Button(stock=gtk.STOCK_OK)
		button_ok.show()
		button_ok.connect("clicked",self.callback_click,"ok",data)
		button_box.add(button_ok)

		button_box.show()

		vbox.add(button_box)
		vbox.show()
		self.add(vbox)
		self.show()

	def my_run(self,data):
		self.show_all()

		gtk.main()
		self.destroy()
		return self.ret

