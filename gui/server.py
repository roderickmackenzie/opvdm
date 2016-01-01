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
from search import return_file_list
from util import str2bool
from inp import inp_get_token_value
import threading
import gobject
import multiprocessing
import time
import glob
import socket
from time import sleep
from win_lin import running_on_linux
import subprocess
from util import gui_print_path
from progress import progress_class
from copying import copying
from cal_path import get_exe_command
from global_objects import global_object_get
from help import my_help_class
import i18n
_ = i18n.language.gettext

def server_find_simulations_to_run(commands,search_path):
	for root, dirs, files in os.walk(search_path):
		for my_file in files:
			if my_file.endswith("sim.opvdm")==True:
				full_name=os.path.join(root, my_file)
				commands.append(root)

class server:
	def __init__(self):
		self.running=False
		self.thread_data=[""]
		self.enable_gui=False

		self.statusicon = gtk.StatusIcon()
		self.statusicon.set_from_stock(gtk.STOCK_YES) 
		#self.statusicon.connect("popup-menu", self.right_click_event)
		self.statusicon.set_tooltip("opvdm")


	def init(self,sim_dir):
		self.terminate_on_finish=False
		self.mylock=False
		self.cpus=multiprocessing.cpu_count()
		self.jobs=[]
		self.status=[]
		self.jobs_running=0
		self.jobs_run=0
		self.sim_dir=sim_dir
		self.cluster=str2bool(inp_get_token_value("server.inp","#cluster"))
		self.server_ip=inp_get_token_value("server.inp","#server_ip");
		self.error_messages=[]
		self.finished_jobs=[]

	def start_threads(self):
		if self.cluster==True:
			if self.running==False:
				try:
					self.tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

					# Open TPC port to server
					server_address = (self.server_ip, 10002)
					print >>sys.stderr, 'connecting to %s port %s' % server_address
					self.tcp_sock.connect(server_address)

					self.tcp_sock.sendall("open_tpc")
				except:
					return -1

				self.mylock=False
				self.thread = threading.Thread(target = self.listen)
				self.thread.daemon = True
				self.thread.start()

		return 0

	def set_terminal(self,terminal):
		self.terminal=terminal

	def gui_sim_start(self):
		self.progress_window.start()
		self.statusicon.set_from_stock(gtk.STOCK_NO)
		self.extern_gui_sim_start()

	def gui_sim_stop(self):
		self.progress_window.stop()
		self.statusicon.set_from_stock(gtk.STOCK_YES)
		self.extern_gui_sim_stop("Finished simulation")
		my_help_class.help_set_help(["plot.png",_("<big><b>Simulation finished!</b></big>\nClick on the plot icon to plot the results")])
		if len(self.error_messages)!=0:
			text='\n'.join(self.error_messages)
			if (text.count('License')==0):
				message = gtk.MessageDialog(type=gtk.MESSAGE_ERROR, buttons=gtk.BUTTONS_OK)
				message.set_markup(text)
				message.run()
				message.destroy()
			else:
				c=copying()
				c.wow(get_exe_command())


	def setup_gui(self,extern_gui_sim_start,extern_gui_sim_stop):
		self.enable_gui=True
		self.extern_gui_sim_start=extern_gui_sim_start
		self.extern_gui_sim_stop=extern_gui_sim_stop
		self.progress_window=progress_class()
		self.progress_window.init()


	def add_job(self,command):
		if self.cluster==False:
			self.jobs.append(command)
			self.status.append(0)
		else:
			s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			port = 8888;
			s.sendto("addjobs#"+command, (self.server_ip, port))
			s.close()

	def wake_nodes(self):
		if self.cluster==True:
			s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			port = 8888;
			s.sendto("wake_nodes", (self.server_ip, port))
			s.close()

	def clear_cache(self):
		if self.cluster==True:
			if self.running==True:
				print "Clear cache on server"
				self.mylock=True
				s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
				port = 8888;
				s.sendto("clear_cache#", (self.server_ip, port))
				s.close()
				print "I should be waiting for mylock"
				self.wait_lock()
				print "I have finished waiting"


	def start(self):
		self.finished_jobs=[]
		self.error_messages=[]
		if self.enable_gui==True:
			self.progress_window.show()
			self.gui_sim_start()
		#self.statusicon.set_from_stock(gtk.STOCK_NO) 
		self.running=True
		self.run_jobs()



	def listen(self):
		print "thread !!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		self.running=True
		try:
			self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		except socket.error:
			print 'Failed to create socket'
			sys.exit()

		self.socket.bind(('', 8889))

		while(1):
			d = self.socket.recvfrom(1024)
			data = d[0]
			addr = d[1]
			print data
			print "command=",data
			if data.startswith("percent"):
				command=data.split("#")
				percent=float(command[1])
				self.progress_window.set_fraction(percent/100.0)

			if data.startswith("job_finished"):
				command=data.split("#")
				self.label.set_text(gui_print_path("Finished:  ",command[1],60))
			if data.startswith("finished"):
				self.stop()
				sys.exit()
			if data.startswith("ok"):
				self.mylock=False
			if data.startswith("node_error"):
				command=data.split("#")
				self.error_messages.append("Node error!!! "+command[1])
				#self.mylock.set()
			if data.startswith("load"):
				if data.count("#")>1:
					command=data.split("#")
					items=len(command)
					height=50
					for i in range(1,len(command)):
						print command[i]
						height=height+15
						percent=float(command[i])
						if self.progress_window.progress_array[i].get_property("visible")==False:
							self.progress_window.progress_array[i].show()
						self.progress_window.progress_array[i].set_fraction(percent)

					window_height=self.progress_window.get_size()
					window_height=window_height[1]
					if window_height!=height:
						self.progress_window.set_size_request(400, height)
			if data.startswith("server_quit"):
				self.stop()
				print "Server quit!"

	def set_wait_bit(self):
		self.opp_finished=False

	
	def print_jobs(self):
		for i in range(0, len(self.jobs)):
			print self.jobs[i],self.status[i]

	def killall(self):
		if self.cluster==True:
			s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			port = 8888;
			s.sendto("killall", (self.server_ip, port))
			s.close()
		else:
			exe_name=os.path.basename(get_exe_command())
			cmd = 'killall '+exe_name
			ret= os.system(cmd)

		self.stop()

	def print_jobs(self):
		if self.cluster==True:
			s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			port = 8888;
			s.sendto("print_jobs", (self.server_ip, port))
			s.close()


	def sleep(self):
		if self.cluster==True:
			s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			port = 8888;
			s.sendto("sleep", (self.server_ip, port))
			s.close()	

	def poweroff(self):
		if self.cluster==True:
			s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			port = 8888;
			s.sendto("poweroff", (self.server_ip, port))
			s.close()	

	def get_data(self):
		if self.cluster==True:
			s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			port = 8888;
			s.sendto("get_data", (self.server_ip, port))
			s.close()	


	def wait_lock(self):
		print "Waiting for cluster..."
		while(self.mylock==True):
			sleep(0.1)


	def run_jobs(self):
		if self.cluster==True:
			s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			port = 8888;
			self.mylock=True
			s.sendto("set_master_ip", (self.server_ip, port))
			self.wait_lock()

			self.mylock=True
			s.sendto("set_exe#"+get_exe_command(), (self.server_ip, port))
			self.wait_lock()

			#self.mylock.clear()
			s.sendto("run", (self.server_ip, port))
			#self.mylock.wait(1000)

			s.close()
		else:
			if (len(self.jobs)==0):
				return
			for i in range(0, len(self.jobs)):
				if (self.jobs_running<self.cpus):
					if self.status[i]==0:
						self.status[i]=1
						print "Running job",self.jobs[i]
						if self.enable_gui==True:
							self.progress_window.set_text("Running job"+self.jobs[i])

						self.jobs_running=self.jobs_running+1
						if running_on_linux()==True:
							cmd="cd "+self.jobs[i]+";"
							cmd=cmd+get_exe_command()+" --lock "+"lock"+str(i)+" &\n"							
							print "command="+cmd
							if self.enable_gui==True:
								self.terminal.feed_child(cmd)
							else:
								print cmd
								os.system(cmd)
							
						else:
							cmd=get_exe_command()+" --lock "+"lock"+str(i)+" &\n"
							print cmd,self.jobs[i]
							subprocess.Popen(cmd,cwd=self.jobs[i])
							#os.system(cmd)

							#sys.exit()

	def stop(self):
		if self.cluster==True:
			self.socket.close()
			self.tcp_sock.close()


		self.jobs=[]
		self.status=[]
		self.jobs_running=0
		self.jobs_run=0
		self.gui_sim_stop()
		self.progress_window.set_fraction(0.0) 
		self.running=False

	
		print "I have shut down the server."


	def simple_run(self):
		ls=os.listdir(self.sim_dir)
		for i in range(0, len(ls)):
			if ls[i][:4]=="lock" and ls[i][-4:]==".dat":
				del_file=os.path.join(self.sim_dir,ls[i])
				print "delete file:",del_file
				os.remove(del_file)
		self.run_jobs()

		while(1):
			ls=os.listdir(self.sim_dir)
			for i in range(0, len(ls)):
				if ls[i][:4]=="lock" and ls[i][-4:]==".dat":
					lock_file=ls[i]
					os.remove(os.path.join(self.sim_dir,lock_file))
					self.jobs_run=self.jobs_run+1
					self.jobs_running=self.jobs_running-1
			self.run_jobs()
			time.sleep(0.1)

			if self.jobs_run==len(self.jobs):
				break

	def callback_dbus(self,data_in):
		if data_in.startswith("hex"):
			data_in=data_in[3:]
			data=data_in.decode("hex")
			#print "dbus:",data
			if data.startswith("lock"):
				if str(data)>4:
					test=data[:4]
					if test=="lock":
						if self.finished_jobs.count(data)==0:
							self.finished_jobs.append(data)

							rest=data[4:]
							self.jobs_run=self.jobs_run+1
							self.jobs_running=self.jobs_running-1
							self.progress_window.set_fraction(float(self.jobs_run)/float(len(self.jobs)))
							self.run_jobs()
							if (self.jobs_run==len(self.jobs)):
								self.stop()

			elif (data=="pulse"):
				if len(self.jobs)==1:
					splitup=data.split(":")
					if len(splitup)>1:
						text=data.split(":")[1]
						self.progress_window.set_text(text)
					self.progress_window.progress.set_pulse_step(0.01)
					self.progress_window.pulse()
			elif data.startswith("error")==True:
				error_messsage=data.split(":")[1]
				self.error_messages.append(error_messsage)		


