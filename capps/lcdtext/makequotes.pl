

open QUOTES, "<quotes.txt" or die "quotes.txt: $!";

sub doArray()
{
	print "const char * const quotes[] = \n{\n";
	$first = 1;
	while ( <QUOTES> )
	{
		chomp;
		next if /^#.*/;
		print "\t\"$_\",\n";
	}
	print "\t(const char *)0\n";
	print "};\n";
}

doArray();

