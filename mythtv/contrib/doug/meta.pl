#!/usr/bin/perl -w

use DBI;
use Config::Properties;

open my $fh, '<', 'data' or die "unable to open configuration file";
my $properties = Config::Properties->new();
$properties->load($fh);

$dbh = DBI->connect('dbi:mysql:mythconverg',$properties->getProperty("DBUserName"),$properties->getProperty("DBPassword"))
or die "Connection Error: $DBI::errstr\n";

printf "--------------------------TV RULES---------------------\n";
$sql = "select recordid,title,subtitle,recgroup,inetref,season,episode
from record
where (inetref = '' or season = 0)
and inactive = 0
and recgroup <> 'Movies'
and recordid not in (
	1833,1044,1047,736,898,1128,1326,1713,1903,672,1904,1680,1519,1737,1659,952,1774,189,1434,1619,
	1660,1276,1949,1322,835,1256,1123,1835,1620,1336,1999,1755,820,1042,836,1248,977,1991,1025,728,
	561,1545,1266,888,1260,1544,670,1277,941,1880,1278,1992,1367,1342,828,727,1510,1031,1077,1077,
	1059,1066,975,1661,1363,1894,843,636,1079,923,1918,1359,1360,767,1338,1246,1608,878,947,1721,
	1293,1294,1355,1292,1883,1884,1268,1269,1328,1919,976,1546,1864,1307,1908,1998,893,1264,1786,
	2000,1250,1738,1335,1814,1574,905,1994,1259,1863,955,1033,2001,894,1885,1590,1315,1126,1543,
	1065,462,458,1815,985,737,825,969,1297,757,1298,1997,1035,1258,983,1934,1858,1784,1936,1279,
        1308,1299,1300,1351,150,1339,2012,2013,2015,2016,2017,2018,2019,2021,2041,2072,2074,2079,2081,
	2083,2075,2088,2090,2098,2103,2107,2108,2111,2112,2113,2117,2119,2120,2122,2125,2123,2126,2127,
	2124,2140,2149,2150,2154,2162,2171,2172,2175,2176,2190,2207,2210,2205,2217,2204,2219,2223,2225,
	2229,2226,2233,2234,2227,2239,2242,2259,2269,2288,2289,2291
	)
order by title";

$sth = $dbh->prepare($sql);
$sth->execute
or die "SQL Error: $DBI::errstr\n";

printf "%-5.5s %-45.45s %-20.20s %-10.10s %-8.8s %-3.3s %-3.3s\n", "id","title","subtitle","group","inetref","season","episode";
while (($id,$title,$subtitle,$recgroup,$inetref,$season,$episode) = $sth->fetchrow_array) {
	printf "%-5.5s %-45.45s %-20.20s %-10.10s %-8.8s %-3.3s %-3.3s\n", $id,$title,$subtitle,$recgroup,$inetref,$season,$episode;
}


printf "-------------------------MOVIE RULES---------------------\n";
$sql = "select recordid,title,subtitle,recgroup,inetref,season,episode
from record
where (inetref = '' or season <> 0)
and recgroup = 'Movies'
and recordid not in (2084
	)
order by title";

$sth = $dbh->prepare($sql);
$sth->execute
or die "SQL Error: $DBI::errstr\n";

printf "%-5.5s %-45.45s %-20.20s %-10.10s %-8.8s %-3.3s %-3.3s\n", "id","title","subtitle","group","inetref","season","episode";
while (($id,$title,$subtitle,$recgroup,$inetref,$season,$episode) = $sth->fetchrow_array) {
	printf "%-5.5s %-45.45s %-20.20s %-10.10s %-8.8s %-3.3s %-3.3s\n", $id,$title,$subtitle,$recgroup,$inetref,$season,$episode;
}

printf "-------------------------RECORDED SHOWS---------------------\n";
$sql = "select recordid,title,subtitle,recgroup,inetref,season,episode,chanid,starttime
from recorded
where 	(
	inetref = '' 
	or ((recgroup = 'Movies' or substring(programid,1,2) = 'MV') and season <> 0) 
	or (recgroup <> 'Movies' and substring(programid,1,2) <> 'MV' and season = 0 and episode = 0)
	)
and recgroup <> 'LiveTV'
and recordid not in (1519,1047,1044,1999,1991,1266,670,1992,1883,1919,1864,2000,1814,2001,1315,1328,1863,1251,1920,
	1997,1718,1776,1917,1915,1901,1184,1867,1340,2012,2015,2018,2013,2016,2019,1990,2041,2081,2084,2079,767,2103,
	2107,2112,1033,2120,2122,2124,2127,636,2125,2119,2126,2123,1661,2117,2140,2149,2150,2154,2172,2162,2176,2171,
	2170,2190,2175,2207,2205,2217,2210,2225,2236,2226,2233,2242,2234
	)
and title not in ('Curious George: A Very Monkey Christmas', 'Elmo\\'s World: Happy Holidays!')
order by title";

$sth = $dbh->prepare($sql);
$sth->execute
or die "SQL Error: $DBI::errstr\n";

printf "%-5.5s %-20.20s %-45.45s %-10.10s %-8.8s %-3.3s %-3.3s\n", "id","title","subtitle","group","inetref","season","episode";
while (($id,$title,$subtitle,$recgroup,$inetref,$season,$episode,$chanid,$starttime) = $sth->fetchrow_array) {
	printf "%-5.5s %-20.20s %-45.45s %-10.10s %-8.8s %-3.3s %-3.3s mythmetadatalookup --chanid %s --starttime \"%s\"\n", $id,$title,$subtitle,$recgroup,$inetref,$season,$episode,$chanid,$starttime;
}
