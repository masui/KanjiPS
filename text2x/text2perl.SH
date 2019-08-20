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
echo "Extracting text2perl (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >text2perl <<!GROK!THIS!
#!$perlloc
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>text2perl <<'!NO!SUBS!'
#
#	Convert a text file into a Perl program 
#	which outputs the same text.....
#
#			1989 3/30 MASUI Toshiyuki
#
eval "exec /usr/local/bin/perl -S $0 $*"
	if $Shell_cannot_understand; #!
($program) = ($0 =~ m#([^/]+)$#);
while(<>){
	chop;
	s/\\/\\\\/g;
	s/\t/\\t/g;
	s/\"/\\"/g;
	s/\$/\\$/g;
	print "print \"$_\\n\";\n";
}
!NO!SUBS!
chmod 755 text2perl
$eunicefix text2perl
