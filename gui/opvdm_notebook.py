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


import os
import pygtk
import gtk
from progress import progress_class
from gui_util import process_events
from inp import inp_get_token_value
from inp import inp_write_lines_to_file
from inp import inp_load_file
from inp import inp_get_next_token_array
from inp import inp_isfile
from inp import inp_update_token_value
from tab_main import tab_main
from tab import tab_class
from tab_homo import tab_bands
from welcome import welcome_class
from win_lin import running_on_linux
if running_on_linux()==True:
	from tab_terminal import tab_terminal
from dos_main import dos_main
from pl_main import pl_main

from cal_path import get_bin_path
from cal_path import get_image_file_path
from inp_description import inp_file_to_description

import i18n
_ = i18n.language.gettext

class opvdm_notebook(gtk.Notebook):
	progress=progress_class()
	finished_loading=False
	item_factory=None
	menu_items=[]

	if running_on_linux()==True:
		terminal_widget=tab_terminal()
		terminal_widget.init()
		terminal=terminal_widget.terminal
	else:
		terminal_widget=None
		terminal=None

	def goto_page(self,page):
		for i in range(0,len(self.get_children())):
				if self.get_nth_page(i).label_name==page:
					self.set_current_page(i)
					break

	def callback_close_button(self, widget, data):
		self.toggle_tab_visible(data)

	def callback_switch_page(self, notebook, page, page_num):
		if self.last_page!=page_num:
			self.last_page=page_num
			self.get_children()[page_num].help()
			#print "Rod", page_num
		#self.toggle_tab_visible(data)

	def clean_menu(self):
		for menu in self.menu_items:
			self.item_factory.delete_item(menu)
		self.menu_items=[]

	def callback_view_toggle(self, widget, data):
		self.toggle_tab_visible(data.get_label())

	def set_item_factory(self,item_factory):
		self.item_factory=item_factory

	def toggle_tab_visible(self,name):
		if self.finished_loading==True:
			for child in self.get_children():
				if child.label_name==name:
					if child.visible==False:
						if self.item_factory!=None:
							widget=self.item_factory.get_widget(_("/View/")+name)
							widget.set_active(True)
						child.show()
						child.visible=True
					else:
						if self.item_factory!=None:
							widget=self.item_factory.get_widget(_("/View/")+name)
							widget.set_active(False)
						child.hide()
						child.visible=False

					#print "gui_config.inp", "#"+child.file_name, str(int(child.visible)),2
					inp_update_token_value("gui_config.inp", "#"+child.file_name, str(int(child.visible)),1)

	def add_to_menu(self,name,visible):
		print _("/View/")+name
		a = (( _("/View/")+name,  None, self.callback_view_toggle, 0, "<ToggleItem>" ),   )
		self.item_factory.create_items( a, )
		path=_("/View/")+name
		myitem=self.item_factory.get_item(path)
		self.menu_items.append(path)
		myitem.set_active(visible)

	def add_welcome_page(self):
		self.welcome=welcome_class()
		self.welcome.init(os.path.join(get_image_file_path(),"image.jpg"))
		self.welcome.web.connect("got-data", self.welcome.update)
		self.welcome.get_data()
		self.welcome.show()
		self.append_page(self.welcome, gtk.Label(_("Information")))

	def load(self):
		self.clean_menu()
		self.last_page=0
		#print "paths",os.getcwd(),get_bin_path(),(os.path.normcase(os.getcwd())!=os.path.normcase(get_bin_path()))
		if (os.path.exists("sim.opvdm")==True) and (os.path.normcase(os.getcwd())!=os.path.normcase(get_bin_path())):
			self.finished_loading=False
			self.progress.init()
			self.progress.show()
			self.progress.start()
			self.progress.set_text("Loading..")
			process_events()

			for child in self.get_children():
					self.remove(child)

			dos_files=inp_get_token_value("device_epitaxy.inp", "#layers")

			self.main_tab=tab_main()
			self.main_tab.init()
			self.main_tab.show()
			self.append_page(self.main_tab, gtk.Label(_("Device structure")))

			lines=[]
			pos=0
			if inp_load_file(lines,"gui_config.inp")==True:
				pos=0
				tab_number=0
				tabs=(len(lines)-3)/2
				print "tabs=",tabs
				while (1):
					add_to_widget=False
					ret,pos=inp_get_next_token_array(lines,pos)

					if ret[0]=="#ver":
						break

					file_name=ret[0]
					if file_name[0]=="#":
						file_name=file_name[1:]
					name=inp_file_to_description(file_name)
					if name==False:
						print "name not found",name
						break
					visible=bool(int(ret[1]))

					self.progress.set_fraction(float(tab_number)/float(tabs))

					tab_number=tab_number+1
					self.progress.set_text(_("Loading ")+name)
					process_events()
					if file_name=="pl0.inp":
						tab=pl_main()
						tab.init()
						tab.update()
						add_to_widget=True
						tab.visible=visible
						tab.label_name=name
						tab.file_name=file_name
					elif file_name=="epitaxy.inp":
						tab=dos_main()
						tab.init()
						tab.update()
						add_to_widget=True
						tab.visible=visible
						tab.label_name=name
						tab.file_name=file_name
					elif file_name=="lumo0.inp":
						tab=tab_bands()
						tab.update()
						if tab.enabled==True:
							add_to_widget=True
							tab.visible=visible
							tab.wow()
							tab.label_name=name
							tab.file_name=file_name

					elif inp_isfile(file_name)==True:
						add_to_widget=True
						tab=tab_class()
						tab.visible=visible
						tab.init(file_name,name)
						tab.label_name=name
						tab.file_name=file_name

					if add_to_widget==True:
						#print file_name,name,visible,type(tab)
						hbox=gtk.HBox()
						hbox.set_size_request(-1, 25)
						mytext=name
						if len(mytext)<10:
							for i in range(len(mytext),10):
								mytext=mytext+" "

						label=gtk.Label(mytext)
						label.set_justify(gtk.JUSTIFY_LEFT)
						self.connect("switch-page", self.callback_switch_page)
						hbox.pack_start(label, False, True, 0)

						button = gtk.Button()
						close_image = gtk.Image()
						close_image.set_from_file(os.path.join(get_image_file_path(),"close.png"))
						print os.path.join(get_image_file_path(),"close.png")
						close_image.show()
						button = gtk.Button()
						button.add(close_image)


						button.props.relief = gtk.RELIEF_NONE
						button.connect("clicked", self.callback_close_button,name)
						button.set_size_request(25, 25)
						button.show()
					

						hbox.pack_end(button, False, False, 0)
						hbox.show_all()

						if (visible==True):
							tab.show()

						self.append_page(tab,hbox)



						self.set_tab_reorderable(tab,True)

						self.add_to_menu(name,visible)

			else:
				print _("No gui_config.inp file found\n")

			#for child in self.get_children():
			#		print type(child)

			if self.terminal_widget!=None:
				self.terminal_widget.show()
				self.append_page(self.terminal_widget, gtk.Label(_("Terminal")))


			self.add_welcome_page()

			self.finished_loading=True
			self.progress.stop()
			self.progress.set_fraction(0.0)
			self.goto_page("tab_main")

			self.set_tab_pos(gtk.POS_TOP)
			#self.set_current_page(1)

			return True
		else:
			self.add_welcome_page()
			self.goto_page(_("Welcome"))
			return False



