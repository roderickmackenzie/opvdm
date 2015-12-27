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
import shutil
import signal
import subprocess
from util import latex_to_pygtk_subscript

class my_data():
	token=""
	units=""
	info=""
	def __init__(self,a,b,c,d,e,f):
		self.token=a
		self.units=b
		self.info=c
		self.opt=d
		self.number_type=e
		self.number_mul=f

class tokens:
	lib=[]
	lib.append(my_data("#mueffe","m^{2}V^{-1}s^{-1}","Electron mobility",["text"],"e",1.0))
	lib.append(my_data("#mueffh","m^{2}V^{-1}s^{-1}","Hole mobility",["text"],"e",1.0))
	lib.append(my_data("#Ntrape","m^{-3} eV^{-1}","Electron trap density",["text"],"e",1.0))
	lib.append(my_data("#Ntraph","m^{-3} eV^{-1}","Hole trap density",["text"],"e",1.0))
	lib.append(my_data("#Etrape","eV","Electron tail slope",["text"],"e",1.0))
	lib.append(my_data("#Etraph","eV","Hole tail slope",["text"],"e",1.0))
	lib.append(my_data("#epsilonr","au","Relative permittivity",["text"],"e",1.0))
	lib.append(my_data("#srhsigman_e","m^{-2}","Free electron to Trapped electron",["text"],"e",1.0))
	lib.append(my_data("#srhsigmap_e","m^{-2}","Trapped electron to Free hole",["text"],"e",1.0))
	lib.append(my_data("#srhsigman_h","m^{-2}","Trapped hole to Free electron",["text"],"e",1.0))
	lib.append(my_data("#srhsigmap_h","m^{-2}","Free hole to Trapped hole",["text"],"e",1.0))
	lib.append(my_data("#Rshunt","Ohms","Shunt resistance",["text"],"e",1.0))
	lib.append(my_data("#Rcontact","Ohms","Series resistance",["text"],"e",1.0))
	lib.append(my_data("#lcharge","m^{-3}","Charge on left contact",["text"],"e",1.0))
	lib.append(my_data("#rcharge","m^{-3}","Charge on right contact",["text"],"e",1.0))
	lib.append(my_data("#Vstart","V","Start voltage",["text"],"e",1.0))
	lib.append(my_data("#Vstop","V","Stop voltage",["text"],"e",1.0))
	lib.append(my_data("#Vstep","V","Voltage step",["text"],"e",1.0))
	lib.append(my_data("#I0","Apms","I0",["text"],"e",1.0))
	lib.append(my_data("#nid","(a.u.)","ideality factor",["text"],"e",1.0))
	#lib.append(my_data("#Psun","Sun","Intensity of the sun",["text"],"e",1.0))

	lib.append(my_data("#saturation_n0","#saturation_n0","#saturation_n0",["text"],"e",1.0))
	lib.append(my_data("#saturation_rate","#saturation_rate","#saturation_rate",["text"],"e",1.0))
	lib.append(my_data("#imps_saturate","#imps_saturate","#imps_saturate",["text"],"e",1.0))


	lib.append(my_data("#simplephotondensity","m^{-2}s^{-1}","Photon density",["text"],"e",1.0))
	lib.append(my_data("#simple_alpha","m^{-1}","Absorption of material",["text"],"e",1.0))
	lib.append(my_data("#celiv_laser_eff","1/0","#celiv_laser_eff",["text"],"e",1.0))
	lib.append(my_data("#doping","m^{-3}","Doping",["text"],"e",1.0))
	lib.append(my_data("#simmode","au","#simmode",["text"],"e",1.0))
	lib.append(my_data("#Nc","m^{-3}","Effective density of free electron states",["text"],"e",1.0))
	lib.append(my_data("#Nv","m^{-3}","Effective density of free hole states",["text"],"e",1.0))
	lib.append(my_data("#maxelectricalitt","au","Max electrical itterations",["text"],"e",1.0))
	lib.append(my_data("#electricalclamp","au","Electrical clamp",["text"],"e",1.0))
	lib.append(my_data("#posclamp","au","Poisson clamping",["text"],"e",1.0))
	lib.append(my_data("#electricalerror","au","Minimum electrical error",["text"],"e",1.0))
	lib.append(my_data("#sun","filename","Sun's spectra",["text"],"e",1.0))
	lib.append(my_data("#meshpoints","au","Mesh points (x)",["text"],"e",1.0))
	lib.append(my_data("#lpoints","au","Mesh points (lambda)",["text"],"e",1.0))
	lib.append(my_data("#lstart","m","Lambda start",["text"],"e",1.0))
	lib.append(my_data("#lstop","m","Lambda stop",["text"],"e",1.0))
	lib.append(my_data("#laserwavelength","m","Laser wavelength",["text"],"e",1.0))
	lib.append(my_data("#spotx","m","Spot size x",["text"],"e",1.0))
	lib.append(my_data("#spoty","m","Spot size y",["text"],"e",1.0))
	lib.append(my_data("#pulseJ","J","Energy in pulse",["text"],"e",1.0))
	lib.append(my_data("#gather","au","#gather",["text"],"e",1.0))
	lib.append(my_data("#laser_pulse_width","s","#Length of pulse",["text"],"e",1.0))
	lib.append(my_data("#electron_eff","0-1","Electron generation efficiency",["text"],"e",1.0))
	lib.append(my_data("#hole_eff","0-1","Hole generation efficiency",["text"],"e",1.0))
	lib.append(my_data("#function","au","#function",["text"],"e",1.0))
	lib.append(my_data("#lr_pcontact","left/right","P contact on left or right",["left","right"],"s",1.0))
	lib.append(my_data("#Vexternal","V","start voltage",["text"],"e",1.0))
	lib.append(my_data("#Vmax","V","Max voltage",["text"],"e",1.0))
	lib.append(my_data("#celiv_t0","s","#celiv_t0",["text"],"e",1.0))
	lib.append(my_data("#celiv_t1","s","#celiv_t1",["text"],"e",1.0))
	lib.append(my_data("#celiv_t2","s","#celiv_t2",["text"],"e",1.0))
	lib.append(my_data("#Eg","eV","Eg",["text"],"e",1.0))
	lib.append(my_data("#Xi","eV","Xi",["text"],"e",1.0))
	lib.append(my_data("#start_stop_time","s","Time of pause",["text"],"e",1.0))
	lib.append(my_data("#stopstart","1/0","Pause between iterations",["1","0"],"e",1.0))
	lib.append(my_data("#invert_current","true/false","Invert output",["text"],"e",1.0))
	lib.append(my_data("#lr_bias","left/right","Bias left or right",["left","right"],"s",1.0))
	lib.append(my_data("#otherlayers","m","Other layers",["text"],"e",1.0))

	lib.append(my_data("#tofstop","s","ToF stop time",["text"],"e",1.0))
	lib.append(my_data("#tof_cap_escape_fx","s^{-1}","Capture to escape frequency",["text"],"e",1.0))
	lib.append(my_data("#toflaseroff","s","ToF laser off time",["text"],"e",1.0))
	lib.append(my_data("#dt","s","ToF time step",["text"],"e",1.0))
	lib.append(my_data("#tofdtmull","au","ToF dt multiplier",["text"],"e",1.0))
	lib.append(my_data("#toflaserpower","0-1","ToF laser power",["text"],"e",1.0))
	lib.append(my_data("#start_steps","1/0","ToF steps before start",["text"],"e",1.0))
	lib.append(my_data("#tof_voltage","V","ToF voltage",["text"],"e",1.0))
	lib.append(my_data("#tof_Rshort","Ohms","Rshort",["text"],"e",1.0))
	lib.append(my_data("#use_capacitor","1/0","Use capacitor",["1","0"],"e",1.0))
	lib.append(my_data("#stark_startime","s","startime",["text"],"e",1.0))
	lib.append(my_data("#stark_ea_factor","au","ea_factor",["text"],"e",1.0))
	lib.append(my_data("#stark_Np","1/0","Np",["text"],"e",1.0))
	lib.append(my_data("#stark_den","1/0","den",["text"],"e",1.0))
	lib.append(my_data("#stark_externalv","V","externalv",["text"],"e",1.0))
	lib.append(my_data("#stark_dt_neg_time","s","dt_neg_time",["text"],"e",1.0))
	lib.append(my_data("#stark_dt","s","dt",["text"],"e",1.0))
	lib.append(my_data("#stark_dt_mull","au","dt_mull",["text"],"e",1.0))
	lib.append(my_data("#stark_stop","s","stop",["text"],"e",1.0))
	lib.append(my_data("#stark_stark","1/0","stark",["text"],"e",1.0))
	lib.append(my_data("#stark_lasereff","1/0","lasereff",["text"],"e",1.0))
	lib.append(my_data("#stark_probe_wavelength","nm","wavelength",["text"],"e",1e9))
	lib.append(my_data("#stark_sim_contacts","1/0","sim_contacts",["text"],"e",1.0))
	lib.append(my_data("#pulse_dt","s","dt",["text"],"e",1.0))
	lib.append(my_data("#pulse_t0","s","time_{0}",["text"],"e",1.0))
	lib.append(my_data("#pulse_t1","s","time_{1}",["text"],"e",1.0))
	lib.append(my_data("#pulse_t2","s","time_{2}",["text"],"e",1.0))
	lib.append(my_data("#Rshort_pulse","Ohms","R_{short}",["text"],"e",1.0))
	lib.append(my_data("#imps_points","s","points",["text"],"e",1.0))
	lib.append(my_data("#imps_n","s","Wavelengths to simulate",["text"],"e",1.0))
	lib.append(my_data("#imps_Vexternal","Volts","External voltage",["text"],"e",1.0))
	lib.append(my_data("#Rshort_imps","Ohms","R_{short}",["text"],"e",1.0))
	lib.append(my_data("#imps_sun","1=1 Sun","Backgroud light bias",["text"],"e",1.0))
	lib.append(my_data("#imps_modulation_max","1=1 Sun","Modulation depth",["text"],"e",1.0))
	lib.append(my_data("#imps_modulation_fx","Hz","Modulation frequency",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_laser_eff","0-1.0","Laser Efficiency",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_light_bias","0-1000","Light bias",["text"],"e",1.0))
	lib.append(my_data("#high_sun_scale","au","High light multiplyer",["text"],"e",1.0))
	lib.append(my_data("#Dphotoneff","0-1","Photon efficiency",["text"],"e",1.0))
	lib.append(my_data("#jv_step_mul","0-2.0","JV voltage step multiplyer",["text"],"e",1.0))
	lib.append(my_data("#jv_max_j","A m^{-2}","Maximum current density",["text"],"e",1.0))

	lib.append(my_data("#voc","V","V_{oc}",["text"],"e",1.0))

	lib.append(my_data("#imps_r","Amps","Re(i)",["text"],"e",1.0))
	lib.append(my_data("#imps_i","Amps","Im(i)",["text"],"e",1.0))
	lib.append(my_data("#imps_Jr","Amps $m^{-2}$","Re(J)",["text"],"e",1.0))
	lib.append(my_data("#imps_Ji","Amps $m^{-2}$","Im(J)",["text"],"e",1.0))

	lib.append(my_data("#imps_fx","Hz","Frequency",["text"],"e",1.0))
	lib.append(my_data("#imps_delta_i","s","Phase shift",["text"],"e",1.0))
	lib.append(my_data("#imps_delta_g","s","Phase shift",["text"],"e",1.0))
	lib.append(my_data("#imps_delta_phase","s","Phase shift",["text"],"e",1.0))

	lib.append(my_data("#Cext","C","External C",["text"],"e",1.0))
	lib.append(my_data("#Rext","Ohms","External R",["text"],"e",1.0))

	lib.append(my_data("#Rscope","Ohms","Resistance of scope",["text"],"e",1.0))
	lib.append(my_data("#srh_bands","bands","Number of traps",["text"],"s",1.0))

	lib.append(my_data("#dostype","au","DoS distribution",["exponential","complex"],"s",1.0))

	lib.append(my_data("#sun_voc_single_point","1/0","Single point",["1","0"],"e",1.0))
	lib.append(my_data("#sun_voc_Psun_start","Suns","Start intensity",["text"],"e",1.0))
	lib.append(my_data("#sun_voc_Psun_stop","Suns","Stop intensity",["text"],"e",1.0))
	lib.append(my_data("#sun_voc_Psun_mul","au","step multiplier",["text"],"e",1.0))


	lib.append(my_data("#simplephotondensity","m^{-2}s^{-1}","Photon Flux",["text"],"e",1.0))
	lib.append(my_data("#simple_alpha","m^{-1}","Absorption",["text"],"e",1.0))
	lib.append(my_data("#xlen","m","device width",["text"],"e",1.0))
	lib.append(my_data("#zlen","m","device breadth",["text"],"e",1.0))
	lib.append(my_data("#ver","","",["text"],"e",1.0))
	lib.append(my_data("#dostype","","",["text"],"e",1.0))
	lib.append(my_data("#me","","",["text"],"e",1.0))
	lib.append(my_data("#mh","","",["text"],"e",1.0))
	lib.append(my_data("#gendos","","",["text"],"e",1.0))
	lib.append(my_data("#notused","","",["text"],"e",1.0))
	lib.append(my_data("#notused","","",["text"],"e",1.0))
	lib.append(my_data("#Tstart","","",["text"],"e",1.0))
	lib.append(my_data("#Tstop","","",["text"],"e",1.0))
	lib.append(my_data("#Tpoints","","",["text"],"e",1.0))
	lib.append(my_data("#nstart","","",["text"],"e",1.0))
	lib.append(my_data("#nstop","","",["text"],"e",1.0))
	lib.append(my_data("#npoints","","",["text"],"e",1.0))
	lib.append(my_data("#nstart","","",["text"],"e",1.0))
	lib.append(my_data("#nstop","","",["text"],"e",1.0))
	lib.append(my_data("#npoints","","",["text"],"e",1.0))
	lib.append(my_data("#srhbands","","",["text"],"e",1.0))
	lib.append(my_data("#srh_start","","",["text"],"e",1.0))
	lib.append(my_data("#srhvth_e","","",["text"],"e",1.0))
	lib.append(my_data("#srhvth_h","","",["text"],"e",1.0))
	lib.append(my_data("#srh_cut","","",["text"],"e",1.0))
	lib.append(my_data("#lumodelstart","","",["text"],"e",1.0))
	lib.append(my_data("#lumodelstop","","",["text"],"e",1.0))
	lib.append(my_data("#homodelstart","","",["text"],"e",1.0))
	lib.append(my_data("#homodelstop","","",["text"],"e",1.0))
	lib.append(my_data("#gaus_mull","","",["text"],"e",1.0))
	lib.append(my_data("#Esteps","","",["text"],"e",1.0))

	lib.append(my_data("#pulse_voc_dt","s","dt",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_dt_mul","au","dt multiplier",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_time_start","s","Start time",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_stop","s","Stop time",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_laser_pulse_width","s","Laser pulse width",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_Vexternal","V","V_{external}",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_repeat","","",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_shift","","",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_use_big_mesh","1/0","Use a big mesh",["1","0"],"e",1.0))
	lib.append(my_data("#pulse_voc_bigmeshstart","s","Start of big mesh",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_bigmeshstop","s","Stop of big mesh",["text"],"e",1.0))
	lib.append(my_data("#pulse_voc_bigdt","s","dt for big mesh",["text"],"e",1.0))

	lib.append(my_data("#server_cpus","","",["text"],"e",1.0))
	lib.append(my_data("#server_stall_time","","",["text"],"e",1.0))
	lib.append(my_data("#server_exit_on_dos_error","","",["text"],"e",1.0))
	lib.append(my_data("#server_max_run_time","","",["text"],"e",1.0))
	lib.append(my_data("#server_auto_cpus","","",["text"],"e",1.0))
	lib.append(my_data("#server_min_cpus","","",["text"],"e",1.0))
	lib.append(my_data("#server_steel","","",["text"],"e",1.0))

	lib.append(my_data("#invert_applied_bias","","",["text"],"e",1.0))
	lib.append(my_data("#Rshort","","",["text"],"e",1.0))
	lib.append(my_data("#Dphoton","","",["text"],"e",1.0))
	lib.append(my_data("#interfaceleft","","",["text"],"e",1.0))
	lib.append(my_data("#interfaceright","","",["text"],"e",1.0))
	lib.append(my_data("#phibleft","","",["text"],"e",1.0))
	lib.append(my_data("#phibright","","",["text"],"e",1.0))
	lib.append(my_data("#vl_e","","",["text"],"e",1.0))
	lib.append(my_data("#vl_h","","",["text"],"e",1.0))
	lib.append(my_data("#vr_e","","",["text"],"e",1.0))
	lib.append(my_data("#vr_h","","",["text"],"e",1.0))
	lib.append(my_data("#jv_light_efficiency","","",["text"],"e",1.0))
	lib.append(my_data("#light_model","","",["text"],"e",1.0))
	lib.append(my_data("#NDfilter","","",["text"],"e",1.0))
	lib.append(my_data("#plottime","","",["text"],"e",1.0))
	lib.append(my_data("#startstop","","",["text"],"e",1.0))
	lib.append(my_data("#dumpitdos","","",["text"],"e",1.0))
	lib.append(my_data("#plotfile","","",["text"],"e",1.0))


	lib.append(my_data("#celiv_dt","","",["text"],"e",1.0))
	lib.append(my_data("#celiv_time_j0","","",["text"],"e",1.0))
	lib.append(my_data("#celiv_time_j1","","",["text"],"e",1.0))
	lib.append(my_data("#Rshort","","",["text"],"e",1.0))
	lib.append(my_data("#celiv_laser_on","","",["text"],"e",1.0))
	lib.append(my_data("#celiv_laser_width","","",["text"],"e",1.0))
	lib.append(my_data("#celiv_enable_time_mesh","","",["text"],"e",1.0))
	lib.append(my_data("#solve_at_Vbi","","",["text"],"e",1.0))
	lib.append(my_data("#maxelectricalitt_first","au","Max Electrical itterations (first step)",["text"],"e",1.0))

	lib.append(my_data("#electricalclamp_first","au","Electrical clamp (first step)",["text"],"e",1.0))
	lib.append(my_data("#newton_clever_exit","","",["text"],"e",1.0))
	lib.append(my_data("#newton_min_itt","","",["text"],"e",1.0))
	lib.append(my_data("#remesh","","",["text"],"e",1.0))
	lib.append(my_data("#newmeshsize","","",["text"],"e",1.0))
	lib.append(my_data("#epitaxy","","",["text"],"e",1.0))
	lib.append(my_data("#alignmesh","","",["text"],"e",1.0))
	lib.append(my_data("#stark_start_time","","",["text"],"e",1.0))

	lib.append(my_data("#jv_pmax_n","m^{-3}","Average carrier density at P_{max}",["text"],"e",1.0))
	lib.append(my_data("#jv_pmax_tau","m^{-1}","Recombination time constant",["text"],"e",1.0))

	lib.append(my_data("#voc_nt","m^{-3}","Trapped electrons at Voc",["text"],"e",1.0))
	lib.append(my_data("#voc_pt","m^{-3}","Trapped holes at Voc",["text"],"e",1.0))
	lib.append(my_data("#voc_nf","m^{-3}","Free electrons at Voc",["text"],"e",1.0))
	lib.append(my_data("#voc_pf","m^{-3}","Free holes at Voc",["text"],"e",1.0))
	lib.append(my_data("#voc_np_tot","m^{-3}","Total carriers (n+p)/2 at Voc",["text"],"e",1.0))
	lib.append(my_data("#voc_tau","s}","Recombination time constant at Voc",["text"],"e",1.0))
	lib.append(my_data("#voc_R","m^{-3}s^{-1}","Recombination rate at Voc",["text"],"e",1.0))
	lib.append(my_data("#voc_J","A m^{-2}","Current density at Voc",["text"],"e",1.0))
	lib.append(my_data("#voc_J_to_Jr","au","Ratio of conduction current to recombination current",["text"],"e",1.0))

	lib.append(my_data("#voc_i","au","Current",["text"],"e",1.0))
	lib.append(my_data("#kl_in_newton","1/0","KCL in Newton solver",["1","0"],"e",1.0))

	lib.append(my_data("#tpc_time_start","s","Start time",["text"],"e",1.0))
	lib.append(my_data("#tpc_stop","s","Stop time",["text"],"e",1.0))
	lib.append(my_data("#tpc_laser_pulse_width","s","Laser pulse width",["text"],"e",1.0))
	lib.append(my_data("#tpc_dtstart","s","dt",["text"],"e",1.0))
	lib.append(my_data("#tpc_dtmull","au","dt multiplier",["text"],"e",1.0))
	lib.append(my_data("#tpc_laser_eff","au","Laser power",["text"],"e",1.0))
	lib.append(my_data("#tpc_short","s","Sense resistor",["text"],"e",1.0))
	lib.append(my_data("#tpc_tsteps","Number of iterations","au",["text"],"e",1.0))
	lib.append(my_data("#tpc_pulsestart","au","Steps before laser",["text"],"e",1.0))
	lib.append(my_data("#tpc_pulselen","s","Number of laser on steps",["text"],"e",1.0))
	lib.append(my_data("#tpc_reprate","au","Repeat",["text"],"e",1.0))
	lib.append(my_data("#tpc_timeshift","s","Time shift",["text"],"e",1.0))
	lib.append(my_data("#tpc_externalv","Volts","External voltage",["text"],"e",1.0))
	lib.append(my_data("#tpc_simall","1/0","Include RC effects",["1","0"],"e",1.0))
	lib.append(my_data("#tpc_L","Henry","Inductance",["text"],"e",1.0))

	lib.append(my_data("#do_fit","1/0","Do fit",["1","0"],"e",1.0))
	lib.append(my_data("#simplexmul","au","simplex mull",["text"],"e",1.0))
	lib.append(my_data("#reset","au","Reset steps",["text"],"e",1.0))


	lib.append(my_data("#max_nfree_to_ptrap","m^{-3}s^{-1}","nfree_to_ptrap",["text"],"e",1.0))
	lib.append(my_data("#max_pfree_to_ntrap","m^{-3}s^{-1}","max_pfree_to_ntrap",["text"],"e",1.0))
	lib.append(my_data("#max_nrelax","m^{-3}s^{-1}","max_nrelax",["text"],"e",1.0))
	lib.append(my_data("#max_prelax","m^{-3}s^{-1}","max_prelax",["text"],"e",1.0))

	lib.append(my_data("#max_nfree","m^{-3}","max_nfree",["text"],"e",1.0))
	lib.append(my_data("#max_pfree","m^{-3}","max_pfree",["text"],"e",1.0))
	lib.append(my_data("#max_ntrap","m^{-3}","max_ntrap",["text"],"e",1.0))
	lib.append(my_data("#max_ptrap","m^{-3}","max_ptrap",["text"],"e",1.0))
	lib.append(my_data("#alpha_max_reduction","m^{-1}","alpha_max_reduction",["text"],"e",1.0))
	lib.append(my_data("#alpha_max_increase","m^{-1}","alpha_max_increase",["text"],"e",1.0))

	lib.append(my_data("#srh_n_r1","m^{-3}s^{-1}","srh electron rate 1",["text"],"e",1.0))
	lib.append(my_data("#srh_n_r2","m^{-3}s^{-1}","srh electron rate 2",["text"],"e",1.0))
	lib.append(my_data("#srh_n_r3","m^{-3}s^{-1}","srh electron rate 3",["text"],"e",1.0))
	lib.append(my_data("#srh_n_r4","m^{-3}s^{-1}","srh electron rate 4",["text"],"e",1.0))

	lib.append(my_data("#srh_p_r1","m^{-3}s^{-1}","srh hole rate 1",["text"],"e",1.0))
	lib.append(my_data("#srh_p_r2","m^{-3}s^{-1}","srh hole rate 2",["text"],"e",1.0))
	lib.append(my_data("#srh_p_r3","m^{-3}s^{-1}","srh hole rate 3",["text"],"e",1.0))
	lib.append(my_data("#srh_p_r4","m^{-3}s^{-1}","srh hole rate 4",["text"],"e",1.0))

	lib.append(my_data("#band_bend_max","percent","band bend max",["text"],"e",1.0))

	lib.append(my_data("#Tll","Kelvin","Device temperature",["text"],"e",1.0))
	lib.append(my_data("#layer0","m","Active layer width",["text"],"e",1.0))
	lib.append(my_data("#stark_saturate","au","Stark saturate",["text"],"e",1.0))

	lib.append(my_data("#n_mul","au","n mul",["text"],"e",1.0))
	lib.append(my_data("#alpha_mul","m^{-1}","Alpha mul",["text"],"e",1.0))
	
	lib.append(my_data("#stark_point0","au","DR/R",["text"],"e",1.0))
	lib.append(my_data("#stark_point1","au","DR/R",["text"],"e",1.0))
	lib.append(my_data("#stark_point2","au","DR/R",["text"],"e",1.0))
	lib.append(my_data("#stark_point3","au","DR/R",["text"],"e",1.0))
	lib.append(my_data("#stark_point4","au","DR/R",["text"],"e",1.0))
	lib.append(my_data("#stark_subtracted_value","s","subtracted value",["text"],"e",1.0))
	lib.append(my_data("#light_energy","eV","Energy",["text"],"e",1.0))
	lib.append(my_data("#sim_id","au","sim id",["text"],"e",1.0))

	lib.append(my_data("#tpc_time_start","s","Start time",["text"],"e",1.0))
	lib.append(my_data("#tpc_stop","s","Stop time",["text"],"e",1.0))
	lib.append(my_data("#tpc_laser_pulse_width","s","Laser pulse width",["text"],"e",1.0))
	lib.append(my_data("#tpc_dtstart","s","dt start",["text"],"e",1.0))
	lib.append(my_data("#tpc_dtmull","au","dt multiplyer",["text"],"e",1.0))
	lib.append(my_data("#tpc_pulse_start","s","Start of tpc pulse",["text"],"e",1.0))
	lib.append(my_data("#tpc_laser_eff","au","Laser efficiency",["text"],"e",1.0))
	lib.append(my_data("#tpc_timeshift","s","Start of tpc pulse",["text"],"e",1.0))
	lib.append(my_data("#tpc_short","Ohms","TPC terminator resistance",["text"],"e",1.0))
	lib.append(my_data("#tpc_externalv","V","External voltage",["text"],"e",1.0))
	lib.append(my_data("#tpc_simall","au","Simulate RC components",["text"],["1","0"],1.0))

	lib.append(my_data("#pulse_dt","s","time step",["text"],"e",1.0))
	lib.append(my_data("#pulse_t0","s","Time segment 1",["text"],"e",1.0))
	lib.append(my_data("#pulse_t1","s","Time segment 2",["text"],"e",1.0))
	lib.append(my_data("#pulse_t2","s","Time segment 2",["text"],"e",1.0))
	lib.append(my_data("#pulse_Vexternal","V","Bias voltage",["text"],"e",1.0))
	lib.append(my_data("#Rshort_pulse","Ohms","External short circuit",["text"],"e",1.0))
	lib.append(my_data("#pulse_light","au","Apply an optical pulse (1)\n or an electrical pulse (0)",["1","0"],"e",1.0))
	lib.append(my_data("#pulse_sun","Suns","Intensity of the light pulse",["text"],"e",1.0))
	lib.append(my_data("#pulse_shift","s","Shift of TPC signal",["text"],"e",1.0))
	lib.append(my_data("#pulse_Vss","V","Magnitude of voltage pulse",["text"],"e",1.0))

	lib.append(my_data("#pulse_laser_on","s","Laser on time",["text"],"e",1.0))
	lib.append(my_data("#pulse_laser_width","s","Laser pulse width",["text"],"e",1.0))
	lib.append(my_data("#pulse_laser_power","Joules","Laser energy",["text"],"e",1.0))

	lib.append(my_data("#flip_field","au","Filp the opticl field",["text"],["1","0"],1.0))


	lib.append(my_data("#newton_dump","True/False","Dump from newton solver",["true","false"],"e",1.0))
	lib.append(my_data("#plot","True/False","Plot bands etc.. ",["true","false"],"e",1.0))
	lib.append(my_data("#dump_band_structure","","",["text"],"e",1.0))
	lib.append(my_data("#dump_slices_by_time","True/False","dump slices by time",["true","false"],"e",1.0))
	lib.append(my_data("#dump_1d_slices","True/False","Dump 1D slices",["true","false"],"e",1.0))
	lib.append(my_data("#dump_slices","True/False","Dump slices",["true","false"],"e",1.0))
	lib.append(my_data("#dump_dynamic","True/False","Dump dynamic",["true","false"],"e",1.0))
	lib.append(my_data("#dump_zip_files","True/False","Dump zip files",["true","false"],"e",1.0))
	lib.append(my_data("#dump_write_out_band_structure","True/False","Write out band structure",["true","false"],"e",1.0))
	lib.append(my_data("#dump_iodump","","",["text"],"e",1.0))
	lib.append(my_data("#dump_movie","","",["text"],"e",1.0))
	lib.append(my_data("#dump_optics","True/False","Dump optical information",["true","false"],"e",1.0))
	lib.append(my_data("#dump_optics_verbose","","",["text"],"e",1.0))
	lib.append(my_data("#dump_print_newtonerror","True/False","Print newton error",["true","false"],"e",1.0))
	lib.append(my_data("#dump_print_converge","True/False","Print solver convergence",["true","false"],"e",1.0))
	lib.append(my_data("#dump_write_converge","True/False","Write newton solver convergence to disk",["true","false"],"e",1.0))
	lib.append(my_data("#dump_print_pos_error","True/False","Print poisson solver convergence",["true","false"],"e",1.0))
	lib.append(my_data("#dump_pl","True/False","Dump PL spectra",["true","false"],"e",1.0))
	lib.append(my_data("#dump_norm_time_to_one","True/False","Normalize output x-time to one",["true","false"],"e",1.0))



	lib.append(my_data("#pl_enabled","True/False","Turn on luminescence",["true","false"],"e",1.0))
	lib.append(my_data("#pl_fe_fh","0.0-1.0","n_{frae} to n_{frae} photon generation efficiency",["text"],"e",1.0))
	lib.append(my_data("#pl_fe_te","0.0-1.0","n_{frae} to n_{trap} photon generation efficiency",["text"],"e",1.0))
	lib.append(my_data("#pl_te_fh","0.0-1.0","n_{trap} to p_{free} photon generation efficiency",["text"],"e",1.0))
	lib.append(my_data("#pl_th_fe","0.0-1.0","n_{trap} to p_{free} photon generation efficiency",["text"],"e",1.0))
	lib.append(my_data("#pl_fh_th","0.0-1.0","p_{free} to p_{free} photon generation efficiency",["text"],"e",1.0))

	lib.append(my_data("#free_to_free_recombination","0.0-1.0","n_{free} to p_{free} Recombination rate constant",["text"],"e",1.0))

	def find(self,token):
		for i in range(0, len(self.lib)):
			if self.lib[i].token==token.strip():
				if self.lib[i].units=="" and self.lib[i].info=="":
					return False
				else:
					return self.lib[i]

		#sys.stdout.write("Add -> lib.append(my_data(\""+token+"\",\"\",\"\",[\"text\"]))\n")
		return False
