CFLAGS = -O -I..

FONTOBJ = outlinefont.o dotfont.o texfont.o lwfont.o hersheyfont.o 	oafont.o gmwfont.o x11font.o

kanjips: kanjips.o header.o $(FONTOBJ) fonts.o sub.o
	$(CC) $(CFLAGS) -o kanjips kanjips.o header.o $(FONTOBJ) fonts.o sub.o -lm
kanjips.o: kanjips.c ../config.h
	$(CC) $(CFLAGS) -c kanjips.c
#kanjips.c: kanjips.z kanjips.h
#	Flex kanjips.z > kanjips.c
header.o: headerfile
	../text2x/text2c -f outheader -o outfile headerfile > header.c
	$(CC) $(CFLAGS) -c header.c
$(FONTOBJ): kanjips.h ../config.h
fonts.o sub.o: kanjips.h ../config.h

clean:
	rm -f *.o core
