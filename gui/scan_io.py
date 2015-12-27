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
import gc
import gtk
import sys
import os
import shutil
from util import opvdm_delete_file
from inp import inp_get_token_value
from scan_tree import tree_load_flat_list
from util import copy_scan_dir
from scan_tree import tree_load_program
from scan_tree import tree_gen
from scan_item import scan_item_load
from server import server_find_simulations_to_run

def scan_delete_files(dirs_to_del):
	for i in range(0, len(dirs_to_del)):
		opvdm_delete_file(dirs_to_del[i])

def scan_list_simulations(dir_to_search):
	found_dirs=[]
	for root, dirs, files in os.walk(dir_to_search):
		for name in files:
			full_name=os.path.join(root, name)
			if name=="sim.opvdm":
				found_dirs.append(root)
	return found_dirs

def scan_list_unconverged_simulations(dir_to_search):
	found_dirs=[]
	for root, dirs, files in os.walk(dir_to_search):
		for name in files:
			full_name=os.path.join(root, name)
			if name=="sim.opvdm":
				add=True
				fit_log=os.path.join(root,'fitlog.dat')
				if os.path.isfile(fit_log):
					f = open(fit_log, "r")
					lines = f.readlines()
					f.close()
				
					for l in range(0, len(lines)):
						lines[l]=lines[l].rstrip()

					if len(lines)>4:
						error=float(lines[len(lines)-2].split()[1])
						if error<0.1:
							print fit_log,error
							add=False

				if add==True:
					found_dirs.append(root)
	return found_dirs

def scan_ask_to_delete(dirs_to_del):
	if (len(dirs_to_del)!=0):

		settings = gtk.settings_get_default()
		settings.set_property('gtk-alternative-button-order', True)

		dialog = gtk.Dialog()
		cancel_button = dialog.add_button(gtk.STOCK_YES, gtk.RESPONSE_YES)

		ok_button = dialog.add_button(gtk.STOCK_NO, gtk.RESPONSE_NO)
		ok_button.grab_default()

		help_button = dialog.add_button(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL)

		text_del_dirs=""
		if len(dirs_to_del)>30:
			for i in range(0,30,1):
				text_del_dirs=text_del_dirs+dirs_to_del[i]+"\n"
			text_del_dirs=text_del_dirs+"and "+str(len(dirs_to_del)-30)+" more."
		else:
			for i in range(0,len(dirs_to_del)):
				text_del_dirs=text_del_dirs+dirs_to_del[i]+"\n"

		label = gtk.Label("Should I delete these files?:\n"+"\n"+text_del_dirs)
		dialog.vbox.pack_start(label, True, True, 0)
		label.show()

		dialog.set_alternative_button_order([gtk.RESPONSE_YES, gtk.RESPONSE_NO,
					       gtk.RESPONSE_CANCEL])

		#dialog = gtk.MessageDialog(None, 0, gtk.MESSAGE_QUESTION,  gtk.BUTTONS_YES_NO, str("Should I delete the old simualtions first?:\n"+"\n".join(dirs_to_del)))
		response = dialog.run()
		
		if response == gtk.RESPONSE_YES:
				scan_delete_files(dirs_to_del)
		elif response == gtk.RESPONSE_NO:
			print "Not deleting"
		elif response == gtk.RESPONSE_CANCEL:
			run=False
			print "Cancel"

		dialog.destroy()

def scan_clean_simulation_output(dir_to_clean):
		dirs_to_del=[]
		simulation_dirs=scan_list_simulations(dir_to_clean)

		clean_simulation(dir_to_clean,simulation_dirs)

def scan_nested_simulation(root_simulation,nest_simulation):
	names=tree_load_flat_list(root_simulation)
	commands=[]
	for i in range(0,len(names)):
		dest_name=os.path.join(names[i],os.path.basename(nest_simulation))
		copy_scan_dir(dest_name,nest_simulation)

		program_list=[]
		flat_list=[]
		scan_item_load(os.path.join(dest_name,"scan_items.inp"))
		tree_load_program(program_list,dest_name)
		print "call=",names[i],dest_name
		tree_gen(flat_list,program_list,names[i],dest_name)


		server_find_simulations_to_run(commands,dest_name)

	return commands

def clean_simulation(dir_to_clean,simulation_dirs):
	files_to_delete=[]
	sims_we_should_have=tree_load_flat_list(dir_to_clean)
	for i in range(0,len(simulation_dirs)):
		if sims_we_should_have.count(simulation_dirs[i])==0:
			files_to_delete.append(simulation_dirs[i])
		else:
			listing=os.listdir(simulation_dirs[i])

			for ii in range(0,len(listing)):
				delete=True
				path=os.path.join(simulation_dirs[i],listing[ii])
				if path.endswith(".inp"):
					delete=False

				if path.endswith("exp"):
					delete=False

				if path.endswith("materials"):
					delete=False

				if path.endswith(".opvdm"):
					delete=False

				elif path.endswith(".opvdm"):
					delete=False

				elif path.endswith("fitlog_time_error.dat"):
					delete=False

				elif path.endswith("fitlog_time_speed.dat"):
					delete=False

				elif path.endswith("fitlog_time_odes.dat"):
					delete=False

				elif path.endswith("fitlog.dat"):
					delete=False

				elif path.endswith("fitlog_speed.dat"):
					delete=False

				elif path.endswith("fiterror.dat"):
					delete=False

				if delete==True:
					print "delete",path		
					files_to_delete.append(path)

	scan_ask_to_delete(files_to_delete)

def scan_clean_dir(dir_to_clean):
		dirs_to_del=[]
		dirs_to_del=scan_list_simulations(dir_to_clean)

		print dirs_to_del,dir_to_clean

		scan_ask_to_delete(dirs_to_del)

def scan_clean_unconverged(dir_to_clean):
		dirs_to_del=[]
		dirs_to_del=scan_list_unconverged_simulations(dir_to_clean)

		print dirs_to_del,dir_to_clean

		scan_ask_to_delete(dirs_to_del)

def scan_push_to_hpc(base_dir,only_unconverged):
	config_file=os.path.join(os.getcwd(),"server.inp")
	print config_file
	hpc_path=inp_get_token_value(config_file, "#hpc_dir")
	hpc_path=os.path.abspath(hpc_path)

	if os.path.isdir(hpc_path)==True:
		hpc_files=[]
		hpc_files=scan_list_simulations(hpc_path)
		print "hpc files=",hpc_files
		scan_delete_files(hpc_files)
		files=[]

		if only_unconverged==True:
			files=scan_list_unconverged_simulations(base_dir)
		else:
			files=scan_list_simulations(base_dir)

		print "copy files=",files
		for i in range(0,len(files)):
			dest_path=os.path.join(hpc_path,files[i][len(base_dir)+1:])
			print dest_path
			shutil.copytree(files[i], dest_path,symlinks=True)
	else:
		print "HPC dir not found",hpc_path

def scan_import_from_hpc(base_dir):
	config_file=os.path.join(os.getcwd(),"server.inp")
	hpc_path=inp_get_token_value(config_file, "#hpc_dir")
	hpc_path=os.path.abspath(hpc_path)

	if os.path.isdir(hpc_path)==True:

		hpc_files=scan_list_simulations(hpc_path)

		for i in range(0,len(hpc_files)):
			if hpc_files[i].endswith("orig")==False:
				src_path=hpc_files[i]
				dest_path=os.path.join(base_dir,hpc_files[i][len(hpc_path)+1:])
				if os.path.isdir(dest_path):
					shutil.rmtree(dest_path)
				shutil.copytree(src_path, dest_path, symlinks=False, ignore=None)
				print src_path,dest_path
	else:
		print "HPC dir not found",hpc_path

def get_scan_dirs(scan_dirs,sim_dir):
	ls=os.listdir(sim_dir)

	for i in range(0, len(ls)):
		dir_name=os.path.join(sim_dir,ls[i])
		full_name=os.path.join(sim_dir,ls[i],"opvdm_gui_config.inp")
		if os.path.isfile(full_name):
			scan_dirs.append(dir_name)


def delete_scan_dirs(path):
	sim_dirs=[]
	get_scan_dirs(sim_dirs,path)

	for my_file in sim_dirs:
		print "Deleteing ",my_file
		shutil.rmtree(my_file)

