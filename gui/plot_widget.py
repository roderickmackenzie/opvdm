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
import re
import sys
import os
import shutil
from token_lib import tokens
from numpy import *
from util import pango_to_gnuplot
from util import read_data_2d
from plot_io import plot_load_info
import matplotlib.pyplot as plt
from matplotlib.widgets import Cursor
from plot_export import plot_export 
from numpy import arange, sin, pi, zeros
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg
from matplotlib.figure import Figure
from util import read_xyz_data
import matplotlib.ticker as ticker
from gui_util import dlg_get_text
from inp import inp_load_file
from inp import inp_write_lines_to_file
from inp import inp_search_token_value
from util import numbers_to_latex
from util import pygtk_to_latex_subscript
from util import fx_with_units
from plot_state import plot_state
from plot import plot_populate_plot_token
from plot_io import plot_save_oplot_file
from plot_state import plot_state
from gui_util import dlg_get_multi_text
from util import time_with_units

class NavigationToolbar(NavigationToolbar2GTKAgg):
    # only display the buttons we need
    toolitems = [t for t in NavigationToolbar2GTKAgg.toolitems if
                 t[0] in ('Home', 'Pan', 'Zoom', 'Subplots')]


class plot_widget(gtk.VBox):

	def on_key_press_event(self,widget, event):
		keyname = gtk.gdk.keyval_name(event.keyval)
		#print "Key %s (%d) was pressed" % (keyname, event.keyval)
		if keyname=="a":
			self.do_plot()

		if keyname=="g":
			if self.plot_token.grid==False:
				for i in range(0,len(self.ax)):
					self.ax[i].grid(True)
				self.plot_token.grid=True
			else:
				for i in range(0,len(self.ax)):
					self.ax[i].grid(False)
				self.plot_token.grid=False
		if keyname=="r":
			if self.lx==None:
				for i in range(0,len(self.ax)):
					self.lx = self.ax[i].axhline(color='k')
					self.ly = self.ax[i].axvline(color='k')
			self.lx.set_ydata(self.ydata)
			self.ly.set_xdata(self.xdata)

		if keyname=="l":
			if self.plot_token.logy==True:
				self.plot_token.logy=False
				for i in range(0,len(self.ax)):
					self.ax[i].set_yscale("linear")
			else:
				self.plot_token.logy=True
				for i in range(0,len(self.ax)):
					self.ax[i].set_yscale("log")

		if keyname=="L":
			if self.plot_token.logx==True:
				self.plot_token.logx=False
				for i in range(0,len(self.ax)):
					self.ax[i].set_xscale("linear")
			else:
				self.plot_token.logx=True
				for i in range(0,len(self.ax)):
					self.ax[i].set_xscale("log")

		if keyname=="q":
			self.win.destroy()

		if keyname == "c":
			if event.state == gtk.gdk.CONTROL_MASK:
				self.do_clip()

		self.fig.canvas.draw()
		plot_save_oplot_file(self.config_file,self.plot_token)

	def do_clip(self):
		snap = self.canvas.get_snapshot()
		pixbuf = gtk.gdk.pixbuf_get_from_drawable(None, snap, snap.get_colormap(),0,0,0,0,snap.get_size()[0], snap.get_size()[1])
		clip = gtk.Clipboard()
		clip.set_image(pixbuf)



	def mouse_move(self,event):
		#print event.xdata, event.ydata
		self.xdata=event.xdata
		self.ydata=event.ydata
	
		#self.fig.canvas.draw()

		#except:
		#	print "Error opening file ",file_name


	def sub_zero_frame(self,t,s,i):
		tt=[]
		ss=[]
		z=[]
		#print self.zero_frame_enable
		if self.zero_frame_enable==True:
			if read_xyz_data(tt,ss,z,self.zero_frame_list[i])==True:
				for ii in range(0,len(t)):
					s[ii]=s[ii]-ss[ii]

	def read_data_file(self,t,s,z,index):
		if read_xyz_data(t,s,z,self.input_files[index])==True:
			self.sub_zero_frame(t,s,index)
			my_min=0.0;
				

			for ii in range(0,len(t)):
				t[ii]=t[ii]*self.plot_token.x_mul
				s[ii]=s[ii]*self.plot_token.y_mul

				if self.plot_token.invert_y==True:
					s[ii]=-s[ii]					

				if self.plot_token.subtract_first_point==True:
					if ii==0:
						val=s[0]
					s[ii]=s[ii]-val

	
			if self.plot_token.add_min==True:
				my_min=min(s)
				for ii in range(0,len(t)):
					s[ii]=s[ii]-my_min

			if self.plot_token.normalize==True:
				local_max=max(s)
				for ii in range(0,len(t)):
					if s[ii]!=0:
						s[ii]=s[ii]/local_max
					else:
						s[ii]=0.0


			if index<len(self.plot_id):
				plot_number=self.plot_id[index]
			else:
				plot_number=index
			print "YMIN=",self.plot_token.ymin
			if self.plot_token.ymax!=-1:
				self.ax[plot_number].set_ylim((self.plot_token.ymin,self.plot_token.ymax))
			return True
		else:
			return False

	def do_plot(self):
		print "PLOT TYPE=",self.plot_token.type
		if self.plot_token!=None and len(self.plot_id)!=0:
			plot_number=0

			self.fig.clf()
			self.fig.subplots_adjust(bottom=0.2)
			self.fig.subplots_adjust(bottom=0.2)
			self.fig.subplots_adjust(left=0.1)
			self.fig.subplots_adjust(hspace = .001)

			title=""
			if self.plot_title=="":
				title=self.plot_token.title
			else:
				title=self.plot_title

			if self.plot_token.time!=-1.0 and self.plot_token.Vexternal!=-1.0:
				mul,unit=time_with_units(self.plot_token.time)
				title=title+" V="+str(self.plot_token.Vexternal)+" time="+str(self.plot_token.time*mul)+" "+unit

			self.fig.suptitle(title)

			self.ax=[]
			number_of_plots=max(self.plot_id)+1
			if self.plot_token.type=="heat":
				number_of_plots=1

			if number_of_plots>1:
				yloc = plt.MaxNLocator(4)
			else:
				yloc = plt.MaxNLocator(10)




			for i in range(0,number_of_plots):
				self.ax.append(self.fig.add_subplot(number_of_plots,1,i+1, axisbg='white'))
				#Only place label on bottom plot
				if i==number_of_plots-1:
					print self.plot_token.x_label,self.plot_token.x_units
					self.ax[i].set_xlabel(self.plot_token.x_label+" ("+self.plot_token.x_units+")")
					
				else:
					self.ax[i].tick_params(axis='x', which='both', bottom='off', top='off',labelbottom='off') # labels along the bottom edge are off

				#Only place y label on center plot
				if self.plot_token.normalize==True or self.plot_token.norm_to_peak_of_all_data==True:
					y_text="Normalized "+self.plot_token.y_label
					y_units="au"
				else:
					y_text=self.plot_token.y_label
					y_units=self.plot_token.y_units
				if i==math.trunc(number_of_plots/2):
					self.ax[i].set_ylabel(y_text+" ("+y_units+")")

				if self.plot_token.logx==True:
					self.ax[i].set_xscale("log")

				if self.plot_token.logy==True:
					self.ax[i].set_yscale("log")


			lines=[]
			files=[]

			my_max=1.0
			print "token type==",self.plot_token.type
			if self.plot_token.type=="xy":

				all_max=1.0
				if self.plot_token.norm_to_peak_of_all_data==True:
					m=[]
					my_max=-1e40
					for i in range(0, len(self.input_files)):
						t=[]
						s=[]
						z=[]
						if self.read_data_file(t,s,z,i)==True:
							if max(s)>my_max:
								my_max=max(s)
					all_max=my_max

				for i in range(0, len(self.input_files)):
					#t,s = loadtxt(self.input_files[i], unpack=True)
					t=[]
					s=[]
					z=[]
					if self.read_data_file(t,s,z,i)==True:
						#print "z==",z
						if all_max!=1.0:
							for ii in range(0,len(s)):
								s[ii]=s[ii]/all_max
						print plot_number,len(self.ax)
						#print len(self.ax),plot_number,i,len(self.color),len(self.marker)
						Ec, = self.ax[plot_number].plot(t,s, linewidth=3 ,alpha=1.0,color=self.color[i],marker=self.marker[i])

						#label data if required
						if self.plot_token.label_data==True:
							for ii in range(0,len(t)):
								if z[ii]!="":
									self.ax[plot_number].annotate(fx_with_units(float(z[ii])),xy = (t[ii], s[ii]), xytext = (-20, 20),textcoords = 'offset points', ha = 'right', va = 'bottom',bbox = dict(boxstyle = 'round,pad=0.5', fc = 'yellow', alpha = 0.5),arrowprops = dict(arrowstyle = '->', connectionstyle = 'arc3,rad=0'))

						if number_of_plots>1:
							self.ax[plot_number].yaxis.set_major_formatter(ticker.FormatStrFormatter('%0.1e'))
							if min(s)!=max(s):
								print "TICKS=",(max(s)-min(s))/4.0
								self.ax[plot_number].yaxis.set_ticks(arange(min(s), max(s), (max(s)-min(s))/4.0 ))

						#print self.labels,i,self.labels[i]
						if self.labels[i]!="":
							#print "Rod=",self.labels[i]
							#print self.plot_token.key_units
							files.append("$"+numbers_to_latex(str(self.labels[i]))+" "+pygtk_to_latex_subscript(self.plot_token.key_units)+"$")

							lines.append(Ec)

				self.lx = None
				self.ly = None
				if self.plot_token.legend_pos=="No key":
					self.ax[plot_number].legend_ = None
				else:
					legend=self.fig.legend(lines, files, self.plot_token.legend_pos)
			elif self.plot_token.type=="3d":
				x=[]
				y=[]
				z=[]
				if read_data_2d(x,y,z,self.input_files[0])==True:

					x_len=len(x)
					y_len=len(y)
					data = zeros((x_len,y_len))
					for xx in range(0,x_len):
						for yy in range(0,y_len):
							data[xx][yy]=z[xx][yy]
					self.ax[0].pcolor(data)

					#self.ax[0].plot_surface(x, y, z, rstride=1, cstride=1, cmap=cm.coolwarm,linewidth=0, antialiased=False)
					#self.ax[0].invert_yaxis()
					#self.ax[0].xaxis.tick_top()
			elif self.plot_token.type=="heat":
				x=[]
				y=[]
				z=[]

				pos=float(self.plot_token.x_start)
				x_step=(float(self.plot_token.x_stop)-float(self.plot_token.x_start))/self.plot_token.x_points
				while(pos<float(self.plot_token.x_stop)):
					x.append(pos)
					pos=pos+x_step

				pos=float(self.plot_token.y_start)
				y_step=(float(self.plot_token.y_stop)-float(self.plot_token.y_start))/self.plot_token.y_points
				while(pos<float(self.plot_token.y_stop)):
					y.append(pos)
					pos=pos+y_step

				data = zeros((len(y),len(x)))

				for ii in range(0,len(self.input_files)):
					t=[]
					s=[]
					z=[]
					if self.read_data_file(t,s,z,ii)==True:
						print self.input_files[ii]
						for points in range(0,len(t)):
							found=0
							if t[points]>x[0]:
								for x_pos in range(0,len(x)):
									if x[x_pos]>t[points]:
										found=found+1
										break

							if s[points]>y[0]:
								for y_pos in range(0,len(y)):
									if y_pos!=0:
										if y[y_pos]>s[points]:
											found=found+1
											break
							if found==2:
								print "adding data at",x_pos,y_pos
								if data[y_pos][x_pos]<10.0:
									data[y_pos][x_pos]=data[y_pos][x_pos]+1
							else:
								print "not adding point",t[points],s[points]

				print x
				print y
				print data
				x_grid, y_grid = mgrid[float(self.plot_token.y_start):float(self.plot_token.y_stop):complex(0, len(y)), float(self.plot_token.x_start):float(self.plot_token.x_stop):complex(0, len(x))]
				self.ax[0].pcolor(y_grid,x_grid,data)

			else:
				x=[]
				y=[]
				z=[]

				if read_data_2d(x,y,z,self.input_files[0])==True:
					maxx=-1
					maxy=-1
					for i in range(0,len(z)):
						if x[i]>maxx:
							maxx=x[i]

						if y[i]>maxy:
							maxy=y[i]

					maxx=maxx+1
					maxy=maxy+1

					data = zeros((maxy,maxx))


					for i in range(0,len(z)):
						data[y[i]][x[i]]= random.random()+5
						self.ax[0].text(x[i], y[i]+float(maxy)/float(len(z))+0.1,'%.1e' %  z[i], fontsize=12)

					#fig, ax = plt.subplots()
					self.ax[0].pcolor(data,cmap=plt.cm.Blues)

					self.ax[0].invert_yaxis()
					self.ax[0].xaxis.tick_top()

			#self.fig.tight_layout(pad=0.0, w_pad=0.0, h_pad=0.0)
			self.fig.canvas.draw()
			print "exit do plot"

	def callback_plot_save(self, widget, data=None):
		dialog = gtk.FileChooserDialog("Directory to make a gnuplot script..",
                               None,
                               gtk.FILE_CHOOSER_ACTION_OPEN,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_SAVE, gtk.RESPONSE_OK))
		print os.path.basename(self.output_file),os.path.dirname(self.output_file)
		dialog.set_current_folder(os.path.dirname(self.output_file))
		dialog.set_default_response(gtk.RESPONSE_OK)
		dialog.set_action(gtk.FILE_CHOOSER_ACTION_SAVE)

		filter = gtk.FileFilter()
		filter.set_name("png file")
		filter.add_pattern("*.png")
		dialog.add_filter(filter)

		filter = gtk.FileFilter()
		filter.set_name("gnuplot script")
		filter.add_pattern("*.")
		dialog.add_filter(filter)

		dialog.set_current_name(os.path.basename(self.output_file))

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			print "Logscale x = ",self.plot_token.logx
			plot_export(dialog.get_filename(),self.input_files,self.plot_token,self.fig)

		elif response == gtk.RESPONSE_CANCEL:
		    print 'Closed, no dir selected'

		dialog.destroy()

	def set_labels(self,labels):
		self.labels=labels

	def set_plot_ids(self,plot_id):
		self.plot_id=plot_id

	def load_data(self,input_files,config_file):

		self.input_files=input_files
		self.config_file=config_file

		if config_file=="":
			config_file=os.path.splitext(input_files[0])[0]+".oplot"

		loaded=False		
		self.plot_token=plot_state()

		#Try and get the data from the config file
		if plot_load_info(self.plot_token,config_file)==True:
			loaded=True
			print "I HAVE LOADED THE OPLOT FILE",self.plot_token.type

		#If that did not work get it from the data file
		if loaded==False:
			if plot_load_info(self.plot_token,input_files[0])==True:
				loaded=True

		print "the config file is",config_file
		print input_files,loaded
		if loaded==True:

			if len(self.plot_id)==0:
				for i in range(0,len(input_files)):
					self.plot_id.append(0)
		
			self.plot_token.path=os.path.dirname(config_file)
			if self.plot_token.tag0=="":
				self.plot_token.file0=os.path.basename(input_files[0])

			plot_save_oplot_file(config_file,self.plot_token)

			self.output_file=os.path.splitext(config_file)[0]+".png"

			#ret=plot_populate_plot_token(plot_token,self.input_files[0])
			#if ret==True:
			#print "Rod",input_files
			title=self.plot_token.title
			self.win.set_title(title+" - www.opvdm.com")
			lines=[]

			ret=plot_load_info(self.plot_token,input_files[0])
			myitem=self.item_factory.get_item("/Math/Subtract first point")
			myitem.set_active(self.plot_token.subtract_first_point)

			myitem=self.item_factory.get_item("/Math/Add min point")
			myitem.set_active(self.plot_token.add_min)

			myitem=self.item_factory.get_item("/Math/Invert y-axis")
			myitem.set_active(self.plot_token.invert_y)

			myitem=self.item_factory.get_item("/Math/Norm to 1.0 y")
			myitem.set_active(self.plot_token.normalize)

			myitem=self.item_factory.get_item("/Math/Norm to peak of all data")
			myitem.set_active(self.plot_token.norm_to_peak_of_all_data)

			print "Loaded OK",self.config_file

			test_file=self.input_files[0]
			for i in range(0,len(self.input_files)):
				if os.path.isfile(self.input_files[i]):
					test_file=self.input_files[i]

			print "test_file=",test_file
			print "Exit here"


	def gen_colors_black(self,repeat_lines):
		#make 100 black colors
		marker_base=["","x","o"]
		c_tot=[]
		base=[[0.0,0.0,0.0]]
		self.marker=[]
		self.color=[]
		for i in range(0,100):
			for n in range(0,repeat_lines):
				self.color.append([base[0][0],base[0][1],base[0][2]])
				self.marker.append(marker_base[n])

	def gen_colors(self,repeat_lines):
		base=[[0.0,0.0,1.0],[0.0,1.0,0.0],[1.0,0.0,0.0],[0.0,1.0,1.0],[1.0,1.0,0.0],[1.0,0.0,1.0]]
		c_tot=[]
		self.marker=[]
		marker_base=["","x","o"]
		mul=1.0
		self.color=[]
		for rounds in range(0,10):
			for i in range(0,len(base)):
				for n in range(0,repeat_lines):
					c_tot.append([base[i][0]*mul,base[i][1]*mul,base[i][2]*mul])
					self.marker.append(marker_base[n])
			mul=mul*0.5

		self.color=c_tot

	def callback_black(self, data, widget):
		self.gen_colors_black(1)
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_rainbow(self, data, widget):
		self.gen_colors(1)
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_save(self, data, widget):
		plot_export(self.output_file,self.input_files,self.plot_token,self.fig)

	def callback_key(self, data, widget):
		self.plot_token.legend_pos=widget.get_label()
		print self.config_file,self.plot_token
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_units(self, data, widget):
		units=dlg_get_text( "Units:", self.plot_token.key_units)
		if units!=None:
			self.plot_token.key_units=units
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()


	def callback_autoscale_y(self, data, widget):
		if widget.get_active()==True:
			self.plot_token.ymax=-1
			self.plot_token.ymin=-1
		else:
			xmin, xmax, ymin, ymax = self.ax[0].axis()
			self.plot_token.ymax=ymax
			self.plot_token.ymin=ymin

	def callback_normtoone_y(self, data, widget):
		if widget.get_active()==True:
			self.plot_token.normalize=True
		else:
			self.plot_token.normalize=False
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_norm_to_peak_of_all_data(self, data, widget):
		if widget.get_active()==True:
			self.plot_token.norm_to_peak_of_all_data=True
		else:
			self.plot_token.norm_to_peak_of_all_data=False
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_log_scale_y(self, widget, data):
		self.plot_token.logy=data.get_active()
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_log_scale_x(self, widget, data):
		self.plot_token.logx=data.get_active()
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_label_data(self, widget, data):
		self.plot_token.label_data=data.get_active()
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_set_heat_map(self, widget, data):
		self.plot_token.type="heat"
		plot_save_oplot_file(self.config_file,self.plot_token)
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_heat_map_edit(self, widget, data):
		[a,b,c,d,e,f] = dlg_get_multi_text("2D plot editor", [["x start",str(self.plot_token.x_start)],["x stop",str(self.plot_token.x_stop)],["x points",str(self.plot_token.x_points)],["y start",str(self.plot_token.y_start)],["y stop",str(self.plot_token.y_stop)],["y points",str(self.plot_token.y_points)]])
		print "---------",a,b,c,d,e,f
		self.plot_token.x_start=float(a)
		self.plot_token.x_stop=float(b)
		self.plot_token.x_points=float(c)

		self.plot_token.y_start=float(d)
		self.plot_token.y_stop=float(e)
		self.plot_token.y_points=float(f)

		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_set_xy_plot(self, widget, data):
		self.plot_token.type="xy"
		plot_save_oplot_file(self.config_file,self.plot_token)
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_invert_y(self, widget, data):
		self.plot_token.invert_y=data.get_active()
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_subtract_first_point(self, widget, data):
		self.plot_token.subtract_first_point=data.get_active()
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_add_min(self, widget, data):
		self.plot_token.add_min=data.get_active()
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def update(self):
		self.load_data(self.input_files,self.config_file)
		self.do_plot()

	def callback_refresh(self, widget, data=None):
		self.update()

	def init(self,in_window):
		self.zero_frame_enable=False
		self.zero_frame_list=[]
		#print type(in_window)
		self.plot_title=""
		self.gen_colors(1)
		#self.color =['r','g','b','y','o','r','g','b','y','o']
		self.win=in_window
		self.toolbar = gtk.Toolbar()
		self.toolbar.set_style(gtk.TOOLBAR_ICONS)
		self.toolbar.set_size_request(-1, 50)
		self.toolbar.show()
		self.config_file=""
		self.plot_token=None
		self.labels=[]
		self.fig = Figure(figsize=(2.5,2), dpi=100)
		self.plot_id=[]
		self.canvas = FigureCanvas(self.fig)  # a gtk.DrawingArea

		self.fig.canvas.mpl_connect('motion_notify_event', self.mouse_move)

		self.item_factory = gtk.ItemFactory(gtk.MenuBar, "<main>", None)

		menu_items = (
		    ( "/_File",         None,         None, 0, "<Branch>" ),
		    ( "/_File/Save",  None,         self.callback_save , 0, None ),
		    ( "/_File/Save As...",  None,         self.callback_plot_save , 0, None ),
		    ( "/_Key",         None,         None, 0, "<Branch>" ),
		    ( "/_Key/No key",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		    ( "/_Key/upper right",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		    ( "/_Key/upper left",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		    ( "/_Key/lower left",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		    ( "/_Key/lower right",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		    ( "/_Key/right",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		    ( "/_Key/center right",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		    ( "/_Key/lower center",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		    ( "/_Key/upper center",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		    ( "/_Key/center",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		    ( "/_Key/Units",  None,         self.callback_units , 0, None ),
		    ( "/_Color/Black",  None,         self.callback_black , 0, None ),
		    ( "/_Color/Rainbow",  None,         self.callback_rainbow , 0, None ),
		    ( "/_Axis/_Autoscale y",     None, self.callback_autoscale_y, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Axis/_Set log scale y",     None, self.callback_toggle_log_scale_y, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Axis/_Set log scale x",     None, self.callback_toggle_log_scale_x, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Axis/_Label data",     None, self.callback_toggle_label_data, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Math/_Subtract first point",     None, self.callback_toggle_subtract_first_point, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Math/_Add min point",     None, self.callback_toggle_add_min, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Math/_Invert y-axis",     None, self.callback_toggle_invert_y, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Math/_Norm to 1.0 y",     None, self.callback_normtoone_y, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Math/_Norm to peak of all data",     None, self.callback_norm_to_peak_of_all_data, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Math/_Heat map",     None, self.callback_set_heat_map, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Math/_Heat map edit",     None, self.callback_heat_map_edit, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Math/_xy plot",     None, self.callback_set_xy_plot, 0, "<ToggleItem>", "gtk-save" ),
		    )

		self.item_factory.create_items(menu_items)


		menubar=self.item_factory.get_widget("<main>")
		menubar.show_all()
		self.pack_start(menubar, False, True, 0)

		self.pack_start(self.toolbar, False, True, 0)	

		pos=0

		self.plot_save = gtk.ToolButton(gtk.STOCK_SAVE)
		self.plot_save.connect("clicked", self.callback_plot_save)
		self.toolbar.add(self.plot_save)
		pos=pos+1

		refresh = gtk.ToolButton(gtk.STOCK_REFRESH)
		refresh.connect("clicked", self.callback_refresh)
		self.toolbar.insert(refresh, pos)
		pos=pos+1

		plot_toolbar = NavigationToolbar(self.canvas, self.win)
		plot_toolbar.show()
		box=gtk.HBox(True, 1)
		box.set_size_request(400,-1)
		box.show()
		box.pack_start(plot_toolbar, True, True, 0)
		tb_comboitem = gtk.ToolItem();
		tb_comboitem.add(box);
		tb_comboitem.show_all()

		self.toolbar.add(tb_comboitem)
		pos=pos+1



		self.toolbar.show_all()


		self.canvas.figure.patch.set_facecolor('white')
		self.canvas.set_size_request(650, 400)
		self.pack_start(self.canvas, True, True, 0)	

		#self.fig.canvas.draw()

		self.canvas.show()

		self.win.connect('key_press_event', self.on_key_press_event)
