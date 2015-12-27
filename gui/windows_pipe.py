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
import time
import re
import gobject
import win32pipe
import win32file
import winerror
from threading import Thread
import threading


class win_pipe(gobject.GObject):
	def __init__(self):
		self.__gobject_init__()

	def foo(self,n):
		p = win32pipe.CreateNamedPipe(r'\\.\pipe\opvdm_pipe',
			win32pipe.PIPE_ACCESS_INBOUND,
			win32pipe.PIPE_TYPE_MESSAGE | win32pipe.PIPE_WAIT,
			1, 65536, 65536,300,None)

		while(1):
			#print "going to read"
			win32pipe.ConnectNamedPipe(p, None)
			res,data = win32file.ReadFile(p, 4096)
			#print res,data
			if res != winerror.ERROR_MORE_DATA:
				#print data
				#print "emit event from",threading.current_thread()
				#gobject.idle_add(gobject.GObject.emit,self,*args)
				#self.emit("new-data",data)
				gobject.idle_add(gobject.GObject.emit,self,"new-data",data)
			else:
				print "no more data"
			win32pipe.DisconnectNamedPipe(p)

	def start(self):
		p = Thread(target=self.foo, args=(10,))
		p.daemon = True
		p.start()


gobject.type_register(win_pipe)
gobject.signal_new("new-data", win_pipe, gobject.SIGNAL_RUN_FIRST,gobject.TYPE_NONE, (gobject.TYPE_STRING,))

