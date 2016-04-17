#!/usr/bin/perl -w

# find recordings rules that should be marked inactive
# because they record movies that I already watched and deleted

use DBI;
use Config::Properties;

open my $fh, '<', 'data' or die "unable to open configuration file";
my $properties = Config::Properties->new();
$properties->load($fh);

$dbh = DBI->connect('dbi:mysql:mythconverg',$properties->getProperty("DBUserName"),$properties->getProperty("DBPassword"))
or die "Connection Error: $DBI::errstr\n";

$sql = "
select r.title 
from record r, oldrecorded o 
where r.recgroup = 'Movies'
and r.title = o.title
and o.recstatus = -3 
and r.inactive = 0 
and o.duplicate = 1 
and (select count(*) from recorded d where d.title = r.title) = 0  
and r.title not in ('Beowulf', 'Marie Antoinette', 'Bananas', 'Spellbound')
order by r.title";

$sth = $dbh->prepare($sql);
$sth->execute
or die "SQL Error: $DBI::errstr\n";

#printf "Title\n";
while (($title) = $sth->fetchrow_array) {
	printf "%s\n",$title;
}
