#!/usr/bin/env python

from MythTV import MythDB, MythBE, Recorded, MythError
from socket import timeout

import os
import sys

def human_size(s):
    s = float(s)
    o = 0
    while s > 1000:
        s /= 1000
        o += 1
    return str(round(s,1))+('B ','KB','MB','GB')[o]

class File( str ):
    def __new__(self, host, group, path, name, size):
        return str.__new__(self, name)
    def __init__(self, host, group, path, name, size):
        self.host = host
        self.group = group
        self.path = path
        self.size = int(size)
    def pprint(self):
        name = u'%s: %s' % (self.host, os.path.join(self.path, self))
        print u'  {0:<90}{1:>8}'.format(name, human_size(self.size))
    def delete(self):
        be = MythBE(self.host, db=DB)
        be.deleteFile(self, self.group)

class MyRecorded( Recorded ):
    _table = 'recorded'
    def pprint(self):
        name = u'{0.hostname}: {0.title}'.format(self)
        if self.subtitle:
            name += u' - '+self.subtitle
        print u'{2}|{0}|{1}'.format(name,self.basename,self.recordedid)

def printrecs(title, recs):
    print title
    i = 0;
    for rec in sorted(recs, key=lambda x: x.title):
	i = i + 1
	print i
	with DB as c:
	    rec.pprint()
            c.execute("""UPDATE recorded set duplicate = 0 where recordedid=%s""",(rec.recordedid))
	    #r = c.fetchone()
            #print r
            c.execute("""UPDATE oldrecorded set duplicate = 0 where title=%s and subtitle=%s and description=%s""",(rec.title, rec.subtitle, rec.description))
	    #o = c.fetchall()
#            if (len(o) == 0):
	    #print len(o)
#            print u'{0}-{1}-{2}'.format(o[0],o[1],o[2])
    print u'count:{0}'.format(len(recs))


def printfiles(title, files):
    print title
    for f in sorted(files, key=lambda x: x.path):
        f.pprint()
    size = sum([f.size for f in files])
    print u'{0:>88}{1:>12}'.format('Total:',human_size(size))

def populate(host=None):
    unfiltered = []
    kwargs = {'livetv':True}
    recs = list(DB.searchRecorded(**kwargs))

    if host:
        with DB as c:
            c.execute("""SELECT count(1) FROM settings
                         WHERE hostname=%s AND value=%s""",
                        (host, 'BackendServerIP'))
            if c.fetchone()[0] == 0:
                raise Exception('Invalid hostname specified on command line.')
        hosts = [host]
        kwargs['hostname'] = host
    else:
        with DB as c:
            c.execute("""SELECT hostname FROM settings
                         WHERE value='BackendServerIP'""")
            hosts = [r[0] for r in c.fetchall()]
    for host in hosts:
        for sg in DB.getStorageGroup():
            if sg.groupname in ('Videos','Banners','Coverart',\
                                'Fanart','Screenshots','Trailers'):
                continue
            try:
                dirs,files,sizes = BE.getSGList(host, sg.groupname, sg.dirname)
                for f,s in zip(files,sizes):
                    newfile = File(host, sg.groupname, sg.dirname, f, s)
                    if newfile not in unfiltered:
                        unfiltered.append(newfile)
            except:
                pass


    zerorecs = []
    orphvids = []
    for rec in list(recs):
        if rec.basename in unfiltered:
            recs.remove(rec)
            i = unfiltered.index(rec.basename)
            f = unfiltered.pop(i)
            if f.size < 1024:
                zerorecs.append(rec)
            name = rec.basename.rsplit('.',1)[0]
            for f in list(unfiltered):
                if name in f:
                    unfiltered.remove(f)
    for f in list(unfiltered):
        if not (f.endswith('.mpg') or f.endswith('.nuv')):
            continue
        orphvids.append(f)
        unfiltered.remove(f)

    orphimgs = []
    for f in list(unfiltered):
        if not f.endswith('.png'):
            continue
        orphimgs.append(f)
        unfiltered.remove(f)

    dbbackup = []
    for f in list(unfiltered):
        if 'sql' not in f:
            continue
        dbbackup.append(f)
        unfiltered.remove(f)

    return (recs, zerorecs, orphvids, orphimgs, dbbackup, unfiltered)

def delete_recs(recs):
    printrecs('The following recordings will be deleted', recs)
    print 'Are you sure you want to continue?'
    try:
        res = raw_input('> ')
        while True:
            if res == 'yes':
                for rec in recs:
                    rec.delete(True, True)
                break
            elif res == 'no':
                break
            else:
                res = raw_input("'yes' or 'no' > ")
    except MythError:
        name = u'{0.hostname}: {0.title}'.format(rec)
        if rec.subtitle:
            name += ' - '+rec.subtitle
        print "Warning: Failed to delete '" + name + "'"
    except KeyboardInterrupt:
        pass
    except EOFError:
        sys.exit(0)

def delete_files(files):
    printfiles('The following files will be deleted', files)
    print 'Are you sure you want to continue?'
    try:
        res = raw_input('> ')
        while True:
            if res == 'yes':
                for f in files:
                    f.delete()
                break
            elif res == 'no':
                break
            else:
                res = raw_input("'yes' or 'no' > ")
    except KeyboardInterrupt:
        pass
    except EOFError:
        sys.exit(0)

def main(host=None):
    recs, zerorecs, orphvids, orphimgs, dbbackup, unfiltered = populate(host)
    printrecs("Recordings with missing files", recs)

DB = MythDB()
BE = MythBE(db=DB)
DB.searchRecorded.handler = MyRecorded
DB.searchRecorded.dbclass = MyRecorded

if __name__ == '__main__':
    if len(sys.argv) == 2:
        main(sys.argv[1])
    else:
        main()
