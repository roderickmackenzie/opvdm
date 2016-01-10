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
from win_lin import running_on_linux

if running_on_linux()==False:
	import _winreg

materials_path=None
light_dll_path=None
exe_command=None
share_path=None
device_lib_path=None
bin_path=None
lib_path=None


def remove_cwdfrompath(path):
	tmp=path
	if tmp.startswith(os.getcwd()):
		tmp=tmp[len(os.getcwd())+1:]
	return tmp

def join_path(one,two):
	output_file=os.path.join(one,two)

	if two[0]=='/':
		if one!="" :
			output_file=os.path.join(one,two[1:])

	return output_file


def cal_share_path():
	global share_path
	if running_on_linux()==True:
		if os.path.isfile("main.c")==True:
			share_path=os.getcwd()
		else:
			share_path="/usr/share/opvdm/"
	else:
		try:
			registry_key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, "Software\\opvdm", 0, _winreg.KEY_READ)
			value, regtype = _winreg.QueryValueEx(registry_key, "installpath")
			_winreg.CloseKey(registry_key)
			print "Install path at", value
			share_path=value
		except WindowsError:
			print "No registry key found using default"
			share_path="c:\\opvdm"

def cal_lib_path():
	global lib_path
	if running_on_linux()==True:
		if os.path.isfile("main.c")==True:
			lib_path=os.getcwd()
		else:
			lib_path="/usr/lib64/opvdm/"
	else:
		try:
			registry_key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, "Software\\opvdm", 0, _winreg.KEY_READ)
			value, regtype = _winreg.QueryValueEx(registry_key, "installpath")
			_winreg.CloseKey(registry_key)
			print "Lib path at", value
			lib_path=value
		except WindowsError:
			print "No registry key found using default"
			lib_path="c:\\opvdm"

def cal_bin_path():
	global bin_path
	if running_on_linux()==True:
			bin_path="/bin/"
	else:
		try:
			registry_key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, "Software\\opvdm", 0, _winreg.KEY_READ)
			value, regtype = _winreg.QueryValueEx(registry_key, "installpath")
			_winreg.CloseKey(registry_key)
			print "Lib path at", value
			bin_path=value
		except WindowsError:
			print "No registry key found using default"
			bin_path="c:\\opvdm"

def calculate_paths():
	global share_path
	global lib_path
	global materials_path
	global exe_command
	global device_lib_path
	global light_dll_path

	cal_share_path()
	cal_lib_path()
	cal_bin_path()

	device_lib_path=os.path.join(share_path,"device_lib")
	materials_path=os.path.join(share_path,"materials")

	if running_on_linux() == True:
		if os.path.isfile("./main.c")==True:
			exe_command=os.path.join(os.getcwd(), "go.o")
		else:
			exe_command="opvdm_core"

	else:
		if os.path.isfile("opvdm_core.exe")==True:
			exe_command=os.path.join(os.getcwd(), "opvdm_core.exe")
		else:
			exe_command=os.path.join(share_path,"opvdm_core.exe")


	light_dll_path=os.path.join(lib_path,"light")


def get_share_path():
	global share_path
	return share_path

def get_materials_path():
	global materials_path
	return materials_path

def get_device_lib_path():
	global device_lib_path
	return device_lib_path

def get_bin_path():
	global bin_path
	return bin_path

def get_light_dll_path():
	global light_dll_path
	return light_dll_path

def get_exe_command():
	global exe_command
	return exe_command

def get_exe_name():
	if running_on_linux() == True:
		if os.path.isfile("./go.o")==True:
			exe_name="go.o"
		elif os.path.isfile("./main.c")==True:
			exe_name="go.o"
		else:
			exe_name="opvdm_core"
		return exe_name
	else:
		exe_name="opvdm_core.exe"
		return exe_name

def get_inp_file_path():
	global share_path
	return share_path

def get_image_file_path():
	global share_path
	return os.path.join(share_path,"images")

def get_lang_path():
	global share_path
	return os.path.join(share_path,"lang")

def find_data_file(name):
	global share_path
	return os.path.join(share_path,name)

