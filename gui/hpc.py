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
import commands
from cal_path import find_data_file
from search import find_fit_log
from search import find_fit_speed_log
from window_list import windows
from inp import inp_load_file
from inp import inp_search_token_value

class hpc_class(gtk.Window):
	
	file_name=""
	name=""
	visible=1
	hpc_root_dir=""
	enabled=os.path.exists("./hpc.inp")
	cpus=[]

	def callback_node(self, widget, data=None):
		lines=[]
		a = open("../hpc/allowed_nodes", "w")

		if inp_load_file(lines,"./server.inp")==True:
			cpus_per_job=int(inp_search_token_value(lines, "#server_cpus"))
			print "CPUs per job=",cpus_per_job

		a.write(str(cpus_per_job)+"\n")

		for i in range(0, len(self.button)):
			print "cpus=",self.name[i]
			if self.button[i].get_active()==True:
				print "active=",self.name[i]
				a.write(self.name[i]+"\n")
				a.write(str(self.cpus[i])+"\n")
		a.close()

		now_dir=os.getcwd()

		os.chdir("../hpc")
		os.system("./make_node_list.py")

		os.chdir(now_dir)

	def callback_hpc_check_load(self, widget, data=None):
		curdir=os.getcwd()
		os.chdir(self.hpc_root_dir)
		cmd = './hpc_check_load.sh'
		data=os.popen(cmd).readlines()#os.system(cmd)
		self.name=[]
		

		f = open("./hpc/allowed_nodes")
		lines = f.readlines()
		f.close()

		for i in range(0, len(lines)):
			lines[i]=lines[i].rstrip()

		print "out=",data

		for i in range(0, len(data)):
			data[i]=data[i].rstrip()
			self.name.append(data[i].split(':')[0].strip())
			self.cpus.append(data[i].split(':')[1].strip())
			data[i]=data[i].split('load average: ')[1].split(',')

		if len(self.bar)==0:
			for i in range(0, len(data)):
				self.button.append(gtk.ToggleButton())
				a=gtk.HBox(False, 0)
				a.show()
				self.button[i].set_mode(True)
				self.button[i].show()
				self.button[i].set_size_request(-1, 40)
				self.bar.append(gtk.ProgressBar())
				self.bar[i].set_size_request(-1, 40)
				if lines.count(self.name[i])!=0:
					self.button[i].set_active(True)
				b = gtk.Label(str(self.name[i]))
				b.show()
				a.pack_start(b, False, False, 3)
				a.pack_start(self.bar[i], False, False, 3)
				self.button[i].add(a)
				self.bar[i].set_orientation(gtk.PROGRESS_LEFT_TO_RIGHT)
				self.bar[i].show()
				self.prog_hbox.pack_start(self.button[i], False, False, 0)

			for i in range(0, len(data)):
				self.button[i].connect("clicked", self.callback_node, None)
		
		for i in range(0, len(data)):
			self.bar[i].set_text(str(self.cpus[i])+"cpus, "+data[i][0]+" "+data[i][1]+" "+data[i][2])
			prog=float(data[i][0])/float(self.cpus[i])
			if prog>1.0:
				prog=1.0
			self.bar[i].set_fraction(prog)

		os.chdir(curdir)

	def callback_hpc_get_data(self, widget, data=None):
		curdir=os.getcwd()
		#os.chdir(self.hpc_root_dir)
		self.terminal.feed_child("cd "+self.hpc_root_dir+"\n")
		self.terminal.feed_child("./hpc_pull_data.sh\n")
		self.terminal.feed_child("cd "+curdir+"\n")

		#cmd = './from_hpc.sh'
		#os.system(cmd)
		#os.chdir(curdir)

	def callback_close_window(self, widget, event, data=None):
		self.win_list.update(self,"hpc_window")
		#gtk.main_quit()
		return False

	def callback_hpc_build_job_local(self, widget, data=None):
		curdir=os.getcwd()

		self.terminal.feed_child("cd "+self.hpc_root_dir+"\n")
		self.terminal.feed_child("./buildforhpc.sh scan\n")
		self.terminal.feed_child("cd "+curdir+"\n")
		
		print "Build job"

	def callback_hpc_send_to_hpc(self, widget, data=None):
		curdir=os.getcwd()
		self.terminal.feed_child("cd "+self.hpc_root_dir+"\n")
		self.terminal.feed_child("./to_hpc.sh\n")
		self.terminal.feed_child("cd "+curdir+"\n")

	def callback_hpc_build_jobs(self, widget, data=None):
		curdir=os.getcwd()
		self.terminal.feed_child("cd "+self.hpc_root_dir+"\n")
		self.terminal.feed_child("./hpc_make_sim.sh\n")
		self.terminal.feed_child("cd "+curdir+"\n")


	def callback_hpc_clean_nodes(self, widget, data=None):
		curdir=os.getcwd()
		self.terminal.feed_child("cd "+self.hpc_root_dir+"\n")
		self.terminal.feed_child("./hpc_remove_from_nodes.sh\n")
		self.terminal.feed_child("cd "+curdir+"\n")

	def callback_hpc_copy_to_nodes(self, widget, data=None):
		curdir=os.getcwd()
		self.terminal.feed_child("cd "+self.hpc_root_dir+"\n")
		self.terminal.feed_child("./hpc_copy_to_nodes.sh\n")
		self.terminal.feed_child("cd "+curdir+"\n")

	def callback_hpc_kill_jobs(self, widget, data=None):
		curdir=os.getcwd()
		self.terminal.feed_child("cd "+self.hpc_root_dir+"\n")
		self.terminal.feed_child("./hpc_kill_jobs.sh\n")
		self.terminal.feed_child("cd "+curdir+"\n")

	def callback_hpc_make_images(self, widget, data=None):
		curdir=os.getcwd()
		self.terminal.feed_child("cd "+self.hpc_root_dir+"\n")
		self.terminal.feed_child("./makeimages.sh\n")
		self.terminal.feed_child("cd "+curdir+"\n")

	def callback_hpc_view_images(self, widget, data=None):
		cmd = 'gnome-open '+os.getcwd()+'../'
		os.system(cmd)

	def callback_hpc_run_jobs(self, widget, data=None):
		curdir=os.getcwd()
		self.terminal.feed_child("cd "+self.hpc_root_dir+"\n")
		self.terminal.feed_child("./hpc_run_jobs.sh\n")
		self.terminal.feed_child("cd "+curdir+"\n")


	def callback_hpc_fitlog_plot(self, widget, data=None):
		plot_script_file=os.path.join(os.getcwd(),"plot","hpc_fitlog.plot")
		print plot_script_file
		find_fit_log(plot_script_file,"../hpc/")
		self.terminal.feed_child("gnuplot -persist "+plot_script_file+"\n")

	def callback_hpc_fit_speed_log_plot(self, widget, data=None):
		plot_script_file=os.path.join(os.getcwd(),"plot","hpc_fit_speed_log.plot")
		print plot_script_file
		find_fit_speed_log(plot_script_file,"../hpc/")
		self.terminal.feed_child("gnuplot -persist "+plot_script_file+"\n")

		#cmd = 'gnuplot -persist '+os.getcwd()+'plot/hpc_fitlog.plot'
		#os.system(cmd)


	def callback_hpc_run_once(self, widget, data=None):
		curdir=os.getcwd()
		self.terminal.feed_child("cd "+self.hpc_root_dir+"\n")
		self.terminal.feed_child("./autoonefit.sh\n")
		self.terminal.feed_child("cd "+curdir+"\n")


	def init(self, root_dir,terminal):
		print "ROOT=",root_dir
		self.win_list=windows()
		self.terminal=terminal
		main_box=gtk.HBox()
		self.hpc_root_dir=root_dir
		vbox_l=gtk.VBox(False, 2)
		vbox_l.show()
		main_box.pack_start(vbox_l, False, False, 0)
		vbox_r=gtk.VBox(False, 2)
		vbox_r.show()
		main_box.pack_start(vbox_r, False, False, 0)

		self.prog_hbox=gtk.VBox(False, 2)
		self.prog_hbox.show()
		main_box.pack_start(self.prog_hbox, True, True, 0)
		main_box.show_all()
		self.add(main_box)
		#check load
		button = gtk.Button("Check load")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_ZOOM_FIT, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_check_load)
		button.set_size_request(-1, 50)
		button.show()
		vbox_l.pack_start(button, False, False, 0)
		#vbox_l.add(button)
		

		button = gtk.Button("Get data")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_FLOPPY, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_get_data)
		button.set_size_request(-1, 50)
		button.show()
		vbox_l.pack_start(button, False, False, 0)

		button = gtk.Button("Fit log")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_FIND, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_fitlog_plot)
		button.set_size_request(-1, 50)
		button.show()
		vbox_l.pack_start(button, False, False, 0)

		button = gtk.Button("Fit speed")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_FIND, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_fit_speed_log_plot)
		button.set_size_request(-1, 50)
		button.show()
		vbox_l.pack_start(button, False, False, 0)

		button = gtk.Button("Run local sims")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_MEDIA_FORWARD, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_run_once)
		button.set_size_request(-1, 50)
		button.show()
		vbox_l.pack_start(button, False, False, 0)



		button = gtk.Button("Make images")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_CONVERT, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_make_images)
		button.set_size_request(-1, 50)
		button.show()
		vbox_l.pack_start(button, False, False, 0)

		button = gtk.Button("View images")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_COLOR_PICKER, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_view_images)
		button.set_size_request(-1, 50)
		button.show()
		vbox_l.pack_start(button, False, False, 0)



########

		button = gtk.Button("Build local job")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_EXECUTE, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_build_job_local)
		button.set_size_request(-1, 50)
		button.show()
		vbox_r.pack_start(button, False, False, 0)

		button = gtk.Button("Send to hpc")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_JUMP_TO, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_send_to_hpc)
		button.set_size_request(-1, 50)
		button.show()
		vbox_r.pack_start(button, False, False, 0)

		button = gtk.Button("Build jobs on HPC")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_EXECUTE, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_build_jobs)
		button.set_size_request(-1, 50)
		button.show()
		vbox_r.pack_start(button, False, False, 0)

		button = gtk.Button("Copy to nodes")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_NETWORK, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_copy_to_nodes)
		button.set_size_request(-1, 50)
		button.show()
		vbox_r.pack_start(button, False, False, 0)

		button = gtk.Button("Run jobs")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_MEDIA_FORWARD, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_run_jobs)
		button.set_size_request(-1, 50)
		button.show()
		vbox_r.pack_start(button, False, False, 0)

		button = gtk.Button("Clean nodes")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_CLEAR, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_clean_nodes)
		button.set_size_request(-1, 50)
		button.show()
		vbox_r.pack_start(button, False, False, 0)

		button = gtk.Button("Kill jobs")
		image = gtk.Image()
		image.set_from_stock(gtk.STOCK_MEDIA_STOP, gtk.ICON_SIZE_BUTTON)
		button.set_image(image)
		button.connect("clicked", self.callback_hpc_kill_jobs)
		button.set_size_request(-1, 50)
		button.show()
		vbox_r.pack_start(button, False, False, 0)
		self.win_list.set_window(self,"hpc_window")
		self.set_icon_from_file(find_data_file("gui/server.png"))
		self.set_size_request(700,-1)
		self.set_title("Organic Photovoltaic Device Model (www.opvdm.com)")
		self.connect("delete-event", self.callback_close_window) 
		self.bar=[]
		self.button=[]

