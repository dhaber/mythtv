#!/usr/bin/python

import os
import shutil
import sys
from datetime import datetime
from MythTV import MythBE

d="/var/lib/mythtv/movies-group/foxx/"
dirs=[
	"/var/lib/mythtv/movies-group/local/",
	"/var/lib/mythtv/movies-group/biggie/",
	"/var/lib/mythtv/movies-group/2pac/",
	"/var/lib/mythtv/tv-group/biggie/",
	"/var/lib/mythtv/tv-group/2pac/",
	"/var/lib/mythtv/tv-group/local/"]

def get_dir(file):
	for dir in dirs:
		path = dir + file
		if os.path.exists(path):
			return path
	return False

def pretty_filesize(bytes):
	bytes = float(bytes)
	if bytes >= 1099511627776:
		terabytes = bytes / 1099511627776
		size = '%.2fT' % terabytes
	elif bytes >= 1073741824:
		gigabytes = bytes / 1073741824
		size = '%.2fG' % gigabytes
	elif bytes >= 1048576:
		megabytes = bytes / 1048576
		size = '%.2fM' % megabytes
	elif bytes >= 1024:
		kilobytes = bytes / 1024
		size = '%.2fK' % kilobytes
	else:
		size = '%.2fb' % bytes
	return size

def cmpFileSize(a,b):
	return a.filesize - b.filesize

def filterMovies(a):
	filename = a.filename.split("/")[3]
	return 	a.recgroup == "Movies" and  \
		get_dir(filename)

def getIds(input):
	groups = input.split(",")
	items = []
	for i in groups:
		seq = i.split("-")
		if len(seq) == 1:
			items.append(int(i))
		elif len(seq) == 2:
			items.extend(xrange(int(seq[0]),int(seq[1])+1))
		else:
			raise InputError("bad input")
	# remove dupes
	items =  list(set(items))

	return items
	

def moveMovies(ids, progs):
	movies = []
	for i in ids:
		movies.append(progs[i])
	
	doMoveMovies(movies)

def doMoveMovies(movies):
	print "You selected the following movies:"
	for i in movies:
		doMoveMovie(i, True)

	input = raw_input("Move them (yN)? ")
	if input == "y":
		for i in movies:
			doMoveMovie(i, False)
	
def doMoveMovie(movie, simulate):
	filename = movie.filename.split("/")[3]

	src = get_dir(filename)

	if not src:
		print "Can't find src for %s" % movie.title
		return

	dst = d + filename
	if not simulate:
		print "before: %s" % datetime.now()

	print "%-20.20s: moving from %s to %s" % (movie.title,src, dst)

	if not simulate:
		shutil.move(src,dst)
		print "after: %s" % datetime.now()


myth = MythBE()
input = ""
progs = filter(filterMovies, myth.getRecordings())
progs = sorted(progs,cmpFileSize)

totalSize = 0
for i,p in enumerate(progs):
	totalSize += p.filesize
	print "%2d: %-35.35s (%-5s) %s" % (i,p.title,pretty_filesize(p.filesize),get_dir(p.filename.split("/")[3]))
print "Total size: %s" % pretty_filesize(totalSize)
input = raw_input("Enter some numbers or 'q' : ")
moveMovies(getIds(input),progs)
