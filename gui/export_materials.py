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

import sys
import os
import glob
import zipfile
from util_zip import archive_add_file
from progress import progress_class
from gui_util import process_events
from cal_path import remove_cwdfrompath

def export_materials(target):
	if target.endswith(".zip")==False:
		target=target+".zip"

	file_list=[]

	progress_window=progress_class()
	progress_window.init()
	progress_window.show()
	progress_window.start()
	process_events()
	               
	for path, dirs, files in os.walk(os.path.join(os.getcwd(),"materials")):	
		for file_name in files:
			if file_name=="alpha.omat" or file_name=="fit.inp" or file_name=="mat.inp" or file_name=="n.omat":
				file_list.append(os.path.join(path,file_name))

	zf = zipfile.ZipFile(target, 'a')

	for i in range(0,len(file_list)):
		cur_file=file_list[i]

		lines=[]
		if os.path.isfile(cur_file):
			f=open(cur_file, mode='rb')
			lines = f.read()
			f.close()


			zf.writestr(remove_cwdfrompath(cur_file), lines)
			progress_window.set_fraction(float(i)/float(len(file_list)))
			progress_window.set_text("Adding"+cur_file[len(os.getcwd()):])
			process_events()

	zf.close()
	progress_window.stop()

