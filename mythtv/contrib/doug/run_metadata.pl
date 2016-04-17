#!/usr/bin/perl -w

use DBI;
use POSIX qw/strftime/;
use Config::Properties;

open my $fh, '<', 'data' or die "unable to open configuration file";
my $properties = Config::Properties->new();
$properties->load($fh);

$dbh = DBI->connect('dbi:mysql:mythconverg',$properties->getProperty("DBUserName"),$properties->getProperty("DBPassword"))
or die "Connection Error: $DBI::errstr\n";

$sql = "
select title, chanid, date_format(starttime, '%Y%m%d%H%i%s')
from recorded
where title like '" . $ARGV[0] . "'
order by originalairdate";

$sth = $dbh->prepare($sql);
$sth->execute
or die "SQL Error: $DBI::errstr\n";

while (($title,$chanid,$starttime) = $sth->fetchrow_array) {	
	printf "Running: %s  %s %s \n",$title,$chanid,$starttime;
	system("/usr/local/bin/mythmetadatalookup --chanid $chanid --starttime $starttime");
}
