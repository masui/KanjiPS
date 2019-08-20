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
echo "Extracting jenscript (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >jenscript <<!GROK!THIS!
#!$perlloc -P
#
#	Convert Text Files into PostScript Format
#
#			1989 3/28 MASUI Toshiyuki
#
eval "exec $perlloc -P -S \$0 \$*"
	if \$Shell_cannot_understand;
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>jenscript <<'!NO!SUBS!'
($program) = ($0 =~ m#([^/]+)$#);

$mflag = $gflag = $xflag = $nflag = $sflag = $dflag = $eflag = $kflag = 0;
$columns = 1;
$curcolumn = 0;
$printpage = 0;
$lines = 0;
$fontsize = 6.0;
chop($date = `date`);
$page = 0;
$language = "";
$posx = $posy = 0;
$outbuf = "";
@aabv = ("N", "C", "S", "L", "K");
$# = "%.2f";

while($_ = $ARGV[0], /^-/){ # get options
	shift;
	s/^-//;
	while(! /^$/){
		study;
		if(s/^M//){ $mflag = 1; }
		elsif(s/^G//){ $gflag = 1; }
		elsif(s/^x//){ $xflag = 1; }
		elsif(s/^n//){ $nflag = 1; }
		elsif(s/^s//){ $sflag = 1; }
		elsif(s/^d//){ $dflag = 1; }
		elsif(s/^E//){ $eflag = 1; }
		elsif(s/^k//){ $kflag = 1; }
		elsif(s/^(\d)//){ $columns = $1; }
		elsif(/^f$/){ $kfont = $ARGV[0]; shift; last; }
		elsif(/^f(.+)/){ $kfont = $1; last; }
		elsif(/^b$/){ $header = $ARGV[0]; shift; last; }
		elsif(/^b(.+)/){ $header = $1; last; }
		elsif(/^l$/){ $language = $ARGV[0]; shift; last; }
		elsif(/^l(.+)/){ $language = $1; last; }
		elsif(/^F$/){ $afont = $ARGV[0]; shift; last; }
		elsif(/^F(.+)/){ $afont = $1; last; }
		elsif(/^S$/){ $fontsize = $ARGV[0]; shift; last; }
		elsif(/^S(.+)/){ $fontsize = $1; last; }
		elsif(/^P$/){ $printpage = $ARGV[0]; shift; last; }
		elsif(/^P(.+)/){ $printpage = $1; last; }
		elsif(/^L$/){ $lines = $ARGV[0]; shift; last; }
		elsif(/^L(.+)/){ $lines = $1; last; }
		else {
		  print stderr "$program --- Convert Text Files into PostScript\n";
		  exit 0;
		}
	}
}
($normalfont, $keywordfont, $commentfont, $stringfont, $literalfont) = 
$afont ne "" ? ($afont, $afont, $afont, $afont, $afont) :
$xflag ? ("Courier", "Courier-Bold", "Courier-Oblique", "Courier", "Courier") :
  ("Times-Roman", "Times-Bold", "Times-Italic", "Times-Roman", "Times-Roman");

($height, $width, $offsetx, $offsety) = 
	$sflag ? (650, 440, 120, 150) : # B5
#		 (730, 550, 60, 60);	# A4
		 (700, 550, 60, 40);	# letter
$linepitch = $lines ? $height / $lines : $fontsize * 7.0 / 6.0;
($kfont, $jfontsize) = $kfont ne "" ? ($kfont, $fontsize) :
			$fontsize < 5 ? ("jis16", 3.84) :
			$fontsize < 7 ? ("jis24", 5.76) :
			$fontsize < 12 ? ("jis32", 7.68) :
			("jis50", 12) ;
$_ = $kfont;
$texfont = /^jisj/ || /^dmj/ || /^dgj/ ;

#define NORMAL 0
#define COMMENT 1
#define STRING 2
#define LITERAL 3
#define KEYWORD 4
$mode = $outmode = NORMAL;

#define VGRINDEFS "/usr/lib/vgrindefs"
if(! open(vdef,VGRINDEFS)){
	$language = "";
}
if($language){
	#
	#	Read Language Information from VGRINDEFS
	#
	$line = "";
	while(<vdef>){
		next if /^#/;
		chop;
		s/^\s+//;
		$line .= $_;
		if(/\\$/){
			$line =~ s/\\$//;
			$line .= " ";
			next;
		}
		$_ = $line;
		$line = "";
		s/\\\\/<ESC>/g;
		s/\\:/<COLON>/g;
		split(/:/);
		$pat = $_[0];
		shift(@_);
		next if $language !~ /^($pat)$/;
		$pb = $bb = $be = $cb = $ce = $sb =
			$se = $lb = $le = $kw = "CANTMATCH!#$%&!";
		foreach $i (@_){
			$_ = $i;
			s/<ESC>/\\/g;
			s/<COLON>/:/g;
			if(/^pb=(.*)/){ $pb = $1; }
				if(/^bb=(.*)/){ $bb = $1; }
			if(/^be=(.*)/){ $be = $1; }
			if(/^cb=(.*)/){ $cb = $1; }
			if(/^ce=(.*)/){ $ce = $1; }
			if(/^sb=(.*)/){ $sb = $1; }
			if(/^se=(.*)/){ $se = $1; }
			if(/^lb=(.*)/){ $lb = $1; }
			if(/^le=(.*)/){ $le = $1; }
			if(/^kw=(.*)/){ $kw = $1; }
			if(/^tl/){ $tl = 1; }
			if(/^oc/){ $oc = 1; }
		}
		# correct regular expressions
		$cb = '\/\*' if $cb eq '/*';
		$ce = '\*\/' if $ce eq '*/';

		foreach $keyword (split(/\s+/,$kw)){
			$iskeyword{$keyword} = 1;
		}

		if(index($le,"\\e") == 0){
			$lesc = 1;
			$le =~ s/^..//;
		}
		if(index($se,"\\e") == 0){
			$sesc = 1;
			$se =~ s/^..//;
		}
		$ctop = $ltop = $stop = 0;
		$ctop = 1 if $cb =~ /^\^/;
		$ltop = 1 if $lb =~ /^\^/;
		$stop = 1 if $sb =~ /^\^/;
	}
}

#
#	Process Text
#
do prologue();
while(<>){
	do beginpage(0) if $. == 1;
	goto nextline if $printpage && $printpage != $page;
	chop;
	for($top = 1; ! /^$/; $top = 0){
		if($language eq ""){
			do output($_,NORMAL);
			last;
		}
		else {
			print $_,"\n" if $dflag;
			study;
			if($mode == NORMAL){
				$done = 0;
				if(/^($cb)(.*)$/ && (! $ctop || $top)){
					do output($1,COMMENT);
					s/^($cb)//;
					$mode = COMMENT;
					$done = 1;
				}
				elsif(/^($sb)(.*)/ && (! $stop || $top)){
					s/^($sb)//;
					$stringstr = $1;
					$mode = STRING;
					$done = 1;
				}
				elsif(/^($lb)(.*)/ && (! $ltop || $top)){
					s/^($lb)//;
					$literalstr = $1;
					$mode = LITERAL;
					$done = 1;
				}
				else { # keyword? 
					/^(\W?\w*)(.*)/;
					if($iskeyword{$1}){
						do output($1,KEYWORD);
						$_ = $2;
						next;
					}
				}
				if(! $done){
					s/^(\033\$.([^\033].)*\033\(.|([\200-\377].)+|\w+|[^\w\200-\377])//;
					do output($1,NORMAL);
					next;
				}
			}
			if($mode == COMMENT){
				if(/^.*$ce/){
					do output($&,COMMENT);
					do skip($&);
					$mode = NORMAL;
				}
				else{
					do output($_,COMMENT);
					last;
				}
			}
			elsif($mode == STRING){
				if(s/^$se//){
					do output($stringstr.$&,STRING);
					$stringstr = "";
					$mode = NORMAL;
				}
				elsif(s/^\033\$.([^\033].)*\033\(.//){
					$stringstr .= $&;
				}
				elsif($sesc && index($_,"\\") == 0){
					if(s/^\\.//){
						$stringstr .= $&;
					}
					elsif(s/^.//){
						$stringstr .= $&;
					}
				}
				elsif(s/^.//){
					$stringstr .= $&;
				}
				if(/^$/){
					do output($stringstr,LITERAL);
					$stringstr = "";
				}
			}
			elsif($mode == LITERAL){
				if(s/^$le//){
					do output($literalstr.$&,LITERAL);
					$literalstr = "";
					$mode = NORMAL;
				}
				elsif(s/^\033\$.([^\033].)*\033\(.//){
					$literalstr .= $&;
				}
				elsif($lesc && index($_,"\\") == 0){
					if(s/^\\.//){
						$literalstr .= $&;
					}
					elsif(s/^.//){
						$literalstr .= $&;
					}
				}
				elsif(s/^.//){
					$literalstr .= $&;
				}
				if(/^$/){
					do output($literalstr,LITERAL);
					$literalstr = "";
				}
			}
			else {
				next;
			}
		}
	}
nextline:
	do flush();
	$posx = 0;
	if(($posy -= $linepitch) <= 0){
		if(++$curcolumn >= $columns){
			do endpage(1);
			$curcolumn = 0;
		}
		else {
			do endpage(0);
		}
		do beginpage($curcolumn);
	}
	elsif(! $printpage || $printpage == $page){
		print $posx, " ", $posy, " M\n";
	}
	if(eof){
		do endpage(1);
		$curcolumn = 0;
		close(ARGV);	# reset line number
		$page = 0;
	}
}
do epilogue();
exit 0;
#
#	Subroutines
#
sub flush {
	print "Flush!! mode = $mode\n" if $dflag;
	$save = $_;
	$_ = join("",@outbuf);
	while(! /^$/){
		if(s/^[^\033\t\200-\377]+//){
			$s = $&;
			$s =~ s/\\/\\\\/g;
			$s =~ s/\(/\\(/g;
			$s =~ s/\)/\\)/g;
			print "($s) ",$aabv[$outmode],"\n";
		}
		elsif(s/^\t+//){
			do tab(length($&));
		}
		elsif(s/^([\200-\377].)+//){
			print "D (",
				$kflag ? ($eflag ? do jis2oct(do ujis2jis($&)) :
						   do jis2oct(do sjis2jis($&)))
				 : "\033\$B" .
				    ( $eflag ? do ujis2jis($&) : 
						do sjis2jis($&) ) .
				   "\033(B" ,
				") J U\n";
		}
		elsif(s/^\033\$.(([^\033].)*)\033\(.//){
			print "D (", $kflag ? do jis2oct($1) : $& ,") J U\n";
		}
		else {
			s/.//;
			print sprintf("([%02x]) ",ord($&) & 0xff);
			print $aabv[$outmode],"\n";
		}
	}
	$_ = $save;
	@outbuf = ();
}
sub output {
	local($str,$newmode) = @_;
	print "Output: ($str, $newmode)\n" if $dflag;
	if($outmode == $newmode){
		push(outbuf,$str);
	}
	else {
		do flush();
		$outmode = $newmode;
		push(outbuf,$str);
	}
}
sub skip {
	local($str) = @_;
	$len = length($str);
	$_ = substr($_,$len,length($_)-$len);
}
sub tab {
	local($num) = @_;
	print "$num T\n";
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
	$result;
}
sub ujis2jis {
	local($result) = '';
	local($_) = @_;
	while(s/^(.)(.)//){
		$result .= sprintf("%c%c",ord($1) & 0x7f,ord($2) & 0x7f);
	}
	$result;
}
sub jis2oct {
	local($result) = '';
	local($_) = @_;
	while(s/^(.)(.)//){
		$result .= sprintf("\\%03o\\%03o",
			(ord($1) & 0x7f) + 0x80, (ord($2) & 0x7f) + 0x80);
	}
	$result;
}

sub prologue {
	print "%!", $nflag ? "" : "PS-Adobe-2.0\n";
	print "%%Creator: jenscript\n" if ! $nflag;
	print "%%Title: Jenscript\n" if ! $nflag;
	print "%%CreationDate: $date\n" if ! $nflag;
	print "%%EndComments\n" if ! $nflag;
	print "%%DocumentFonts: $kfont",$afont ne "" ? " $afont" : "", "\n";
	print "save/EnscriptJob exch def\n";
	print "/StartEnscriptDoc{\$enscript begin}def\n";
	print "/\$enscript 60 dict def \$enscript begin\n";
	print "/EndEnscriptDoc{end}def\n";
	print "/StartPage{/svpg save def }def\n";
	print "/EndPage{svpg restore showpage}def\n";
	print "/SetStTime{statusdict /manualfeedtimeout 120 put} def\n";
	print "/SetStatus{statusdict /manualfeed true put\n";
	print "statusdict /product get (LaserWriter) eq \n";
	print "{version (23.0) eq  % Don't redefine EndPage if printer is not \"Classic LW\"\n";
	print "     {/EndPage {svpg restore\n";
	print "\t {statusdict /printerstatus get exec 16#22000000 and 0 eq{exit}if}loop\n";
	print "\t showpage}def}if }if}def\n";
	if(! $xflag){
		print "/FN { /$normalfont findfont $fontsize scalefont setfont } bind def\n";
		print "/FK { /$keywordfont findfont $fontsize scalefont setfont } bind def\n";
		print "/FC { /$commentfont findfont $fontsize scalefont setfont } bind def\n";
		print "/FS { /$stringfont findfont $fontsize scalefont setfont } bind def\n";
		print "/FL { /$literalfont findfont $fontsize scalefont setfont } bind def\n";
	}
	else {
		$xfontsize = $jfontsize * 5 / 6;
		print "/FN { /$normalfont findfont [$xfontsize 0 0 $fontsize 0 0] makefont setfont } bind def\n";
		print "/FK { /$keywordfont findfont [$xfontsize 0 0 $fontsize 0 0] makefont setfont } bind def\n";
		print "/FC { /$commentfont findfont [$xfontsize 0 0 $fontsize 0 0] makefont setfont } bind def\n";
		print "/FS { /$stringfont findfont [$xfontsize 0 0 $fontsize 0 0] makefont setfont } bind def\n";
		print "/FL { /$litefalfont findfont [$xfontsize 0 0 $fontsize 0 0] makefont setfont } bind def\n";
	}
	print "/FJ { /$kfont findfont $jfontsize scalefont setfont } bind def\n";
	print "/N { FN show } bind def\n";
	print "/K { FK show } bind def\n";
	print "/C { FC show } bind def\n";
	print "/S { FS show } bind def\n";
	print "/L { FL show } bind def\n";
	print "/J { FJ show } bind def\n";
	print "FN (ABCDabcd) stringwidth pop /TABWIDTH exch def\n";
	print "/T { currentpoint exch 3 -1 roll \n";
	print "\tTABWIDTH mul add TABWIDTH div truncate\n";
	print "\tTABWIDTH mul exch moveto\n";
	print "} bind def\n";
	print "/U { 0 ",$texfont ? 0 : $fontsize/10," rmoveto } bind def\n";
	print "/D { 0 -",$texfont ? 0 : $fontsize/10," rmoveto } bind def\n";
	print "/M { moveto } bind def\n";
	print "end\n";
	print "StartEnscriptDoc % end fixed prolog\n";
	print "SetStatus  SetStTime\n" if $mflag;
	print "%%EndProlog\n" if !$nflag;
}
sub epilogue {
	print "%%Trailer\n" if ! $nflag;
	print "%%Pages: $page\n" if ! $nflag;
	print "EndEnscriptDoc\n";
	print "EnscriptJob restore\n";
}
sub beginpage {
	local($col) = @_;
	($posx, $posy) = (0, $height);
	++$page if $col == 0;
	if($printpage && $printpage != $page){ return; }
	if($col == 0){
		print "%%Page: ? $page\n";
		print "StartPage\n";
		if($gflag){
			print "gsave\n";
			print $offsetx, " ",$offsety + $height + 15," M\n";
			print "/Times-Roman findfont\n";
			print "12 scalefont setfont\n";
			$title = $header eq "" ? $ARGV : $header ;
			print "($title    Page $page     $date) show\n";
			print "grestore\n";
		}
	}
	printf("gsave\n");
	print $offsetx + $width * $col / $columns," $offsety translate\n";
	print "0 $height M\n";
}
sub endpage {
	local($show) = @_;
	if($printpage && $printpage != $page){ return; }
	print "grestore\n";
	print "EndPage\n" if $show;
}
!NO!SUBS!
chmod 755 jenscript
$eunicefix jenscript
