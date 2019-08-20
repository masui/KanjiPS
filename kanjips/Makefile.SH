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
echo "Extracting Makefile (with variable substitutions)"
: This section of the file will have variable substitutions done on it.
: Move anything that needs config subs from !NO!SUBS! section to !GROK!THIS!.
: Protect any dollar signs and backticks that you do not want interpreted
: by putting a backslash in front.  You may delete these comments.
$spitshell >Makefile <<!GROK!THIS!
CFLAGS = -O -I..

FONTOBJ = outlinefont.o dotfont.o texfont.o lwfont.o hersheyfont.o \
	oafont.o gmwfont.o x11font.o

kanjips: kanjips.o header.o $(FONTOBJ) fonts.o sub.o
	$(CC) $(CFLAGS) -o kanjips kanjips.o header.o $(FONTOBJ) fonts.o sub.o -lm
kanjips.o: kanjips.c ../config.h
	$(CC) $(CFLAGS) -c kanjips.c
${flexcomment}kanjips.c: kanjips.z kanjips.h
${flexcomment}	Flex kanjips.z > kanjips.c
header.o: headerfile
${perlcomment}	../text2x/text2c -f outheader -o outfile headerfile > header.c
	$(CC) $(CFLAGS) -c header.c
$(FONTOBJ): kanjips.h ../config.h
fonts.o sub.o: kanjips.h ../config.h

clean:
	rm -f *.o core
!GROK!THIS!

: In the following dollars and backticks do not need the extra backslash.
$spitshell >>Makefile <<'!NO!SUBS!'
!NO!SUBS!
chmod 755 Makefile
$eunicefix Makefile
