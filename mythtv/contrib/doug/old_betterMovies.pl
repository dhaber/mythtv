#!/usr/bin/perl -w

use DBI;
use Config::Properties;

open my $fh, '<', 'data' or die "unable to open configuration file";
my $properties = Config::Properties->new();
$properties->load($fh);

$dbh = DBI->connect('dbi:mysql:mythconverg',$properties->getProperty("DBUserName"),$properties->getProperty("DBPassword"))
or die "Connection Error: $DBI::errstr\n";

$sql = "select distinct r.title, r.recordid, rc.channum, rc.callsign, c.channum, c.callsign, rc.commmethod, c.commmethod, rp.videoprop
from recorded r, recordedprogram rp, channel rc, channel c, program p 
where 
	r.progstart = rp.starttime 
	and r.chanid = rp.chanid 
	and rc.chanid = r.chanid
	and p.title = rp.title
	and c.chanid = p.chanid
	and rp.category_type = 'movie'
	and p.category_type = 'movie'
#	and rc.channum != c.channum
#	and (rc.channum - 500) != c.channum
	and p.starttime > now()
	and  (
		( rc.channum < 500 )
		or 
		(rc.commmethod = -1)
	)
	order by rc.commmethod, r.title, rc.channum, c.channum
;";

#		( rc.channum < 500 and c.channum > 500)
#		or 
#		(rc.commmethod = -1 and c.commmethod = -2)


#		or
#		(rc.commmethod = -1)

$sth = $dbh->prepare($sql);
$sth->execute
or die "SQL Error: $DBI::errstr\n";

printf "%-35.35s %7s %8.8s %14.12s\n", "TITLE", "ID", "NEW", "ORIGINAL";
while (($title,$recordid,$ochannum,$ocallsign,$nchannum,$ncallsign,$ocommmethod,$ncommmethod,$ovideoprop) = $sth->fetchrow_array) {
	printf "%-35.35s %7d %4.4s(%-7.7s)%s %4.4s(%s)%s %s\n", $title,$recordid,$nchannum,$ncallsign,$ncommmethod,$ochannum,$ocallsign,$ocommmethod, $ovideoprop;
}

