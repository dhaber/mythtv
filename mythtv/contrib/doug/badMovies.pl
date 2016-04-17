#!/usr/bin/perl -w

use DBI;
use Config::Properties;

open my $fh, '<', 'data' or die "unable to open configuration file";
my $properties = Config::Properties->new();
$properties->load($fh);

$dbh = DBI->connect('dbi:mysql:mythconverg',$properties->getProperty("DBUserName"),$properties->getProperty("DBPassword"))
or die "Connection Error: $DBI::errstr\n";

$sql = "select distinct r.title, r.recordid, r.storagegroup, r.recgroup
from recordmatch m, record r, program p 
where r.recordid = m.recordid and m.chanid = p.chanid 
and m.starttime = p.starttime and p.category_type = 'movie' 
and (r.recgroup <> 'movies') 
and r.recgroup <> 'Kids'
and r.inactive = 0
and r.category <> 'Custom Recording' 
and r.title not in (
	'10 items or less','The Class', 'One Night Stand', 'Lost', 'The Original Kings of Comedy', 
	'Committed', 'Parenthood', 'Rules of Engagement', 'Without a Trace', 'Comedy\\'s Dirtiest Dozen',
	'Stella', 'The Conquerors', 'House', 'Unhitched', 'Happy Endings', 'Mildred Pierce', 'Ricky Gervais Live: Animals',
	'Sex and the city', 'Joey', 'Mad Love', 'Last Man Standing', 'Unforgettable', 'Camelot', 'Curious George',
	'Mary, Queen of Scots', 'Curious George 2: Follow That Monkey', 'Heroes', 'Come Fly With Me','Friends With Benefits',
	'Do Not Disturb','Winnie the Pooh: Springtime With Roo', 'Once Upon a Time','V','Anger Management','The Winner',
	'Archer','Revenge','Men At Work','Friends','How I Met Your Mother','The Walking Dead','About a Boy', 'Dracula',
	'Haven','The Bible') 
order by title;";

$sth = $dbh->prepare($sql);
$sth->execute
or die "SQL Error: $DBI::errstr\n";

printf "%-35.35s %7s %-8.8s %-8.8s\n", "TITLE", "ID", "STORAGE", "REC";
@recIds = ();
while (($title,$recordid,$storagegroup,$recgroup) = $sth->fetchrow_array) {
	printf "%-35.35s %7d %-8.8s %-8.8s\n", $title,$recordid,$storagegroup,$recgroup;
	push(@recIds, $recordid);
}

$s = join(", ", @recIds);
$sql = "update record set recgroup = 'Movies', storagegroup = 'Movies' where recordid in (" . $s . ");";

# print $sql . "\n";

if ($#ARGV > -1 && $ARGV[0] eq 'update') {
	print "running update\n";
	$sth = $dbh->prepare($sql);
	$sth->execute
		or die "SQL Error: $DBI::errstr\n";
}
