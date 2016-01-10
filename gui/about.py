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
from ver import ver
from notice import notice
from cal_path import get_image_file_path

def about_dialog_show():
	about = gtk.AboutDialog()
	about.set_program_name("opvdm")
	about.set_version("")
	about.set_copyright("Written by Roderick MacKenzie 2014, published under GPL v2.0")
	about.set_comments(ver()+"\n"+notice())
	about.set_website("http://www.opvdm.com")

	image=os.path.join(get_image_file_path(),"image.jpg")
	about.set_logo(gtk.gdk.pixbuf_new_from_file(image))
	about.run()
	about.destroy()
