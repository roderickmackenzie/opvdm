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

#This is some code I wrote to make OPVDM run as part of a cluster. The server code sends data to the clients and tells them what to do, the server then reports back to the opvdm interface once jobs are finished.  It will only run if you invoke the --server or --client from the command line. It's super unstable/buggy and I only run it on a cluster with no route to the internet as there is authentication control. Use at your own risk!

import socket, select
import threading
from threading import Thread
import gobject
#import pyinotify
import multiprocessing
import time
import glob
import os
import socket
import sys
from time import sleep
import random
import atexit
from listen_for_files_on_network import listen_for_files_on_network
from listen_for_files_on_network import encode_for_tcp
from win_lin import running_on_linux
from win_lin import get_distro

if running_on_linux()==True:
	if get_distro=="Fedora":
		from awake import wol

class myjob:
	def __init__(self):
		self.name=""
		self.job_node=""
		self.status="wait"

class mynode:
	def __init__(self):
		self.ip_address=""
		self.mac=""
		self.cpus=0
		self.jobs_running=0
		self.remote_port=0
		self.busy=False
		self.jobs_run=0
		self.load=0

class udp_server:
	def heart_beat(self):
		while(1):
			command="load"
			if len(self.nodes)==0:
				command=command+"#"
			for i in range(0,len(self.nodes)):
				command=command+"#"+str(self.nodes[i].load/self.nodes[i].cpus)
			self.tx_master(command)
			sleep(0.25)

	def clear_cache(self):
		if self.running==False:
			print "clear cache"
			self.jobs_total=0
			self.jobs_run=0
			self.job_list = []
			self.set_all_nodes_busy()
			self.print_bussy_state()

			for node in range(0,len(self.nodes)):
				print "clear",self.nodes[node].ip_address,self.nodes[node].remote_port
				self.s.sendto("clear" , (self.nodes[node].ip_address,self.nodes[node].remote_port))
			self.wait_all_nodes_ready()
			print "Cache cleared"
			self.print_bussy_state()
		else:
			print "Running can't clear cache"

	def print_bussy_state(self):
		print "Bussy state:"
		for node in range(0,len(self.nodes)):
			print self.nodes[node].ip_address,self.nodes[node].busy

	def send_dir(self,node,directory):
		print "Coping directory ",directory," to ", self.nodes[node].ip_address
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.connect((self.nodes[node].ip_address, 10000))

		sock.sendall(encode_for_tcp(directory))		#send job name

		print ">>>>>>>>>>>>>>>>>>>>>>>"+self.exe_name
		if os.path.isfile(self.exe_name)==False:
			return False

		exe_file_name=os.path.basename(self.exe_name)	#send exe file
		sendfile = open(self.exe_name, 'rb')
		data = sendfile.read()


		data_zip=data.encode("zlib")
		sendfile.close()
		sock.sendall(encode_for_tcp(os.path.join(directory,exe_file_name))+encode_for_tcp(len(data_zip)))
		sock.sendall(data_zip)


		for path, dirs, files in os.walk(directory):
			
			for file_name in files:
				whole_file_name=os.path.join(path,file_name)
				#print whole_file_name,"rod",self.ip_address[node]
				sendfile = open(whole_file_name, 'rb')
				data = sendfile.read()
				data_zip=data.encode("zlib")
				sendfile.close()
				sock.sendall(encode_for_tcp(whole_file_name)+encode_for_tcp(len(data_zip)))
				sock.sendall(data_zip)


		sock.close()
		return True

	def print_nodes(self):
		print "name\t\tcpus\tjobs_running\tjobs_run\tjobs/core\tload"
		for i in range(0,len(self.nodes)):
			print self.nodes[i].ip_address,"\t",self.nodes[i].cpus,"\t\t",self.nodes[i].jobs_running,"\t",self.nodes[i].jobs_run,"\t\t",self.nodes[i].jobs_run/self.nodes[i].cpus,"\t\t",self.nodes[i].load

		if self.command_ip!="":
			print "Client IP at:",self.command_ip

	def get_jobs_running(self):
		tot=0
		for i in range(0,len(self.nodes)):
			tot=tot+self.nodes[i].jobs_running
		return tot

	def print_jobs(self):
		print "job\tstatus\tip"
		for i in range(0,len(self.job_list)):
			print self.job_list[i].name,self.job_list[i].status,self.job_list[i].job_node

	def find_job_by_name(self,job):
		for i in range(0,len(self.job_list)):
			if self.job_list[i].name==job:
				return i
		print "job not found",job
		sys.exit()

	def remove_all_jobs(self):
		self.job_list=[]

	def remove_job(self,job):
		i=self.find_job_by_name(job)
		self.job_list.pop(i)

	def remove_waiting_jobs(self):
		for i in range(0,len(self.job_list)):
			if self.job_list[i].status=="wait":
				self.job_list.pop(i)

	def check_job_exists(self,job):
		for i in range(0,len(self.job_list)):
			if self.job_list[i].name==job:
				return True
		return False

	def find_node_from_ip(self,ip):
		for i in range(0,len(self.nodes)):
			if self.nodes[i].ip_address==ip:
				return i


		print "IP not found",ip
		return -1

	def killall_jobs(self):
		self.remove_waiting_jobs()
		for node in range(0,len(self.nodes)):
			self.s.sendto("killall" , (self.nodes[node].ip_address,self.nodes[node].remote_port))

	def run_jobs(self):
		print "Running jobs",len(self.nodes)
		if (len(self.job_list)==0):
			return
		self.print_nodes()
		node=0
		for i in range(0, len(self.job_list)):
			if self.job_list[i].status=="wait":
				for node in range(0,len(self.nodes)):
					if (self.nodes[node].jobs_running<self.nodes[node].cpus):
						self.job_list[i].status="copy"
						self.job_list[i].job_node=self.nodes[node].ip_address
						self.nodes[node].jobs_running=self.nodes[node].jobs_running+1
						t = Thread(target=self.send_dir,args=(node,self.job_list[i].name,))
						t.daemon = True
						t.start()
						break
						#ret=self.send_dir(node,self.job_list[i].name)
						#if ret==False:
						#	print "Can not find go.o"
						#	self.tx_master("node_error#No go.o file")
			
		self.print_nodes()
		self.print_jobs()

	def command_to_all_nodes(self,command):
		print "sending to all nodes:",command 
		node=0
		for i in range(0, len(self.nodes)):
			self.s.sendto(command , (self.nodes[i].ip_address,self.tx_port))

	def wake_nodes(self):
		if running_on_linux()==True:
			if get_distro=="Fedora":
				print "waking all nodes:" 
				node=0
				for i in range(0, len(self.nodes)):
					print "waking", self.nodes[i].mac
					wol.send_magic_packet(self.nodes[i].mac)

	def add_job(self,command):
		print "Adding job",command
		pos=len(self.job_list)
		self.job_list.append(myjob())
		self.job_list[pos].name=command
		self.job_list[pos].status="wait"

	def set_node_busy(self,i):
		self.nodes[i].busy=True

	def set_node_not_busy(self,node):
		self.nodes[node].busy=False

	def set_all_nodes_busy(self):
		for i in range(0,len(self.nodes)):
			self.set_node_busy(i)
			

		self.print_bussy_state()

	def wait_all_nodes_ready(self):
		while 1:
			self.rx_command()

			finished=True			
			for i in range(0,len(self.nodes)):
				if self.nodes[i].busy==True:
					finished=False
			if finished==True:
				break

	def add_node(self,ip,port,cpus,mac):
		print "Add node",ip
		self.nodes.append(mynode())
		pos=len(self.nodes)-1
		self.nodes[pos].cpus=cpus
		self.nodes[pos].ip_address=ip
		self.nodes[pos].remote_port=port
		self.nodes[pos].mac=mac

		nodes_file = open("node_list.inp", "w")
		for i in range(0,len(self.nodes)):
			nodes_file.write(self.nodes[i].ip_address+"\n")
		nodes_file.close()

	def register_nodes(self):
		if os.path.isfile("node_list.inp"):
			f = open("node_list.inp")
			lines = f.readlines()
			f.close()
			for i in range(0,len(lines)):
				line=lines[i].strip()
				self.s.sendto("register#" , (line,self.tx_port))


	def remove_node(self,ip):
		print "Remove node",ip
		for i in range(0,len(self.nodes)):
			if self.nodes[i].ip_address==ip:
				self.nodes.pop(i)
				break
	def tx_master(self,data):
		#print "tx to server=",data

		self.s.sendto(data , (self.master_ip,8889))

	def decode(self,data,client_ip_address):
		if data.startswith("init"):
			split=data.split("#")
			cpus=int(split[1])
			mac=split[2]
			print mac
			self.add_node(client_ip_address,self.tx_port,cpus,mac)
			self.print_nodes()
		if data.startswith("clear_cache"):
			self.clear_cache()
			self.tx_master("ok")

		if data.startswith("run"):
			if self.running==False:
				self.running=True
				random.shuffle(self.job_list)

				self.jobs_run=0

				self.run_jobs()
			self.jobs_total=len(self.job_list)

		if data.startswith("addjobs"):
			input_jobs=data.split("#")
			for i in range(1,len(input_jobs)):
				self.add_job(input_jobs[i])
		if data.startswith("set_master_ip"):
			self.master_ip=client_ip_address
			self.tx_master("ok")
		if data.startswith("set_exe"):
			i=data.split("#")
			self.exe_name=i[1]
			self.tx_master("ok")
		if data.startswith("finished"):
			job_name=data.split("#")[1]
			if self.check_job_exists(job_name)==True:
				self.remove_job(job_name)
				self.jobs_run=self.jobs_run+1
				i=self.find_node_from_ip(client_ip_address)

				if i==-1:
					print "finished"
					sys.exit()

				self.nodes[i].jobs_running=self.nodes[i].jobs_running-1
				self.nodes[i].jobs_run=self.nodes[i].jobs_run+1
				self.run_jobs()
				percent=str(100.0*float(self.jobs_run)/float(self.jobs_total))
				print percent
				self.s.sendto("percent#"+percent , (self.master_ip,8889))
				self.s.sendto("job_finished#"+job_name , (self.master_ip,8889))
				if self.get_jobs_running()==0:
					self.job_list=[]
					self.s.sendto("finished" , (self.master_ip,8889))
					self.running=False
					print "All jobs finished"
		if data.startswith("exit"):
			self.remove_node(client_ip_address)
			self.print_nodes()
		if data.startswith("killall"):
			self.killall_jobs()
		if data.startswith("sleep"):
			self.command_to_all_nodes("sleep")
		if data.startswith("poweroff"):
			self.command_to_all_nodes("poweroff")
		if data.startswith("ready"):
			print "rx ready!!!!!!!!!!!!",client_ip_address
			self.print_bussy_state()
			node=self.find_node_from_ip(client_ip_address)
			if node==-1:
				print "ready"
				sys.exit()

			self.set_node_not_busy(node)
			self.print_bussy_state()

		if data.startswith("iwanttorun"):
			mysplit=data.split("#")
			job_name=mysplit[1]
			i=self.find_job_by_name(job_name)
			self.job_list[i].status="run"
			self.s.sendto("runjob#"+job_name , (client_ip_address,self.tx_port))
		if data.startswith("get_data"):
			for i in range(0,len(self.job_list)):
				if self.job_list[i].status=="run":
					self.s.sendto("get_data#"+self.job_list[i].name , (self.job_list[i].job_node,self.tx_port))
		if data.startswith("print_jobs"):
			self.print_jobs()

		if data.startswith("wake_nodes"):
			self.wake_nodes()

		if data.startswith("load"):
			#print data
			mysplit=data.split("#")
			my_load=float(mysplit[1])
			node=self.find_node_from_ip(client_ip_address)
			if node==-1:
				print "load from unknown node telling it to register.."
				self.s.sendto("register#" , (client_ip_address,self.tx_port))
			else:
				self.nodes[node].load=my_load

				

	def rx_command(self):
		#print "Waiting for command"
		d = self.s.recvfrom(1024)
		#print "rx=",d
		data = d[0]
		addr = d[1]

		if not data: 
			return False

		if data:
			client_ip_address=addr[0]
			self.decode(data,client_ip_address)

		return True 

	def goodbye(self):
		print "Goodbye!"
		self.tx_master("server_quit#")
		sys.exit()

	def open_tpc(self):
		while True:
			data = self.request.recv(1024)
			if data:
				print "ctrl server",data,format(self.client_address[0])
			else:
				sleep(0.1)
			data = self.request.recv(1024)
			if data=="open_tpc":
				self.open_tpc()

	def listen_tpc(self):
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		server_address = ('', 10002)
		print "TCP starting TCP server on", server_address,"\n"
		sock.bind(server_address)
		sock.listen(10)

		# Wait for a connection
		while True:
			print >>sys.stderr, 'TCP waiting for a connection'
			connection, client_address = sock.accept()


			print  'TCP connection from!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!', client_address
			self.command_ip=client_address
			self.print_nodes()
			# Receive the data in small chunks and retransmit it

			while True:
				try:
					data = connection.recv(16)
				except:
					print "problem with connection.recv(16)"
					break

				print "TCP received:",data
				if data:
					print >>sys.stderr, 'sending data back to the client'
					connection.sendall(data)
			   	else:
					print >>sys.stderr, 'no more data from', client_address
					break
		
			connection.close()

	def start(self):
		self.running=False
		self.job_list = []

		self.nodes=[]
		self.command_ip=""
		self.exe_name="none"
		self.jobs_total=0
		self.jobs_run=0
		self.master_ip=""
		HOST = ''   # Symbolic name meaning all available interfaces
		self.rx_port = 8888
		self.tx_port = 8887
		net_files=listen_for_files_on_network()
		net_files.init("",10001)

		self.thread_tpc = threading.Thread(target = self.listen_tpc)
		self.thread_tpc.daemon = True
		self.thread_tpc.start()

		# Datagram (udp) socket
		try :
			self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			print 'Socket created'
		except socket.error, msg :
			print 'Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
			sys.exit()


		# Bind socket to local host and port
		try:
			self.s.bind((HOST, self.rx_port))
		except socket.error , msg:
			print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
			sys.exit()

		print 'Socket bind complete'

		self.register_nodes()

		heart_beat_thread = Thread(target=self.heart_beat)
		heart_beat_thread.daemon = True
		heart_beat_thread.start()

		atexit.register(self.goodbye)

		while 1:
			# receive data from client (data, addr)
			ret=self.rx_command()
			if ret==False:
				break
			#reply = 'OK...' + data

			#self.s.sendto(reply , addr)
			#print 'Message[' + addr[0] + ':' + str(addr[1]) + '] - ' + data.strip()

		self.s.close()


