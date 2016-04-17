#!/usr/bin/perl -w

use DBI;
use Config::Properties;

open my $fh, '<', 'data' or die "unable to open configuration file";
my $properties = Config::Properties->new();
$properties->load($fh);

$dbh = DBI->connect('dbi:mysql:mythconverg',$properties->getProperty("DBUserName"),$properties->getProperty("DBPassword"))
or die "Connection Error: $DBI::errstr\n";

$sql = "select r.title,
count(d.title),
maxepisodes from record r
left join recorded d on r.recordid = d.recordid
where maxepisodes > 0
group by r.recordid
order by r.title asc;";

$sth = $dbh->prepare($sql);
$sth->execute
or die "SQL Error: $DBI::errstr\n";

printf "%-35s %-7s %-3s %-4s\n", "TITLE", "CURRENT", "MAX", "FULL";
while (($title,$current,$max) = $sth->fetchrow_array) {
	$full = $current == $max ? "*" : "";
	printf "%-35s %7d %3d %4s\n", $title,$current,$max, $full;
}

