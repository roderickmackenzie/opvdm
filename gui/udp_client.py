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

import socket, select, string, sys
import os
import socket
import sys
import multiprocessing
import shutil
from inp import inp_get_token_value
from listen_for_files_on_network import listen_for_files_on_network
from listen_for_files_on_network import encode_for_tcp
import atexit
from time import sleep
from util import join_path
from uuid import getnode as get_mac
from threading import Thread

def return_dir(host,remote_path,otest_path):
	client_path=join_path(otest_path,remote_path)
	print "searching",client_path,remote_path
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((host, 10001))
	sock.sendall(encode_for_tcp(remote_path))
	for path, dirs, files in os.walk(client_path):
		print path
		for file_name in files:
			#print file_name,"rod",host
			if file_name.endswith(".dat")==True or file_name.endswith(".inp")==True :
				local_file=os.path.join(path,file_name)
				target=os.path.join(path,file_name)
				if target.startswith(otest_path):
	    				target=target[len(otest_path):]

				if os.path.isfile(local_file):
					got_data=False
					try:
						sendfile = open(local_file, 'rb')
						#print "I will send",local_file,target,host
						data = sendfile.read()
						zip_data=data.encode("zlib")
						sendfile.close()
						got_data=True
					except:
						print "I could not open the file:"+local_file
						got_data=False

					if len(data)!=0 and got_data==True:
						try:
							sock.sendall(encode_for_tcp(target)+encode_for_tcp(len(zip_data)))
							sock.sendall(zip_data)
						except:
							print "I could not send the data!!!"
							return False
							#sock.close()

	sock.close()
	return True

class udp_client():

	def heart_beat(self):
		while(1):
			my_load=os.getloadavg()
			my_load=my_load[0]
			command="load#"+str(my_load)
			self.socket.sendto(command, (self.server_ip, self.tx_port))
			sleep(0.25)

	def goodbye(self):
		print "Goodbye!"
		os.system("killall go.o")
		self.socket.sendto("exit#", (self.server_ip, self.tx_port))
		self.socket.close()
		sys.exit()

	def send_init(self):
		cpus=multiprocessing.cpu_count()
		mac=':'.join('%02X' % ((get_mac() >> 8*i) & 0xff) for i in reversed(xrange(6)))
		self.socket.sendto("init#"+str(cpus)+"#"+mac, (self.server_ip, self.tx_port))

	def init(self):
		self.tx_port = 8888
		self.rx_port= 8887
		self.server_ip=inp_get_token_value("server.inp", "#server_ip")

		otest_path="/dev/shm/otest"

		if not os.path.exists(otest_path):
			os.makedirs(otest_path)

		#with open("/home/rod/share/orig/debug.dat", "w") as myfile:
		#	myfile.write("debug info")


		net_files=listen_for_files_on_network()
		net_files.init(otest_path,10000)
		net_files.respond(self.server_ip, self.tx_port)

		try:
		    self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		except socket.error:
		    print 'Failed to create socket'
		    sys.exit()

		try:
			self.socket.bind(('', self.rx_port))
		except socket.error , msg:
			print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
			sys.exit()

		atexit.register(self.goodbye)
		 
		self.send_init()

		heart_beat_thread = Thread(target=self.heart_beat)
		heart_beat_thread.daemon = True
		heart_beat_thread.start()

		while(1) :
			d = self.socket.recvfrom(1024)
			data = d[0]
			addr = d[1]
			ip=addr[0]
			if not data: 
				break
			print "command=",data
			if data.startswith("runjob"):
				command=data.split("#")
				job_name=command[1]
				if os.fork() == 0:
					#exe_name=os.path.abspath(otest_path+"go.o")
					#cmd=exe_name
					path=join_path(otest_path,job_name)
					print "RUNNING NOW",path
					#f = open("/home/rod/one.dat", "w")
					#f.write( exe_name+"\n"  )
					#f.write( path+"\n"  )
					#f.close()

					os.chdir(path)

					#with open("/home/rod/share/orig/debug.dat", "a") as myfile:
					#	myfile.write("RUNNING NOW'"+cmd+"' '"+path+"'")


					os.system("./go.o")
					sleep(1)
					count=0
					while (count<200):
						ret=return_dir(self.server_ip,command[1],otest_path)
						if (ret==True):
							break
						sleep(1)
						print "Failed to send resending!!!!!!"
						count=count+1

					if ret==False:
						print "Could not send data"
					else:
						path_to_delete=join_path(otest_path,command[1])
						print "Clean up "+path_to_delete
						shutil.rmtree(path_to_delete)

					self.socket.sendto("finished#"+command[1], (self.server_ip, self.tx_port))
					os._exit(0)

			if data.startswith("get_data"):
				command=data.split("#")
				job_name=command[1]
				return_dir(self.server_ip,command[1],otest_path)

			if data.startswith("clear"):
				print "I will delete",otest_path
				shutil.rmtree(otest_path)
				os.makedirs(otest_path)
				self.socket.sendto("ready#", (self.server_ip, self.tx_port))
				print "I have deleted",otest_path
			if data.startswith("killall"):
				os.system("killall go.o")
			if data.startswith("register"):
				self.server_ip=ip
				net_files.respond(self.server_ip, self.tx_port)
				self.send_init()
			if data.startswith("sleep"):
				os.system("sudo pm-suspend")
			if data.startswith("poweroff"):
				os.system("sudo poweroff")

