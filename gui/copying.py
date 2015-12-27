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

class copying:
	def __init__(self):
		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
		self.window.set_position(gtk.WIN_POS_CENTER)
		self.window.set_title('Organic Photovoltaic Device Model (www.opvdm.com) - activation')
		#self.window.connect("delete_event", self.delete_event)
		#self.window.connect("destroy", self.destroy)
		self.window.set_border_width(10)
		box = gtk.VBox()
		label = gtk.Label()
		label.set_markup("<big><b>Organic photovoltaic device model</b>\n"+
				 "(<a href=\"http://www.opvdm.com\" "+
                         "title=\"Click to find out more\">www.opvdm.com</a>)\n\n"
				+"Dear User,\n\n"
				+"I distribute two versions of opvdm, an <b>open source version</b> which you can download at\n"
				+"<a href=\"http://www.opvdm.com\" title=\"Click to find out more\">www.opvdm.com</a> and a <b>closed source version</b> with more features.\n"
				+"It looks like you have the closed source version.\n"
				+"\nTo activate it do the following:\n"
				+"<b>1.</b> Click on the button below called \"Generate key\"\n"
				+"<b>2.</b> Save the license key file to your desktop (or somewhere else convenient)\n"
				+"<b>3.</b> Send key.dat to <a href=\"mailto:roderick.mackenzie\@nottingham.ac.uk\" >roderick.mackenzie@nottingham.ac.uk</a>\n"
				+"<b>4.</b> I will send you back an activation key.\n"
				+"<b>5.</b> Copy this key into your home directory.\n\n"
				+"Rod\n18/10/13\n"
				+"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n\n"
				+"</big>")
		box.add(label)

		label.show()


		button = gtk.Button("Generate key")
		button.connect("clicked", self.callback_gen_key, None)
		#self.button.connect_object("clicked", gtk.Widget.destroy, self.window)
		button.show()
		box.add(button)
		box.show()
		self.window.add(box)

	def wow(self,exe_command):
		self.exe_command=exe_command
		self.window.show()

	def callback_gen_key(self, widget, data=None):
		dialog = gtk.FileChooserDialog("Select directory to save the key to..",None,
		gtk.FILE_CHOOSER_ACTION_SAVE,
		(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
		gtk.STOCK_SAVE, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)

		dialog.set_action(gtk.FILE_CHOOSER_ACTION_SELECT_FOLDER)
		filter = gtk.FileFilter()
		filter.set_name("*.")
		filter.add_pattern("*.")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			cmd = self.exe_command + ' --genkey-dir '+ dialog.get_filename()+'/'
			text='I have generated file '+str(dialog.get_filename())+'/key.dat.'		
			ret= os.system(cmd)
			dialog.destroy()

			message = gtk.MessageDialog(type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_OK)
						
			message.set_markup(text)
			message.run()
			message.destroy()

		elif response == gtk.RESPONSE_CANCEL:
			print 'Closed, no files selected'
		dialog.destroy()


