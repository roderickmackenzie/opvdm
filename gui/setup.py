from distutils.core import setup
import py2exe
import os
import sys
import matplotlib
import shutil
import glob

#run with python setup.py py2exe
dest_path=os.path.join(os.getcwd(),"dist")

if os.path.isdir(dest_path)==False:
	os.makedirs(dest_path)
#	shutil.rmtree(dest_path)

print "copying from z:\\auto_dep\\"
for file in glob.glob("z:\\auto_dep\\*"):
	dest=os.path.join(os.getcwd(),os.path.basename(file))
	print file,dest
	if os.path.isfile(dest)==False:
		shutil.copy(file, dest)


	dest=os.path.join(dest_path,os.path.basename(file))
	if os.path.isfile(dest)==False:
		print file,dest
		shutil.copy(file, dest)

includes =['cairo', 'pango', 'pangocairo','atk', 'gobject', 'gio',"matplotlib.backends",  "matplotlib.backends.backend_tkagg"]
pack  =[  'gtk','gtk.keysyms']
setup(
		console=['opvdm.py'],
        options={
				"py2exe":{
				'packages': pack,
				"includes": includes}
				},
		data_files=matplotlib.get_py2exe_datafiles()
)

dist=os.path.join(os.getcwd(),"dist","etc")
if os.path.isdir(dist)==False:
	print "copying",dist
	shutil.copytree("c:\\Python27\\Lib\\site-packages\\gtk-2.0\\runtime\\etc", dist)

dist=os.path.join(dest_path,"lib")
if os.path.isdir(dist)==False:
	print "copying",dist
	shutil.copytree("c:\\Python27\\Lib\\site-packages\\gtk-2.0\\runtime\\lib", dist)

dist=os.path.join(dest_path,"share")
if os.path.isdir(dist)==False:
	print "copying",dist
	shutil.copytree("c:\\Python27\\Lib\\site-packages\\gtk-2.0\\runtime\\share", dist)


path_to_del=os.path.join(dest_path,"share","gtk-doc")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","locale")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","man")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)


path_to_del=os.path.join(dest_path,"share","doc")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","gtk-2.0")		#only contains a demo folder
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","pub","gui","dist","tcl","tk8.5","demos")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","pub","gui","dist","tcl","tk8.5","images")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","mpl-data","sample_data")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","icons","Tango","scalable")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)
