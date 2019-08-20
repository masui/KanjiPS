#include <stdio.h>
extern FILE *outfile;
static char *outstr[] = {
	"%\n",
	"%\tHeader for Kanji PostScript\n",
	"%\n",
	"%\t\t1989 2/3 MASUI Toshiyuki @ SHARP CSL\n",
	"%\n",
	"/$kdata 750 dict def\t% dictionary for kanji dot/outline\n",
	"/$kanjips 100 dict def\t% dictionary for kanjiPS operators\n",
	"$kanjips begin\n",
	"\n",
	"/newdotfont\t\t% fontid dotsize newdotfont --- \n",
	"\t\t\t%\tinitialize a new dot font dictionary \n",
	"{\n",
	"\t/dotsize exch def\n",
	"\t/newname exch def\n",
	"\t/fsize 256 def\n",
	"\tnewname 11 dict def\t\t% allocate new font dictionary \n",
	"\tnewname load begin\n",
	"\t\t/FontName newname def\n",
	"\t\t/DotSize dotsize def\n",
	"\t\t/FontType 3 def\t\t\t% user font type\n",
	"\t\t/FontMatrix [1 0 0 1 0 0] def\n",
	"\t\t/FontBBox [0 0 dotsize dotsize] def\n",
	"\t\t/BitMaps fsize array def\n",
	"\t\t/BuildChar {dotbuilder} def\n",
	"\t\t/Encoding fsize array def\n",
	"\t\t/DefineProc {definedot} def\n",
	"\t\t/KanjiPS 0 def\n",
	"\t\t0 1 fsize 1 sub {Encoding exch /.notdef put} for\n",
	"\tend\n",
	"\tnewname newname load definefont pop\n",
	"} def\n",
	"\n",
	"/ch-image {ch-data 0 get} def\n",
	"/ch-width {ch-data 1 get} def\n",
	"/ch-height {ch-data 2 get} def\n",
	"/ch-xoff {ch-data 3 get} def\n",
	"/ch-yoff {ch-data 4 get} def\n",
	"/ch-tfmw {ch-data 5 get} def\n",
	"\n",
	"/dotbuilder\t% fontdict ch Charbuilder -     -- image one character\n",
	"{\n",
	"\t/ch-code exch def           % save the char code\n",
	"\t/font-dict exch def         % and the font dict.\n",
	"\t/ch-data font-dict /BitMaps get ch-code get def \n",
	"\t/dotsize font-dict /DotSize get def\n",
	"\tdotsize 0 le\n",
	"\t{\n",
	"\t\t/ydir -1 def /dotsize dotsize neg def\n",
	"\t\t/yoff dotsize ch-yoff sub 1 add def \n",
	"\t}\n",
	"\t{\n",
	"\t\t/ydir 1 def /yoff ch-yoff def \n",
	"\t}\n",
	"\tifelse\n",
	"\t% get the bitmap descriptor for char\n",
	"\tch-image null eq\n",
	"\tch-width 0 eq or\n",
	"\tch-height 0 eq or\n",
	"\tnot\n",
	"\t{ \n",
	"%\t\t1 0 0 0 1 1 setcachedevice\t% Can't use cache!!\n",
	"\t\tch-tfmw 0 setcharwidth\n",
	"\t\t1 dotsize div dup scale\n",
	"\t\tch-width ch-height true [1 0 0 ydir ch-xoff yoff]\n",
	"\t\t\t{ch-image} imagemask\n",
	"\t} if\n",
	"} def\n",
	"\n",
	"/definedot\t% image ch definedot  - define a dot font\n",
	"{\t\n",
	"\t/ch-code exch def\n",
	"\t/ch-data exch def\n",
	"\tcurrentfont /BitMaps get ch-code ch-data put\n",
	"\tcurrentfont /Encoding get ch-code \n",
	"\tdup (   ) cvs cvn   % generate a unique name from the character code\n",
	"\tput\n",
	"} def\n",
	"\n",
	"/newoutlinefont\t\t% fontid newoutlinefont --- \n",
	"\t\t\t%\tinitialize a new dot font dictionary \n",
	"{\n",
	"\t/newname exch def\n",
	"\t/fsize 256 def\n",
	"\tnewname 10 dict def\t\t% allocate new font dictionary \n",
	"\tnewname load begin\n",
	"\t\t/FontName newname def\n",
	"\t\t/FontType 3 def\t\t\t% user font type\n",
	"\t\t/FontMatrix [.001 0 0 .001 0 0] def\n",
	"\t\t/FontBBox [0 0 1000 1000] def\n",
	"\t\t/CharProcs fsize array def\n",
	"\t\t/BuildChar {outlinebuilder} def\n",
	"\t\t/Encoding fsize array def\n",
	"\t\t/DefineProc {defineoutline} def\n",
	"\t\t/KanjiPS 0 def\n",
	"\t\t0 1 fsize 1 sub {Encoding exch /.notdef put} for\n",
	"\tend\n",
	"\tnewname newname load definefont pop\n",
	"} def\n",
	"\n",
	"/outlinebuilder\t\t% fontdict ch Charbuilder -  draw one charcter\n",
	"{\n",
	"\t/ch-code exch def           % save the char code\n",
	"\t/font-dict exch def         % and the font dict.\n",
	"%\t1 0 0 0 1 1 setcachedevice\t% Can't use cache!!\n",
	"\t1000 0 setcharwidth\t\t% Use this instead..\n",
	"\tfont-dict /CharProcs get ch-code get cvx exec\n",
	"} def\n",
	"\n",
	"/defineoutline\t\t% proc ch defineoutline  - define an outline font\n",
	"{\t\n",
	"\t/ch-code exch def\n",
	"\t/ch-proc exch cvlit def\n",
	"\tcurrentfont /CharProcs get ch-code ch-proc put\n",
	"\tcurrentfont /Encoding get ch-code \n",
	"\tdup (   ) cvs cvn   % generate a unique name from the character code\n",
	"\tput\n",
	"} def\n",
	"\n",
	"/strconv {\n",
	"\tcurrentfont /KanjiPS known { % user defined font\n",
	"%\tcurrentfont /FontType get 3 eq { % user defined font\n",
	"\t\tdup /EUCstr exch def length 2 idiv /len exch def\n",
	"\t\t/str len string def\n",
	"\t\t/fontname 20 string def\n",
	"\t\tcurrentfont /FontName get fontname cvs\n",
	"\t\t/defineproc currentfont /DefineProc get def\n",
	"\t\tdup length 5 add string /codestr exch def\n",
	"\t\tcodestr exch 5 exch putinterval\n",
	"\t\tcodestr 0 (k) putinterval\n",
	"\t\t/hexstr 2 string def\n",
	"\t\t0 1 len 1 sub {\n",
	"\t\t\t/index exch def\n",
	"\t\t\tstr index index put\n",
	"\t\t\tindex 2 mul EUCstr exch get 16 hexstr cvrs \n",
	"\t\t\tcodestr exch 1 exch putinterval\n",
	"\t\t\tindex 2 mul 1 add EUCstr exch get 16 hexstr cvrs \n",
	"\t\t\tcodestr exch 3 exch putinterval\n",
	"\t\t\tcodestr cvn \n",
	"\t\t\t$kdata begin load end\n",
	"\t\t\tindex defineproc\n",
	"\t\t} for\n",
	"\t\tstr\n",
	"\t} if\n",
	"} def\n",
	"/$$charpath /charpath load def\n",
	"/$$stringwidth /stringwidth load def\n",
	"/$$show /show load def\n",
	"/$$ashow /ashow load def\n",
	"/$$kshow /kshow load def\n",
	"end % $kanjips\n",
	"$kdata begin\n",
	"/E { /exec load } bind def\n",
	"end\n",
	"%\n",
	"%\tReDefinition of operators like 'show.'\n",
	"%\n",
	"/charpath { $kanjips begin exch strconv exch $$charpath end } def\n",
	"/stringwidth { $kanjips begin /S save def\n",
	"strconv $$stringwidth S restore end } def\n",
	"/show {\n",
	"\t$kanjips begin /S save def\n",
	"\tstrconv $$show currentpoint S restore moveto end\n",
	"} def\n",
	"/ashow {\n",
	"\t$kanjips begin /S save def\n",
	"\tstrconv $$ashow currentpoint S restore moveto end\n",
	"} def\n",
	"/kshow {\n",
	"\t$kanjips begin /S save def\n",
	"\tstrconv $$kshow currentpoint S restore moveto end\n",
	"} def\n",
	"\n",
	0
};
outheader()
{
	register char **s;
	for(s = outstr; *s; s++){
		fprintf(outfile,"%s",*s);
	}
}
