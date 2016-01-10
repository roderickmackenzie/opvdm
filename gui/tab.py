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
from scan_item import scan_item_add
from token_lib import tokens
from util_zip import check_is_config_file
from inp import inp_update_token_value
from inp import inp_get_token_value
from undo import undo_list_class
import zipfile
import base64
from tab_base import tab_base
from util import str2bool
from scan_item import scan_remove_file
from inp import inp_load_file
from help import my_help_class
from util import latex_to_pygtk_subscript
from i18n import yes_no

class tab_class(gtk.VBox,tab_base):
	
	lines=[]
	edit_list=[]

	line_number=[]


	def callback_edit(self, widget, data=None):
		if type(widget)==gtk.Entry:
			a=undo_list_class()
			a.add([self.file_name, data, inp_get_token_value(self.file_name, data),widget])
			inp_update_token_value(self.file_name, data, widget.get_text(),1)
		else:
			inp_update_token_value(self.file_name, data, widget.get_active_text(),1)

		my_help_class.help_set_help(["save.png","<big><b>Saved to disk</b></big>\n"])

	def help(self):
		my_help_class.get_help(self.file_name)

	def init(self,filename,fullname):
		scan_remove_file(filename)
		self.widget_type=[]
		self.file_name=filename
		self.edit_list=[]
		self.line_number=[]
		inp_load_file(self.lines,filename)

		n=0
		pos=0
		my_token_lib=tokens()
		height=27
		for i in range(0, len(self.lines)/2):

			show=False
			units="Units"
			token=self.lines[pos]
			result=my_token_lib.find(token)
			if result!=False:
				units=result.units
				text_info=result.info
				show=True
			pos=pos+1
			self.set_size_request(600,-1)
			if show == True :
				hbox=gtk.HBox()
				hbox.show()
				label = gtk.Label()
				label.set_size_request(400,height)
				label.set_markup(latex_to_pygtk_subscript(text_info))
				label.set_use_markup(True)
				hbox.pack_start(label, False, False, padding=1)
				label.show()

				self.line_number.append(pos)

				if result.opt[0]=="text":
					edit_box=gtk.Entry(max=0)
					edit_box.set_text(self.lines[pos]);
					edit_box.connect("changed", self.callback_edit, token)
					edit_box.show()
					self.widget_type.append("edit")
				else:
					edit_box=gtk.combo_box_new_text()
					index=0
					true_false=False
					if len(result.opt)==2:
						if result.opt[0]==_("true") and result.opt[1]==_("false"):
							true_false=True

					for i in range(0,len(result.opt)):
						edit_box.append_text(result.opt[i])
						if true_false==False:
							if yes_no(self.lines[pos])==yes_no(result.opt[i]):
								index=i
						else:
							if str2bool(self.lines[pos])==True:
								index=0
							else:
								index=1

					edit_box.set_active(index);
					
					edit_box.connect("changed", self.callback_edit, token)
					edit_box.show()
					self.widget_type.append("combo")
				edit_box.set_size_request(300,height)
				self.edit_list.append(edit_box)
				hbox.pack_start(edit_box, False, False, padding=1)

				label = gtk.Label()
				label.set_markup(latex_to_pygtk_subscript(units))
				label.set_use_markup(True)
				label.set_size_request(200,height)
				label.show()
				hbox.pack_start(label, False, False, padding=1)
				label.show()
				self.pack_start(hbox, False, False, padding=1)
				#self.add()
				line=1
				scan_item_add(filename,token,text_info,line)
				
				n=n+1

			pos=pos+1
