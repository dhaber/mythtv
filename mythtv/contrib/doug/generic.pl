#!/usr/bin/perl -w

use DBI;
use Config::Properties;

open my $fh, '<', 'data' or die "unable to open configuration file";
my $properties = Config::Properties->new();
$properties->load($fh);

$dbh = DBI->connect('dbi:mysql:mythconverg',$properties->getProperty("DBUserName"),$properties->getProperty("DBPassword"))
or die "Connection Error: $DBI::errstr\n";

$sql = "select distinct p.title 
from program p, programgenres g
where p.chanid = g.chanid and p.starttime = g.starttime and g.genre = 'Standup'
and p.starttime > now()
and p.generic <> 0
and p.title not in (select distinct o.title from oldrecorded o where o.title = p.title and o.recstatus = -3)
and p.title not in (select distinct title from record r where inactive <> 0)
and p.title not in ('Eddie Izzard: Live at Madison Square Garden', 'Cold Squad', 'Thou Shalt Laugh')
order by p.title";

$sth = $dbh->prepare($sql);
$sth->execute
or die "SQL Error: $DBI::errstr\n";

while (($title) = $sth->fetchrow_array) {
	printf "%s\n", $title;
}
