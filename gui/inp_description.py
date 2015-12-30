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


import os

import i18n
_ = i18n.language.gettext


class descriptor():
	def __init__(self,file,des):
		self.file=file
		self.des=des

inp_files=[]
inp_files.append(descriptor("device.inp",_("Device")))
inp_files.append(descriptor("jv.inp",_("JV")))
inp_files.append(descriptor("jv_simple.inp",_("JV Simple")))
inp_files.append(descriptor("dump.inp",_("Dump")))
inp_files.append(descriptor("celiv.inp",_("CELIV")))
inp_files.append(descriptor("math.inp",_("Math")))
inp_files.append(descriptor("tof.inp",_("ToF")))
inp_files.append(descriptor("stark.inp",_("Stark")))
inp_files.append(descriptor("pulse.inp",_("Pulse")))
inp_files.append(descriptor("pulse_voc.inp",_("Pulse Voc")))
inp_files.append(descriptor("imps.inp",_("IMPS")))
inp_files.append(descriptor("light_exp.inp",_("Light Exp")))
inp_files.append(descriptor("thermal.inp",_("Thermal")))
inp_files.append(descriptor("sun_voc.inp",_("Sun Voc")))
inp_files.append(descriptor("tpc.inp",_("TPC")))
inp_files.append(descriptor("fit.inp",_("Fit")))
inp_files.append(descriptor("lumo0.inp",_("Bands")))
inp_files.append(descriptor("epitaxy.inp",_("Density of States")))
inp_files.append(descriptor("pl0.inp",_("Luminescence")))

def inp_file_to_description(file_name):
	global inp_files
	for f in inp_files:
		if f.file==file_name:
			return f.des
	return False


