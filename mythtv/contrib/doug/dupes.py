#!/usr/bin/python

from MythTV import MythBE, Program, MythDB

titlesIgnore = ["Masterpiece Classic", "The Daily Show With Jon Stewart","Western Tradition"]

def uniq(seq, idfun=None):  
    # order preserving 
    if idfun is None: 
        def idfun(x): return x 
    seen = {} 
    result = [] 
    for item in seq: 
        marker = idfun(item) 
        # in old Python versions: 
        # if seen.has_key(marker) 
        # but in new ones: 
        if marker in seen: continue 
        seen[marker] = 1 
        result.append(item) 
    return result

def pendFun(x):
	out = x.title + "-"
	if x.subtitle is not None:
		out += x.subtitle
	return out

def goodRec(x):
	return x.recgroup <> 'Sports' and x.subtitle is not None and x.title not in titlesIgnore and (x.recstatus == Program.rsWillRecord or x.recstatus == Program.rsConflict) 

myth = MythBE()
mythDB = MythDB()
pending = uniq(filter(goodRec, myth.getPendingRecordings()), pendFun)

for p in sorted(pending, key=pendFun):
#	print p.title
	olds = list(mythDB.searchOldRecorded(title=p.title, subtitle=p.subtitle, recstatus=-3, duplicate=1))
#	olds = filter(lambda x: x.programid == "", olds)
	if len(olds) > 0:
		print "%s %s (%s) %s" % (p.title, p.subtitle, p.starttime, p.recstatus)
		for o in olds:
			if o.programid == "":
				print "\tNo Old Program Id"

			if o.programid <> "" and o.programid <> p.programid:
#			if o.programid <> p.programid:
				print "\tDifferent Program Ids"

			elif p.description <> o.description:
				print "\tDifferent Description"
			elif p.seriesid <> o.seriesid:
				print "\tDifferent Series Ids"
			else:
				print "\tOther"

		print ""

print "Pending shows: %d" % len(pending)
	
