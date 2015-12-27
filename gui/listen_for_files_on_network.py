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
import threading
from inp import inp_get_token_value
from time import sleep
import SocketServer
from util import join_path

respond_ip=""
respond_port=0
output_path=""

def encode_for_tcp(l):
	#Make it 4 bytes long
	l = str(l)
	while len(l) < 200:
		l = l+" " 
	return l

def ensure_dir(f):
	d = os.path.dirname(f)
	if not os.path.exists(d):
		os.makedirs(d)
		#print "making dir",d

class MyTCPHandler(SocketServer.BaseRequestHandler):

	def handle(self):
		global output_path
		print "Receiving files"
		buf=""
		while True:
			data = self.request.recv(1024)
	  		if data:
				buf=buf+data
	    		else:
				#print "zero length packet",len(buf)
				break

			#print len(buf)
		print "buffer len",len(buf)
		if len(buf)==0:
			print "I have recived a zero length buffer"

		#self.request.close()

		pos=0
		job_name=buf[pos:pos+200].rstrip()

		pos=pos+200
		n=0
		frag_error=False
		frag_error_text=""
		while True:
			if pos+200<len(buf):
				name=buf[pos:pos+200].rstrip()
			else:
				frag_error=True
				break

			#print "Name",buf[pos:pos+200]
			pos=pos+200
			#print "Length",buf[pos:pos+200]

			if pos+200<len(buf):
				length=int(buf[pos:pos+200].rstrip())
			else:
				frag_error=True
				break

			pos=pos+200

			if pos+length>len(buf):
				frag_error=True
				break

			#print "Joining ", output_path,name
			output_file=join_path(output_path,name)
			#print "Writing file:"+output_file+"<"
			#print "Output path:"+output_path+"<"
			path=os.path.dirname(output_file)
			#print "name:"+name+"<"
			#print "test:"+os.path.join(output_path,name)+"<"
			ensure_dir(output_file)

			out=open(output_file, 'wb')
			read_in=buf[pos:pos+length]
			de_comp=read_in.decode("zlib")
			out.write(de_comp)
			out.close()
			if n==0:
 				print "{} wrote:".format(self.client_address[0])
				print "Rx file:"+name

			n=n+1

			if output_file.endswith("go.o"):
				os.chmod(output_file, 0777)

			pos=pos+length
			if pos>=len(buf):
				break

		del(buf)

		if frag_error==True:
			print "Data fragmentation error!!!!!!!!!!!!!!!!!!!!!!!!"

		#sleep(0.5)
		if job_name!="":
			if respond_ip!="":
				#print os.path.isdir("/home/rod/liz/orig/otest//home/rod/liz/orig/high_recombination_mobility/1e-8/200")
				#print "Ready!!!!!!!!"
				s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
				s.sendto("iwanttorun#"+job_name, (respond_ip, respond_port))
				s.close()

class listen_for_files_on_network():

	

	def init(self,dump_path,port):
		global output_path
		if dump_path!="":
			output_path=os.path.abspath(dump_path)
		else:
			output_path=""
		print "output_path:"+output_path
		self.port=port
		self.thread = threading.Thread(target = self.listen)
		self.thread.daemon = True
		self.thread.start()

	def respond(self,in_respond_ip,in_respond_port):
		global respond_ip
		global respond_port
		respond_ip=in_respond_ip
		respond_port=in_respond_port

	def listen(self):
		print "TCP file server running"
		server = SocketServer.TCPServer(('', self.port), MyTCPHandler)
		server.serve_forever()



