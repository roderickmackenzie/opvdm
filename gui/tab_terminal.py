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
from scan_item import scan_item
import vte
from tab_base import tab_base
from help import my_help_class

class tab_terminal(gtk.VBox,tab_base):

	label_name="Terminal"

	def init(self):
		self.main_box=gtk.VBox()

		self.terminal     = vte.Terminal()
		self.terminal.fork_command("/bin/sh")
		self.terminal.feed_child('opvdm --version\n')
		self.terminal.set_scrollback_lines(10000)
		self.terminal.show()
		self.main_box.add(self.terminal)

		self.add(self.main_box)
		self.main_box.show()

	def help(self):
		my_help_class.help_set_help(["command.png","<big><b>The terminal window</b></big>\nThe model will run in this window.  You can also use it to enter bash commands."])

