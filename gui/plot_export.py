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
from util_text import gkt_title_to_gnu_plot_title
from plot_state import plot_state

def plot_export(dest_dir,input_files,state,fig):
	file_ext=os.path.splitext(dest_dir)[1]
	print file_ext

	if file_ext=="":
		print "Saving as tex file"
		if os.path.isdir(dest_dir)==False:
			os.makedirs(dest_dir)

		plot_file=dest_dir+'/plot.plot'

		#build plot script
		plot_string="set term postscript eps enhanced solid color \"Helvetica\" 28\n"
		plot_string=plot_string+"set key top left\n"
		plot_string=plot_string+"set format y \"%1.0t{/Symbol \\327}10^{%L}\"\n"
		plot_string=plot_string+"set format x \"%1.0t{/Symbol \\327}10^{%L}\"\n"
		plot_string=plot_string+"set xlabel '"+gkt_title_to_gnu_plot_title(state.x_label)+" ("+state.x_units+")'\n"
		plot_string=plot_string+"set ylabel '"+gkt_title_to_gnu_plot_title(state.y_label)+" ("+state.y_units+")'\n"
		#plot_string=plot_string+"set format y '%.1le%T'\n"

		if (state.logy==1):
			plot_string=plot_string+"set logscale y\n"

		if (state.logx==1):
			plot_string=plot_string+"set logscale x\n"

		plot_string=plot_string+"plot \\\n"
		for i in range(0, len(input_files)):
			line_name=os.path.basename(os.path.dirname(input_files[i]))
			plot_string=plot_string+"'"+input_files[i]+"' using ($1*"+str(state.x_mul)+"):($2*"+str(state.y_mul)+") with l lw 7 title '"+line_name+" "+state.key_units+"',\\\n"

		plot_string=plot_string[:-3]



		text_file = open(plot_file, "w")
		text_file.write(plot_string)
		text_file.close()

		#build make file
		makefile_string="main:\n"
		makefile_string=makefile_string+"\tgnuplot plot.plot >plot.eps\n"
		makefile_string=makefile_string+"\tgs -dNOPAUSE -r600 -dEPSCrop -sDEVICE=jpeg -sOutputFile=plot.jpg plot.eps -c quit\n"
		makefile_string=makefile_string+"\tgnome-open plot.jpg\n"

		text_file = open(dest_dir+"/makefile", "w")
		text_file.write(makefile_string)
		text_file.close()

	if file_ext==".png":
		print "Saving png as",dest_dir
		fig.savefig(dest_dir)
