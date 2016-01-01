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
import commands
import subprocess
from win_lin import running_on_linux
from cal_path import get_exe_command
import urllib2
import socket 
from threading import Thread
import time
from socket import setdefaulttimeout
from socket import socket
from socket import error
from socket import AF_INET
from socket import SOCK_STREAM
from socket import SOL_SOCKET
from socket import SO_REUSEADDR
from socket import getdefaulttimeout
import urlparse
import re
import os
from ver import ver_core
from ver import ver_mat
from ver import ver_gui
import gobject
import platform
import getpass
from tab_base import tab_base
from help import my_help_class

socket.setdefaulttimeout = 1.0
os.environ['no_proxy'] = '127.0.0.1,localhost'
linkRegex = re.compile('<a\s*href=[\'|"](.*?)[\'"].*?>')
CRLF = "\r\n\r\n"

#This is the welcome tab.  It sends a request to www.opvdm.com/update.php and then displays the text it fetches in the tab.  The idea of this is to tell people when updates are available.

class web_thread(gtk.VBox):
	def __init__(self):
		self.__gobject_init__()
		self.text="none"

	def get_from_web(self,url):
		setdefaulttimeout(4.0)
		url = urlparse.urlparse(url)
		HOST = url.netloc
		PORT = 80
		try:
			s = socket(AF_INET, SOCK_STREAM)
		except error as msg:
			s = None

		s.settimeout(4.0)


		s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

		try:
			s.connect((HOST, PORT))
		except error as msg:
			s.close()
			s = None

		if s!=None:
			s.send("GET http://www.opvdm.com/update.php?ver_core="+ver_core()+"&ver_gui="+ver_gui()+"&ver_mat="+ver_mat()+"&os="+platform.platform()+"&"+" HTTP/1.0" +CRLF)
			data = (s.recv(1000000))

			s.shutdown(1)
			s.close()
			self.text=data.split('charset=UTF-8\r\n\r\n', 1)[-1]
			gobject.idle_add(gobject.GObject.emit,self,"got-data")
			#self.emit("got-data")

	def foo(self,n):
		self.get_from_web('http://www.opvdm.com')

	def start(self):
		p = Thread(target=self.foo, args=(10,))
		#multiprocessing.Process(target=self.foo, name="Foo", args=(10,))
		p.daemon = True
		p.start()

class welcome_class(gtk.HBox,tab_base):
	
	lines=[]
	edit_list=[]
	file_name=""
	line_number=[]
	save_file_name=""
	name="Welcome"

	def delete_event(self, widget, event, data=None):
		gtk.main_quit()
		return False

	def get_data(self):
		self.web.start()

	def init(self,image_name):
		self.label = gtk.Label()
		self.web=web_thread()
		#self.web.connect("got-data", self.update)

		self.text=_("<big><b>Organic photovoltaic device model</b>\n(<a href=\"http://www.opvdm.com\" title=\"Click to find out more\">www.opvdm.com</a>)\n\n To make a new simulation directory click <i>new</i> in the <i>file</i> menu\n or to open an existing simulation click on the <i>open</i> button.\n There is more help on the <a href=\"http://www.opvdm.com/man/index.html\">man pages</a>.  Please report bugs to\nroderick.mackenzie@nottingham.ac.uk.\n\n Rod\n18/10/13\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n\nChecking web for updates.....")
		read_page=False


		self.label.set_markup(self.text+"</big>")
		self.label.set_alignment(0, 0.5)
		self.pack_start(self.label, True, True, 0)
		image = gtk.Image()
   		image.set_from_file(image_name)
		self.pack_start(image, False, False, 0)

   		image.show()
		self.label.show()


	def update(self,data):
		self.text=self.web.text
		self.label.set_markup(self.text+"</big>")
		#self.hide_all()

	def help(self):
		my_help_class.help_set_help(["icon.png",_("<big><b>Welcome to opvdm</b></big>\n The window will provide you with information about new versions and bugs in opvdm.")])

gobject.type_register(web_thread)
gobject.signal_new("got-data", web_thread, gobject.SIGNAL_RUN_FIRST,gobject.TYPE_NONE, ())

