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
from inp import inp_load_file
from inp import inp_search_token_value
from util_zip import zip_get_data_file
from util import str2bool
from inp import inp_save_lines

def plot_load_info(plot_token,file_name_in):
	#print "whoo",file_name_in
	#file_name=os.path.splitext(file_name_in)[0]+".dat"

	ret=False

	config_file=os.path.splitext(file_name_in)[0]+".oplot"
	ret=plot_load_oplot_file(plot_token,config_file)
	if ret==False:
		ret=get_plot_file_info(plot_token,file_name_in)
	else:
		print "not loaded oplot file",config_file

	return ret

def plot_load_oplot_file(plot_token,file_name):
	lines=[]
	if inp_load_file(lines,file_name)==True:
		plot_token.logy=str2bool(inp_search_token_value(lines, "#logy"))
		plot_token.logx=str2bool(inp_search_token_value(lines, "#logx"))
		plot_token.grid=str2bool(inp_search_token_value(lines, "#grid"))
		plot_token.invert_y=str2bool(inp_search_token_value(lines, "#invert_y"))
		plot_token.normalize=str2bool(inp_search_token_value(lines, "#normalize"))
		plot_token.norm_to_peak_of_all_data=str2bool(inp_search_token_value(lines, "#norm_to_peak_of_all_data"))
		plot_token.subtract_first_point=str2bool(inp_search_token_value(lines, "#subtract_first_point"))
		plot_token.add_min=str2bool(inp_search_token_value(lines, "#add_min"))
		plot_token.file0=inp_search_token_value(lines, "#file0")
		plot_token.file1=inp_search_token_value(lines, "#file1")
		plot_token.file2=inp_search_token_value(lines, "#file2")
		plot_token.tag0=inp_search_token_value(lines, "#tag0")
		plot_token.tag1=inp_search_token_value(lines, "#tag1")
		plot_token.tag2=inp_search_token_value(lines, "#tag2")
		plot_token.legend_pos=inp_search_token_value(lines, "#legend_pos")
		plot_token.key_units=inp_search_token_value(lines, "#key_units")
		plot_token.label_data=str2bool(inp_search_token_value(lines, "#label_data"))
		plot_token.type=inp_search_token_value(lines, "#type")
		plot_token.x_label=inp_search_token_value(lines, "#x_label")
		plot_token.y_label=inp_search_token_value(lines, "#y_label")
		plot_token.x_units=inp_search_token_value(lines, "#x_units")
		plot_token.y_units=inp_search_token_value(lines, "#y_units")
		plot_token.x_mul=float(inp_search_token_value(lines, "#x_mul"))
		plot_token.y_mul=float(inp_search_token_value(lines, "#y_mul"))
		plot_token.key_units=inp_search_token_value(lines, "#key_units")
		plot_token.x_start=float(inp_search_token_value(lines, "#x_start"))
		plot_token.x_stop=float(inp_search_token_value(lines, "#x_stop"))
		plot_token.x_points=float(inp_search_token_value(lines, "#x_points"))
		plot_token.y_start=float(inp_search_token_value(lines, "#y_start"))
		plot_token.y_stop=float(inp_search_token_value(lines, "#y_stop"))
		plot_token.y_points=float(inp_search_token_value(lines, "#y_points"))
		plot_token.time=float(inp_search_token_value(lines, "#time"))
		plot_token.Vexternal=float(inp_search_token_value(lines, "#Vexternal"))

		return True
	return False

def plot_save_oplot_file(config_file,plot_token):
	save_name=config_file
	if save_name!="":
		if save_name.endswith(".oplot")==False:
			save_name=save_name.split(".")[0]+".oplot"

		lines=[]
		lines.append("#logy")
		lines.append(str(plot_token.logy))
		lines.append("#logx")
		lines.append(str(plot_token.logx))
		lines.append("#grid")
		lines.append(str(plot_token.grid))
		lines.append("#invert_y")
		lines.append(str(plot_token.invert_y))
		lines.append("#normalize")
		lines.append(str(plot_token.normalize))
		lines.append("#norm_to_peak_of_all_data")
		lines.append(str(plot_token.norm_to_peak_of_all_data))
		lines.append("#subtract_first_point")
		lines.append(str(plot_token.subtract_first_point))
		lines.append("#add_min")
		lines.append(str(plot_token.add_min))
		lines.append("#file0")
		lines.append(plot_token.file0)
		lines.append("#file1")
		lines.append(plot_token.file1)
		lines.append("#file2")
		lines.append(plot_token.file2)
		lines.append("#tag0")
		lines.append(plot_token.tag0)
		lines.append("#tag1")
		lines.append(plot_token.tag1)
		lines.append("#tag2")
		lines.append(plot_token.tag2)
		lines.append("#legend_pos")
		lines.append(plot_token.legend_pos)
		lines.append("#key_units")
		lines.append(plot_token.key_units)
		lines.append("#label_data")
		lines.append(str(plot_token.label_data))
		lines.append("#type")
		lines.append(plot_token.type)
		lines.append("#x_label")
		lines.append(plot_token.x_label)
		lines.append("#y_label")
		lines.append(plot_token.y_label)
		lines.append("#x_units")
		lines.append(plot_token.x_units)
		lines.append("#y_units")
		lines.append(plot_token.y_units)
		lines.append("#x_mul")
		lines.append(str(plot_token.x_mul))
		lines.append("#y_mul")
		lines.append(str(plot_token.y_mul))
		lines.append("#key_units")
		lines.append(plot_token.key_units)
		lines.append("#x_start")
		lines.append(str(plot_token.x_start))
		lines.append("#x_stop")
		lines.append(str(plot_token.x_stop))
		lines.append("#x_points")
		lines.append(str(plot_token.x_points))
		lines.append("#y_start")
		lines.append(str(plot_token.y_start))
		lines.append("#y_stop")
		lines.append(str(plot_token.y_stop))
		lines.append("#y_points")
		lines.append(str(plot_token.y_points))
		lines.append("#time")
		lines.append(str(plot_token.time))
		lines.append("#Vexternal")
		lines.append(str(plot_token.Vexternal))
		lines.append("#ver")
		lines.append("1.0")
		lines.append("#end")

		inp_save_lines(save_name,lines)

def get_plot_file_info(output,file_name):
	found,lines=zip_get_data_file(file_name)

	if found==False:
		print "can't file file"
		return False

	for i in range(0, len(lines)):
		lines[i]=lines[i].rstrip()


	if len(lines)>1:
		if lines[0]=="#opvdm":
			for i in range(0, len(lines)):
				if (lines[i][0]!="#"):
					break
				else:
					command=lines[i].split(" ",1)
					if len(command)<2:
						command.append("")
					if (command[0]=="#x_mul"):
						output.x_mul=float(command[1])
					if (command[0]=="#y_mul"):
						output.y_mul=float(command[1])
					if (command[0]=="#x_label"):
						output.x_label=command[1]
					if (command[0]=="#y_label"):
						output.y_label=command[1]
					if (command[0]=="#x_units"):
						output.x_units=command[1]
					if (command[0]=="#y_units"):
						output.y_units=command[1]
					if (command[0]=="#logscale_x"):
						output.logx=bool(int(command[1]))
					if (command[0]=="#logscale_y"):
						output.logy=bool(int(command[1]))
					if (command[0]=="#type"):
						output.type=command[1]
					if (command[0]=="#title"):
						output.title=command[1]
					if (command[0]=="#section_one"):
						output.section_one=command[1]
					if (command[0]=="#section_two"):
						output.section_two=command[1]
					if (command[0]=="#time"):
						output.time=float(command[1])
					if (command[0]=="#Vexternal"):
						output.Vexternal=float(command[1])

			return True

	return False
