#!/usr/bin/python

# get the files to delete
# ./files.py | cut -f 1 -d " " | grep -v lock | grep var | xargs ls -lastrh

from MythTV import MythBE
import os

myth = MythBE()
allShows = myth.getRecordings()
dirs = ["/var/lib/mythtv/recordings", "/var/lib/mythtv/recordings2", "/var/lib/mythtv/recordings3", "/var/lib/mythtv/movies-group"]

showMap = {}
badFiles = []

def handleFile(dir,file):
	parts = file.split(".")
	key = parts[0]
	bad = False
	if key not in showMap:
		badFiles.append(os.path.join(dir,file))
		bad = True

		if parts[len(parts)-1] == "mpg":
			print key, file

def handleDir(dir):
	for file in os.listdir(dir):
		path = os.path.join(dir,file)
		if os.path.isdir(path):
			handleDir(path)
		else:
			handleFile(dir,file)

for show in allShows:
	key =  "{0}_{1}".format(show.chanid, show.recstartts.strftime("%Y%m%d%H%M%S"))
	showMap[key] = show;

for d in dirs:
	handleDir(d);


size = 0
for f in badFiles:
	s = os.path.getsize(f)
	print f,s
	size += s

print "%.2f MB" % ( size / 1024.0 / 1024.0 )
