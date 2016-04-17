#!/usr/bin/python

from MythTV import MythBE, Program
from datetime import timedelta, datetime, time

def willRec(x):
	return x.recstatus == Program.rsWillRecord and x.airdate is not None

myth = MythBE()
pending = filter(willRec, myth.getPendingRecordings())

print "%-20.20s %-35.35s %-20s %-20s" % ("title", "subtitle", "original date", "record start time")
for p in sorted(pending, key=lambda a: a.title):
	diff = p.starttime - datetime.combine(p.airdate, time(tzinfo=p.starttime.tzinfo))
	if diff > timedelta(days=1,hours=6) and diff < timedelta(days=24):
		print "%-20.20s %-35.35s %-20s %-20s" % (p.title, p.subtitle, p.airdate, p.starttime)

