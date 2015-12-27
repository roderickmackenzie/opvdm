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
from scan_item import scan_item

class tab_class(gtk.Table):
	
	lines=[]
	edit_list=[]
	file_name=""
	line_number=[]
	name=""
	visible=1
	def callback_edit(self, widget, data=None):
		line=self.line_number[data]
		self.lines[line]=self.edit_list[data].get_text()
		self.edit_list[data].set_text(self.lines[line])
		a = open(self.file_name, "w")
		for i in range(0,len(self.lines)):
			a.write(self.lines[i]+"\n")
		a.close()

	# This callback quits the program
	def delete_event(self, widget, event, data=None):
		gtk.main_quit()
		return False


	def wow(self,filename,fullname,check_list):
		self.file_name=filename
		self.edit_list=[]
		self.line_number=[]

		f = open(filename)
		self.lines = f.readlines()
		f.close()
		pos=0
		for i in range(0, len(self.lines)):
			self.lines[i]=self.lines[i].rstrip()

		n=0
		pos=0
		for i in range(0, len(self.lines)/2):

			show=False
			units="Units"
			out_text=self.lines[pos]
			if out_text == "#mueffe":
				units="m<sup>2</sup>V<sup>-1</sup>s<sup>-1</sup>"
				text_info="Electron mobility"
				show=True
			if out_text == "#mueffh":
				text_info="Hole mobility"
				units="m<sup>2</sup>V<sup>-1</sup>s<sup>-1</sup>"
				show=True
			if out_text == "#Ntrape":
				units="m<sup>-3</sup> eV<sup>-1</sup>"
				text_info="Electron trap density"
				show=True
			if out_text == "#Ntraph":
				units="m<sup>-3</sup> eV<sup>-1</sup>"
				text_info="Hole trap density"
				show=True
			if out_text == "#Etrape":
				units="eV"
				text_info="Electron tail slope"
				show=True
			if out_text == "#Etraph":
				units="eV"
				text_info="Hole tail slope"
				show=True
			if out_text == "#epsilonr":
				units="none"
				text_info="Relative permittivity"
				show=True
			if out_text == "#srhsigman_e":
				units="m<sup>-2</sup>"
				text_info="Free electron->Trapped electron"
				show=True
			if out_text == "#srhsigmap_e":
				units="m<sup>-2</sup>"
				text_info="Trapped electron->Free hole"
				show=True
			if out_text == "#srhsigman_h":
				units="m<sup>-2</sup>"
				text_info="Trapped hole->Free electron"
				show=True
			if out_text == "#srhsigmap_h":
				units="m<sup>-2</sup>"
				text_info="Free hole->Trapped hole"
				show=True
			if out_text == "#Rshunt":
				units="Ohms"
				text_info="Shunt resistance"
				show=True
			if out_text == "#Rcontact":
				units="Ohms"
				text_info="Series resistance"
				show=True
			if out_text == "#lcharge":
				units="m<sup>-3</sup>"
				text_info="Charge on left contact"
				show=True
			if out_text == "#rcharge":
				units="m<sup>-3</sup>"
				text_info="Charge on right contact"
				show=True
			if out_text == "#Vstart":
				units="V"
				text_info="Start voltage"
				show=True
			if out_text == "#Vstop":
				units="V"
				text_info="Stop voltage"
				show=True
			if out_text == "#Vstep":
				units="V"
				text_info="Voltage step"
				show=True
			if out_text == "#Psun":
				units="0/1000"
				text_info="Intensity of the sun"
				show=True
			if out_text == "#simplephotondensity":
				units="m<sup>-2</sup>s<sup>-1</sup>"
				text_info="Photon density"
				show=True
			if out_text == "#simple_alpha":
				units="m<sup>-1</sup>"
				text_info="Absorption of material"
				show=True
			if out_text == "#plot":
				text_info="Plot bands etc.. "
				units="1/0"
				show=True
			if out_text == "#celiv_laser_eff":
				text_info="#celiv_laser_eff"
				show=True
			if out_text == "#doping":
				units="m<sup>-3</sup>"
				text_info="#doping"
				show=True
			if out_text == "#simmode":
				units="au"
				text_info="#simmode"
				show=True
			if out_text == "#Nc":
				units="m<sup>-3</sup>"
				text_info="Effective density of free electron states"
				show=True
			if out_text == "#Nv":
				units="m<sup>-3</sup>"
				text_info="Effective density of free hole states"
				show=True
			if out_text == "#maxelectricalitt":
				units="au"
				text_info="Max electical itterations"
				show=True
			if out_text == "#electricalclamp":
				units="au"
				text_info="Electrical clamp"
				show=True
			if out_text == "#posclamp":
				units="au"
				text_info="Poisson clamping"
				show=True
			if out_text == "#electricalerror":
				units="au"
				text_info="Minimum electrical error"
				show=True
			if out_text == "#free_to_free_recombination":
				units="m<sup>3</sup>s<sup>-1</sup>"
				text_info="Free to free recombination"
				show=True
			if out_text == "#sun":
				units="filename"
				text_info="Sun's spectra"
				show=True
			if out_text == "#meshpoints":
				units="au"
				text_info="Mesh points (x)"
				show=True
			if out_text == "#lpoints":
				units="au"
				text_info="Mesh points (lambda)"
				show=True
			if out_text == "#lstart":
				units="m"
				text_info="Lambda start"
				show=True
			if out_text == "#lstop":
				units="m"
				text_info="Lambda stop"
				show=True
			if out_text == "#laserwavelength":
				units="m"
				text_info="Laser wavelength"
				show=True
			if out_text == "#spotx":
				units="m"
				text_info="#spotx"
				show=True
			if out_text == "#spoty":
				units="m"
				text_info="#spoty"
				show=True
			if out_text == "#pulseJ":
				units="J"
				text_info="#pulseJ"
				show=True
			if out_text == "#pulseL":
				units="s"
				text_info="#pulseL"
				show=True
			if out_text == "#gather":
				units="au"
				text_info="#gather"
				show=True
			if out_text == "#dlength":
				units="m"
				text_info="#dlength"
				show=True
			if out_text == "#electron_eff":
				units="0-1"
				text_info="#electron_eff"
				show=True
			if out_text == "#hole_eff":
				units="0-1"
				text_info="#hole_eff"
				show=True
			if out_text == "#function":
				units="au"
				text_info="#function"
				show=True
			if out_text == "#lr_pcontact":
				units="left/right"
				text_info="P contact on left or right"
				show=True
			if out_text == "#Vexternal":
				units="V"
				text_info="start voltage"
				show=True
			if out_text == "#Vmax":
				units="V"
				text_info="Max voltage"
				show=True
			if out_text == "#celiv_t0":
				units="s"
				text_info="#celiv_t0"
				show=True
			if out_text == "#celiv_t1":
				units="s"
				text_info="#celiv_t1"
				show=True
			if out_text == "#celiv_t2":
				units="s"
				text_info="#celiv_t2"
				show=True
			if out_text == "#Eg":
				units="eV"
				text_info="Eg"
				show=True
			if out_text == "#start_stop_time":
				units="s"
				text_info="Time of pause"
				show=True
			if out_text == "#stopstart":
				units="1/0"
				text_info="Pause between iterations"
				show=True
			if out_text == "#invert_current":
				units="true/false"
				text_info="Invert output"
				show=True
			if out_text == "#lr_bias":
				units="left/right"
				text_info="Bias left or right"
				show=True
			if out_text == "#otherlayers":
				units="m"
				text_info="Other layers"
				show=True
			if out_text == "#dump_slices":
				units="1/0"
				text_info="Dump slices"
				show=True
			if out_text == "#tofstop":
				units="s"
				text_info="ToF stop time"
				show=True
			if out_text == "#toflaseroff":
				units="s"
				text_info="ToF laser off time"
				show=True
			if out_text == "#dt":
				units="s"
				text_info="ToF time step"
				show=True
			if out_text == "#tofdtmull":
				units="au"
				text_info="ToF dt multiplier"
				show=True
			if out_text == "#toflaserpower":
				text_info="ToF laser power"
				show=True
			if out_text == "#start_steps":
				text_info="ToF steps before start"
				show=True
			if out_text == "#tof_voltage":
				units="V"
				text_info="ToF voltage"
				show=True
			if out_text == "#tof_Rshort":
				units="Ohms"
				text_info="Rshort"
				show=True
			if out_text == "#stark_startime":
				units="s"
				text_info="startime"
				show=True
			if out_text == "#stark_ea_factor":
				units="au"
				text_info="ea_factor"
				show=True
			if out_text == "#stark_Np":
				text_info="Np"
				show=True
			if out_text == "#stark_den":
				text_info="den"
				show=True
			if out_text == "#stark_externalv":
				units="V"
				text_info="externalv"
				show=True
			if out_text == "#stark_dt_neg_time":
				units="s"
				text_info="dt_neg_time"
				show=True
			if out_text == "#stark_dt":
				units="s"
				text_info="dt"
				show=True
			if out_text == "#stark_dt_mull":
				units="au"
				text_info="dt_mull"
				show=True
			if out_text == "#stark_stop":
				units="s"
				text_info="stop"
				show=True
			if out_text == "#stark_stark":
				text_info="stark"
				show=True
			if out_text == "#stark_lasereff":
				text_info="lasereff"
				show=True
			if out_text == "#stark_probe_wavelength":
				units="m"
				text_info="wavelength"
				show=True
			if out_text == "#stark_sim_contacts":
				text_info="sim_contacts"
				show=True
			if out_text == "#pulse_dt":
				units="s"
				text_info="dt"
				show=True
			if out_text == "#pulse_t0":
				units="s"
				text_info="time<sub>0</sub>"
				show=True
			if out_text == "#pulse_t1":
				units="s"
				text_info="time<sub>1</sub>"
				show=True
			if out_text == "#pulse_t2":
				units="s"
				text_info="time<sub>2</sub>"
				show=True
			if out_text == "#pulse_Vexternal":
				units="Volts"
				text_info="V<sub>external</sub>"
				show=True
			if out_text == "#pulse_repeat":
				units="number"
				text_info="Repeat"
				show=True
			if out_text == "#Rshort_pulse":
				units="Ohms"
				text_info="R<sub>short</sub>"
				show=True
			if out_text == "#pulse_laser_on":
				units="s"
				text_info="laser_on"
				show=True
			if out_text == "#pulse_laser_width":
				units="s"
				text_info="laser_width"
				show=True
			if out_text == "#pulse_laser_power":
				units="J"
				text_info="laser_power"
				show=True
			if out_text == "#pulse_light":
				units="1/0"
				text_info="pulse_light"
				show=True
			if out_text == "#pulse_sun":
				units="J"
				text_info="pulse_sun"
				show=True
			if out_text == "#pulse_shift":
				units="s"
				text_info="pulse_shift"
				show=True
			if out_text == "#pulse_bigmeshstart":
				units="s"
				text_info="Big mesh start"
				show=True
			if out_text == "#pulse_bigmeshstop":
				units="s"
				text_info="Big mesh stop"
				show=True
			if out_text == "#pulse_bigdt":
				units="s"
				text_info="Big mesh dt"
				show=True
			if out_text == "#photokit_points":
				units="s"
				text_info="points"
				show=True
			if out_text == "#photokit_n":
				units="s"
				text_info="Wavelengths to simulate"
				show=True
			if out_text == "#photokit_Vexternal":
				units="Volts"
				text_info="External voltage"
				show=True
			if out_text == "#Rshort_photokit":
				units="Ohms"
				text_info="R<sub>short</sub>"
				show=True
			if out_text == "#photokit_sun":
				units="1000=1 Sun"
				text_info="Backgroud light bias"
				show=True
			if out_text == "#photokit_modulation_max":
				units="1000=1 Sun"
				text_info="Modulation depth"
				show=True
			if out_text == "#photokit_modulation_fx":
				units="Hz"
				text_info="Modulation frequency"
				show=True





			pos=pos+1

			if show == True :

				label = gtk.Label()
				label.set_markup(text_info)
				label.set_use_markup(True)
				self.attach(label, 0, 1, n, n+1)
				label.show()

				self.line_number.append(pos)
				self.edit_list.append(gtk.Entry(max=0))
				self.edit_list[n].set_text(self.lines[pos]);
				self.attach(self.edit_list[n], 1, 2, n, n+1)
				self.edit_list[n].connect("changed", self.callback_edit, n)
				self.edit_list[n].show()

				label = gtk.Label()
				label.set_markup(units)
				label.set_use_markup(True)
				label.show()
				self.attach(label, 2, 3, n, n+1,gtk.SHRINK,gtk.SHRINK)
				self.set_col_spacing(3, 5)
				self.set_col_spacing(2, 5)
				label.show()

				check_list.append(scan_item())
				listpos=len(check_list)-1
				check_list[listpos].name=text_info
				check_list[listpos].filename=filename
				check_list[listpos].token=out_text
				check_list[listpos].line=pos
				
				n=n+1

			pos=pos+1

