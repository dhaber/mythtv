#!/usr/bin/perl -w

use DBI;
use Getopt::Long;
use Config::Properties;

open my $fh, '<', 'data' or die "unable to open configuration file";
my $properties = Config::Properties->new();
$properties->load($fh);

sub lowFlags() {
	my($lowSecs, $highSecs, $minCount) = @_;

	$sql= "
		select p.title, p.subtitle, p.chanid, p.starttime, timediff(p.endtime, p.starttime) as t, count(*) as c
		from recordedmarkup m, recorded p, channel c, record d
		where m.chanid = p.chanid 
		and m.starttime = p.starttime 
		and c.chanid = m.chanid
		and d.recordid = p.recordid
		and d.autocommflag = 1
		and c.commmethod <> -2
		and m.type = 4 
		and time_to_sec(timediff(p.endtime, p.starttime)) < ? 
		and time_to_sec(timediff(p.endtime, p.starttime)) > ? 
		group by m.chanid, m.starttime 
		having c < ? 
		order by m.starttime desc;";

	$sth = $dbh->prepare($sql);
	$sth->execute(($highSecs, $lowSecs, $minCount)) or die "SQL Error: $DBI::errstr\n";

	$first = 0;
	while (($title,$subtitle,$chanid,$starttime,$len,$comms) = $sth->fetchrow_array) {
        	if ($first == 0) {
			printf "\n------------------------------------------------\n";
			printf "Shows between %s mins and %s mins with less than %s comms\n\n", ($lowSecs / 60), ($highSecs / 60), $minCount;
                	printf "%-35.35s %-25.25s %8s %5s %s\n", "TITLE", "SUBTITLE", "LENGTH", "COMMS", "COMMAND";
                	$first++;
        	}

        	printf "%-35.35s %-25.25s %8s %-5smythcommflag --queue --chanid %s --starttime \"%s\"  \n", $title, $subtitle, $len, $comms, $chanid, $starttime;
	}



}

$dbh = DBI->connect('dbi:mysql:mythconverg',$properties->getProperty("DBUserName"),$properties->getProperty("DBPassword"))
or die "Connection Error: $DBI::errstr\n";

$sql = "
select r.title, r.subtitle, c.chanid, r.starttime
from recorded r, channel c, record d 
where d.recordid = r.recordid and r.chanid = c.chanid 
and (
	select count(*) 
	from recordedmarkup m 
	where m.chanid = r.chanid 
	and m.starttime = r.starttime
	and m.type = 4) = 0 
and c.commmethod <> -2 
and d.autocommflag = 1 
and r.endtime < (now() + 5) 
order by r.title, r.subtitle;";

$sth = $dbh->prepare($sql);
$sth->execute
or die "SQL Error: $DBI::errstr\n";

printf "No Commercials Flagged But Should Have\n";
$first = 0;
while (($title,$subtitle,$chanid,$starttime) = $sth->fetchrow_array) {
	if ($first == 0) {
		printf "%-35.35s %-25.25s %s\n", "TITLE", "SUBTITLE", "COMMAND";
		$first++;
	}
	$title =~ s/'//g;
	$subtitle =~ s/'//g;
	printf "#%-35.35s %-25.25s\n                                                 mythcommflag --chanid %s --starttime \"%s\" --queue\n", $title, $subtitle, $chanid, $starttime;
}

GetOptions ("short"  => \$short);

if (!$short) {
 # ~30 mins
 &lowFlags(0, 2400, 3);

 # ~1 hour
 &lowFlags(2400, 4200, 5);

 # ~1.5 hours
 &lowFlags(4200, 6000, 6);

 # ~2 hours
 &lowFlags(6000, 7800, 7);

 # > ~2 hours
 &lowFlags(7800, 999999, 9);
}
