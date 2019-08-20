case $CONFIG in
'')
    if test ! -f config.sh; then
	ln ../config.sh . || \
	ln ../../config.sh . || \
	ln ../../../config.sh . || \
	(echo "Can't find config.sh."; exit 1)
    fi
    . config.sh
    ;;
esac
: This forces SH files to create target in same directory as SH file.
: This is so that make depend always knows where to find SH derivatives.
case "$0" in
*/*) cd `expr X$0 : 'X\(.*\)/'` ;;
esac
echo "Extracting text2c (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >text2c <<!GROK!THIS!
#!$perlloc
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>text2c <<'!NO!SUBS!'
#
#	Convert a text file into a C program 
#	which outputs the same text.....
#
#			1989 2/16 MASUI Toshiyuki
#
eval "exec /usr/local/bin/perl -S $0 $*"
	if $Shell_cannot_understand; #!

$0 =~ /[^\/]+$/ ;	# get the name of this program
$program = $&;
$oflag = 0;
$func = "main";
$outfile = "stdout";
while($_ = $ARGV[0], /^-/){ # get options
	shift;
	if(/^-f$/){ $func = $ARGV[0]; shift; }
	elsif(/^-f(.*)/){ $func = $1; }
	elsif(/^-o$/){ $oflag = 1; $outfile = $ARGV[0]; shift; }
	elsif(/^-o(.*)/){ $oflag = 1; $outfile = $1; }
	else{
		print stderr "$program: convert text to C program...\n";
		print stderr "  Usage: $program " . 
			"[ -f funcname ] [ -o outfp ] { textfile }\n";
		exit 0;
	}
}
print "#include <stdio.h>\n";
print "extern FILE *$outfile;\n" if $oflag;
print "static char *outstr[] = {\n";
while(<>){
	chop;
	s/\\/\\\\/g;
	s/\t/\\t/g;
	s/\"/\\"/g;
	print "\t\"$_\\n\",\n";
}
print "\t0\n";
print "};\n";
print "$func()\n";
print "{\n";
print "\tregister char **s;\n";
print "\tfor(s = outstr; *s; s++){\n";
print "\t\tfprintf($outfile,\"%s\",*s);\n";
print "\t}\n";
print "}\n";
!NO!SUBS!
chmod 755 text2c
$eunicefix text2c
