#!/usr/bin/perl -w

use DBI;

$dbh = DBI->connect('dbi:mysql:mythconverg',$properties->getProperty("DBUserName"),$properties->getProperty("DBPassword"))
or die "Connection Error: $DBI::errstr\n";

$sql = "
select r.title, r.subtitle, c.chanid, r.starttime
from recorded r, channel c, record d 
where d.recordid = r.recordid and r.chanid = c.chanid 
and (
	select count(*) 
	from recordedseek m 
	where m.chanid = r.chanid 
	and m.starttime = r.starttime
     ) = 0 
and c.commmethod <> -2 
and d.autocommflag = 1 
order by r.title, r.subtitle;";

$sth = $dbh->prepare($sql);
$sth->execute
or die "SQL Error: $DBI::errstr\n";

while (($title,$subtitle,$chanid,$starttime) = $sth->fetchrow_array) {
	@args = ("mythcommflag", "--rebuild", "--chanid", $chanid, "--starttime", $starttime);
	system(@args);
#	printf "mythcommflag --rebuild --chanid %s --starttime \"%s\"  \n", $chanid, $starttime;
}
