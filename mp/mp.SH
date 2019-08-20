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
echo "Extracting mp (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >mp <<!GROK!THIS!
#!$perlloc -P
#
#	MP - Print a mail in a fascinating fashon...
#
#				1989 1/25 MASUI Toshiyuki
#				@ SHARP Computer Systems Lab.
#
eval "exec $perlloc -P -S \$0 \$*"
	if \$Shell_cannot_understand;
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>mp <<'!NO!SUBS!'
($program) = ($0 =~ m#([^/]+)$#);	# get the name of this program
#
$kanjifont = "jis32";	# default kanji font
#
chop($date = `date`);
$profile = "";
$eflag = 0;
while($_ = $ARGV[0], /^-/){ # get options
	shift;
	if(/^-p$/){ $profile = $ARGV[0]; shift; }
	elsif(/^-p(.*)/){ $profile = $1; }
	elsif(/^-f$/){ $kanjifont = $ARGV[0]; shift; }
	elsif(/^-f(.*)/){ $kanjifont = $1; }
	elsif(/^-E$/){ $eflag = 1; shift; }
	else {
	  print "$program --- Print a mail in a fascinating fashion\n\n";
	  print " Usage: $program [-p prologue] [-E] [mailfile]\n";
	  print "\n";
	  exit 0;
	}
}
if($profile ne ""){
	open(pro,$profile) || die "Can't open prologue file $profile";
	while(<pro>){
		print;
	}
}
else {
	do prologue();
}
$user = $ENV{'USER'};
print "/timenow ($date) def\n";
print "/title () def\n";
print "%%EndProlog\n";

#define START 0
#define DOHEADER 1
#define DOMESSAGE 2

#define PAGELENGTH 60

$state = START;
while(<>){
	if($. == 1){ # head of each file
		$pn = 1;
		do startpage();
	}
	chop;
	if($state == START){
		if(/^From:/ || /^Date:/ || /^Subject:/ || /^To:/ || /^Cc:/){
			$state = DOHEADER;
		}
	}
	if($state == DOMESSAGE){
		do textshow();
	}
	if($state == DOHEADER){
		if(/^From:/ || /^Date:/ || /^Subject:/){
			do mixshow();
			$more = 0;
		}
		elsif(/^To:/ || /^Cc:/){
			print "/title ",$user eq "" ? "()" :
				"(Mail for $user)", " def\n";
			do mixshow();
			$more = 1;
		}
		elsif(/^Newsgroups:\s*(.*)/){
			print "/title (Article in $1) def\n";
		}
		elsif(/^\s/ && $more && ! /^\s*$/){
			do textshow();
		}
		elsif(/^$/){
			$state = DOMESSAGE;
			print "() showline\n";
		}
		else {
			$more = 0;
		}
	}
	if(eof){
		close(ARGV);	# reset line numbering
		do endpage();
		$state = START;
	}
}
do epilogue();
exit 0;
#
#	Subroutines
#
sub startpage {
	$lin = 0;
	print "%%Page: ? $pn\n";
	print "save /pagesave exch def\n";
	print "newpage\n";
}
sub endpage {
	print "(",$pn++,") endpage\n";
	print "pagesave restore\n";
}
sub useline {
	if(++$lin > PAGELENGTH){
		do endpage();
		do startpage();
	}
}
sub textshow {
	do useline();
	print do kexpand($_), "() showline\n";
}
sub boldshow {
	do useline();
	print "Bold (", do expand($_), ") showline\n";
}
sub mixshow {
	do useline();
	/(\S+)\s(.*)/ ;
	$f1 = $1; $f2 = $2;
	if(/^Subject:/){
		print "/subject (", do expand($f2), ") def\n";
	}
	print "Bold ($f1) show Roman ( ", do expand($f2), ") showline\n";
}
sub expand {
	local($str) = @_;
	local($r) = '';
	$_ = $str;
	while(! /^$/){
		if(s/^[^\033\t\200-\377]+//){
			$s = $&;
			$s =~ s/\\/\\\\/g;
			$s =~ s/\(/\\(/g;
			$s =~ s/\)/\\)/g;
			$r .= $s;
		}
		elsif(s/^\t+//){
			$r .= $&;
		}
		elsif(s/^([\200-\377].)+//){
			$r .= $eflag ? do ujis2jis($&) : do sjis2jis($&) ;
		}
		elsif(s/^\033\$.([^\033].)*\033\(.//){
			$r .= $&;
		}
		else {
			s/.//;
			$r .= sprintf("[%02x]",ord($&) & 0xff);
		}
	}
	$_ = $r;	# return value
}
sub kexpand {
	local($str) = @_;
	local($r) = '';
	$_ = $str;
	while(! /^$/){
		if(s/^[^\033\t\200-\377]+//){
			$s = $&;
			$s =~ s/\\/\\\\/g;
			$s =~ s/\(/\\(/g;
			$s =~ s/\)/\\)/g;
			$r .= "($s) A ";
		}
		elsif(s/^\t+//){
			$r .= sprintf("%d T ",length($&));
		}
		elsif(s/^([\200-\377].)+//){
			$r .= sprintf("D (%s) J U ",
				$eflag ? do ujis2jis($&) : do sjis2jis($&));
		}
		elsif(s/^\033\$.([^\033].)*\033\(.//){
			$r .= "D ($&) J U ";
		}
		else {
			s/.//;
			$r .= sprintf("([%02x]) A ",ord($&) & 0xff);
		}
	}
	$_ = $r;
}
sub sjis2jis {
	local($result) = '';
	local($_) = @_;
	while(s/^(.)(.)//){
		$c1 = ord($1);
		$c2 = ord($2);
		$c1 += 0x100 if $c1 < 0;
		$c2 += 0x100 if $c2 < 0;
		$c1 -= 0x40 if $c1 >= 0xe0;
		$c2-- if $c2 >= 0x80;
		$j1 = ($c1-0x81) * 2 + ($c2>=0x9e ? 1 : 0) + 0x21;
		$j2 = ($c2 >= 0x9e ? $c2-0x9e : $c2-0x40) + 0x21;
		$result .= sprintf("%c%c",$j1,$j2);
	}
	"\033" . '$B' . $result . "\033" . '(B';
}
sub ujis2jis {
	local($result) = '';
	local($_) = @_;
	while(s/^(.)(.)//){
		$result .= sprintf("%c%c",ord($1) & 0x7f,ord($2) & 0x7f);
	}
	"\033" . '$B' . $result . "\033" . '(B';
}
sub prologue {
	print "%!PS-Adobe-1.0\n";
	print "%%Creator: Steve Holden\n";
	print "%%Modifed: Rich Burridge\n";
	print "%%Modifed: MASUI Toshiyuki\n";
	print "%%Title: MP\n";
	print "%%CreationDate: $date\n";
	print "%%DocumentFonts: Times-Bold Times-Roman Courier $kanjifont\n";
	print "%%Pages: (atend)\n";
	print "%%EndComments\n";
	print "%\n\n";
	print "/subject () def\n";
	print "/font1d /Times-Bold findfont 10 scalefont def\n";
	print "/font2d /Times-Roman findfont 10 scalefont def\n";
	print "/font3d /Courier findfont [6.55 0 0 9 0 0 ] makefont def\n";
	print "/fontHd /Helvetica-BoldOblique findfont 15 scalefont def\n";
	print "/fontH2 /Helvetica-BoldOblique findfont 10 scalefont def\n";
	print "/fontNd /Times-Bold findfont 12 scalefont def\n";
	print "/fontJ /$kanjifont findfont 7.86 scalefont def\n";
	print "\n";
	print "/Bold {font1d setfont} def\n";
	print "/Roman {font2d setfont} def\n";
	print "/Courier {font3d setfont} def\n";
	print "/fontH {fontHd setfont} def\n";
	print "/fontD {fontH2 setfont} def\n";
	print "/fontN {fontNd setfont} def\n";
	print "/Kanji {fontJ setfont} def\n";
	print "\n";
	print "/endpage\n";
	print "{\n";
	print "    gsave\n";
	print "    fontH\n";
	print "    newpath\n";
	print "    90 756 moveto\n";
	print "    100 736 10 180 270 arc\n";
	print "    536 736 10 270 0 arc\n";
	print "    536 756 10 0 90 arc\n";
	print "    100 756 10 90 180 arc\n";
	print "    closepath 0.75 setgray fill\n";
	print "    newpath\n";
	print "    318 746 15 0 360 arc\n";
	print "    gsave 1 setgray fill grestore\n";
	print "    closepath\n";
	print "    0 setgray stroke\n";
	print "    100 740 moveto\n";
	print "    title show\n";
	print "    fontD\n";
	print "    timenow stringwidth pop neg 536 add 740 moveto timenow show\n";
	print "    fontN\n";
	print "    dup stringwidth pop 2 div neg 318 add 740 moveto show\n";
	print "    fontH\n";
	print "    newpath\n";
	print "    90 60 moveto\n";
	print "    100 40 10 180 270 arc\n";
	print "    536 40 10 270 0 arc\n";
	print "    536 60 10 0 90 arc\n";
	print "    100 60 10 90 180 arc\n";
	print "    closepath 0.75 setgray fill\n";
	print "    0 setgray\n";
	print "    100 44 moveto subject show\n";
	print "    grestore\n";
	print "    showpage\t\t% display it\n";
	print "} bind def\n";
	print "\n";
	print "/newpage\n";
	print "{ /lct 0 def\n";
	print "  /ypos 700 def\n";
	print "  100 ypos moveto\n";
	print "} bind def\n";
	print "\n";
	print "/showline { show /ypos ypos 10 sub def 100 ypos moveto } bind def\n";
	print "\n";
	print "/A { Courier show } bind def\n";
	print "/J { Kanji show } bind def\n";
	print "/U { 0 0.786 rmoveto } bind def\n";
	print "/D { 0 -0.786 rmoveto } bind def\n";
	print "Courier (ABCDabcd) stringwidth pop /TABWIDTH exch def\n";
	print "/T { currentpoint exch 100 sub 3 -1 roll\n";
	print "\tTABWIDTH mul add 0.01 add TABWIDTH div truncate\n";
	print "\tTABWIDTH mul 100 add exch moveto\n";
	print "} bind def\n";
#	print "-20 30 translate\n";
}
sub epilogue {
	print "%%Trailer\n";
	print "%%Pages: $pn\n";
}
!NO!SUBS!
chmod 755 mp
$eunicefix mp
