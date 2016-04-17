#!/usr/bin/python

# Finds soccer games I want to watch but aren't being recorded

import sys, getopt
from MythTV import MythBE, Program, static
from datetime import timedelta, datetime, time
from subprocess import check_output
import pytz
from unidecode import unidecode

# return only sports recs
def sportsRecs(rec):
	return rec.recstatus in [Program.rsWillRecord] and rec.recgroup == "Sports"

# returns true if the line is a data output line
def filterOutput(line):
	return "c.f.s.s.i.PipeDelimitedPrinterService" in line

# converts a line to a data array
def getData(line):
	# get the actual content
        data = line.split(" : ")[1]

        # put the data into an array
        # [0] => datetime
        # [1] => league
        # [2] => away team
        # [3] => home team
        # [4] => channel
        data = data.split("|")

	# make the datetime a real python datetime
	est = pytz.timezone('US/Eastern')
	data[0] = est.localize(datetime.strptime(data[0], "%Y %b-%d %a %I:%M %p"))
	data[2] = cleanName(data[2])
	data[3] = cleanName(data[3])
	return data

# returns true if any of the pending recs match this data
def hasMatches(data, pending):
	#print "Searching for %s vs %s" % (data[2], data[3])
	for p in pending:
		if (hasMatch(data, p)):
			return True
	return False

def hasMatch(data, p):
	return isTimeClose(data, p) and areTeamsClose(data, p)

def isTimeClose(data, p):
	#print "checking time for %s" % (p.subtitle)

	# Get difference in seconds (absolute value)
	diff = abs((data[0] - p.recstartts).total_seconds())

        # make sure the start time is close
        return diff < timedelta(hours=hours).total_seconds()

def areTeamsClose(data, p):
	#print "checking close program subtitle %s" % (p.subtitle)
	if not p.subtitle:
		return False

	programTeams = teamsFromSubtitle(p.subtitle)
	#print "program teams %s" % (programTeams)
	#print "home: %s; away %s" % (data[2], data[3])
	return isTeamClose(data[2], programTeams) and isTeamClose(data[3], programTeams)


# removes bad characters and returns as an array
def cleanName(name):
	return unidecode(name).strip("0123456789").lower().replace(".", "").replace("fc","").split(" ")

def teamsFromSubtitle(subtitle):
	teams = subtitle.split(" vs. ")
	if len(teams) != 2:
		return [cleanName(subtitle)]
	teams[0] = cleanName(teams[0])
	teams[1] = cleanName(teams[1])
	return teams

def isPartMatch(a, b):
	for part in a:
		if not part in b:
			return False
	return True


def isTeamMatch(team, programTeam):
	return isPartMatch(team, programTeam) or isPartMatch(programTeam, team)

def isTeamClose(team, programTeams):
	#print "%s %s" % (team, subtitle)
	for programTeam in programTeams:
		if isTeamMatch(team, programTeam):
			return True
	return False

try:
    opts, args = getopt.getopt(sys.argv[1:],"h:",["hours="])
except getopt.GetoptError:
    print 'soccer.py -h <hours a recording can be late - default 1>'
    sys.exit(2)

hours = 1
for o, a in opts:
    if o == "-h":
        hours = int(a)


myth = MythBE()
pending = filter(sportsRecs, myth.getPendingRecordings())

# get the output from the schedule
output = check_output(["/usr/bin/java", "-jar", "/opt/soccer/soccerschedule.jar", "--weeks=2"])

# split the output into lines and then filter only the real output
lines = filter(filterOutput,output.splitlines())
for line in lines:
	data = getData(line)
	if not hasMatches(data, pending):
		print "%s %s vs. %s (%s) on %s " % (data[0], " ".join(data[3]), " ".join(data[2]), data[1], data[4])

