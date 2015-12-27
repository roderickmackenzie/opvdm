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
from token_lib import tokens
from numpy import *
from util import pango_to_gnuplot
from plot_window import plot_window
from matplotlib.widgets import Cursor
from inp import inp_load_file
from inp import inp_search_token_value

window=None

destroy=False

def set_plot_auto_close(value):
	global destroy
	destroy=value

def plot_gen(input_files,plot_labels,config_file):
	my_config_file=config_file
	if config_file=="auto":
		my_config_file=os.path.splitext(input_files[0])[0]+".oplot"

	if (len(input_files)==1):
		if os.path.splitext(input_files[0])[1]==".plot":
			plot_file=input_files[0]
			cmd = 'gnuplot -persist '+plot_file
			os.system(cmd)
			return

	global window
	global destroy
	if window!=None:
		if window.shown==True:
			if destroy==True:
				window.input_files=input_files
				window.plot.set_labels(plot_labels)
				window.plot.load_data(input_files,my_config_file)
				window.plot.do_plot()
				window.plot.fig.canvas.draw()
				window.window.present()
				window.window.set_keep_above(True)
				return

	window=plot_window()
	window.init(input_files,plot_labels,my_config_file)


