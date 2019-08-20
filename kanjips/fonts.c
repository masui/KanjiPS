#include <stdio.h>
#include "config.h"
#include "kanjips.h"

#if HERSHEY
extern Font *openhersheyfont();
#endif
#if JTEX
extern Font *opentexfont();
#endif
#if SHARPOA
extern Font *openoafont();
#endif
#if SHARP
extern Font *openoutlinefont();
#endif
#if GMW
extern Font *opengmwfont();
#endif
#if X11
extern Font *openx11font();
#endif
extern Font *openlwfont();
extern Font *opendotfont();

Font *(*fontopenprocs[])() = {
#if HERSHEY
	openhersheyfont,
#endif
#if JTEX
	opentexfont,
#endif
#if SHARPOA
	openoafont,
#endif
#if SHARP
	openoutlinefont,
#endif
#if GMW
	opengmwfont,
#endif
#if X11
	openx11font,
#endif
	openlwfont,
	opendotfont,
	NULL
};

Font *openfont(fontname) char *fontname;
{
	Font *(**f)(),*ff;
	for(f=fontopenprocs; *f; f++){
		if(ff = (*f)(fontname))
			return ff;
	}
	return NULL;
}

char *
kcode(code,fontname) int code; char *fontname;
{
	static char *h = "0123456789ABCDEF";
	static char nam[MAXFONTNAME+7] = "/kxxxx";
	nam[2] = h[(code >> 12) & 0xf];
	nam[3] = h[(code >> 8) & 0xf];
	nam[4] = h[(code >> 4) & 0xf];
	nam[5] = h[(code >> 0) & 0xf];
	strcpy(nam+6,fontname);
	return nam;
}
