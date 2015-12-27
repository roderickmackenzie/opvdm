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

class plot_state():
	def __init__(self):
		self.grid=False
		self.show_pointer=False
		self.logy=False
		self.logx=False
		self.label_data=False
		self.invert_y=False
		self.normalize=False
		self.norm_to_peak_of_all_data=False
		self.subtract_first_point=False
		self.add_min=False
		self.legend_pos="lower right"
		self.ymax=-1
		self.ymin=-1
		self.x_label=""
		self.y_label=""
		self.x_units=""
		self.y_units=""
		self.x_mul=1.0
		self.y_mul=1.0
		self.key_units=""
		self.file0=""
		self.tag0=""
		self.file1=""
		self.tag1=""
		self.file2=""
		self.tag2=""
		self.example_file0=""
		self.example_file1=""
		self.example_file2=""
		self.time=0.0
		self.Vexternal=0.0
		self.file_name=""
		self.other_file=""
		self.title=""
		self.type="xy"
		self.section_one=""
		self.section_two=""

		self.x_start=0
		self.x_stop=1
		self.x_points=25
		self.y_start=0
		self.y_stop=1
		self.y_points=25


