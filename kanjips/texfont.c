/*
 *	This file is based on `texpr/dvi2lpb/pkfont.c'.
 *	The copyright of the original is:
 *		Copyright (c) Japan TeX Users Group, 1986 1987
 */
#include <stdio.h>
#include <ctype.h>
#include "config.h"
#if JTEX

#include "kanjips.h"

static char *texfonts[] = {
	JTEXFONTS,
	NULL
};

#define	min(x,y)	((x)<=(y) ? (x) : (y))

char *fontdir = TEXFONTPATH ;

#ifndef DPI
#define DPI 300
#endif

#define  LASTFNTCHAR    255
#define  NFNTCHARS      256
#define	 NPXLCHARS	256
#define	 NHALFPXLCHARS	128

#define  NONEXISTANT     -1   /* offset for PXL files not found */

#define	FT_PK		1
#define	FT_PXL		2
#define	FT_LW		3
#define	FT_LWTEX	4

#define PK_ID		89
#define PK_FLAG		239
#define PK_XXX1		240
#define PK_XXX2		241
#define PK_XXX3		242
#define PK_XXX4		243
#define PK_YYY		244
#define PK_POST		245
#define PK_NO_OP	246
#define PK_PRE		247

typedef struct {		/* character entry */
	unsigned short width, height;/* width and height in pixels */
	short xOffset, yOffset;	/* x offset and y offset in pixels */
	short psfont, pschar;
	unsigned short nbpl;		/* # of bytes per pixel data line */
	int fileOffset;
	char *pixptr;
	float tfmw;			/* TFM width */
} CharEntry;

typedef struct {  /* font entry */
	int k, c, s, d, a, l;
	char n[MAXFONTNAME];	/* FNT_DEF command parameters		*/
	int font_space;		/* computed from FNT_DEF s parameter	*/
	float font_mag;		/* computed from FNT_DEF s and d parameters */
	int font_type;		/* FT_PK, FT_PXL, FT_LWTEX, FT_LW	*/
	int dic_pack;		/* pack psfont dictionary		*/
	char name[MAXPATHNAME];	/* full name of font file		*/
	FILE *font_file_id;	/* file identifier (NO_FILE if none)	*/
	int magnification;	/* magnification read from PXL file	*/
	int designsize;		/* design size read from PXL file	*/
	int nfntchars;
	CharEntry ch[NFNTCHARS];/* character information	*/
/*	struct FontEntry *next; */
	int ncdl;		/* # of different chars actually downloaded */
} FontEntry;

#define KANJIFONTS 33

typedef FontEntry KanjiFontEntry[KANJIFONTS];

int subfontindex;
int subfontcode;
char *subfontname;	/* "sy", "greek", "ka", etc. */

static 
definetexfont(font,code) Font *font; int code;
{
	KanjiFontEntry *kfe;
	FontEntry *fe;

	if((kfe = (KanjiFontEntry *)(font->d)) == NULL)
		return 0;
	subfont(code);	/* calculate subfont information */
	fe = &((*kfe)[subfontindex]);
	if(fe->name[0] == '\0'){
		readfontinfo(font,&((*kfe)[subfontindex]));
	}
	switch(fe->font_type){
		case FT_PXL:	loadpxlchar(fe,subfontcode);
				break;
		case FT_PK:	loadpkchar(fe,subfontcode);
				break;
	}
	fprintf(outfile,"%s\n",kcode(code,font->name));
	emitchar(fe,subfontcode);
	return 1;
}

subfont(code) int code;
{
	int lb,ub,c;
	char *buf = "k*";
	ub = (code >> 8) & 0x7f;;
	lb = (code & 0x7f);
	if(ub < 0x21 || ub < 0x30 && ub > 0x28)
		return 0;
	switch(ub){
		case 0x21:	subfontname = "sy";
				subfontcode = lb-0x20;
				subfontindex = 0;
				break;
		case 0x22:	subfontname = "sy";
				subfontindex = 0;
				if(lb >= 0x72){
					subfontcode = lb + (0xb6 - 0x72);
				}
				else if(lb >= 0x5c){
					subfontcode = lb + (0xa0 - 0x5c);
				}
				else if(lb >= 0x4a){
					subfontcode = lb + (0x8e - 0x4a);
				}
				else if(lb >= 0x3a){
					subfontcode = lb + (0x7e - 0x3a);
				}
				else {
					subfontcode = lb + (0x65 - 0x21);
				}
				break;
		case 0x23:	subfontname = "roma";
				subfontindex = 1;
				subfontcode = lb;
				break;
		case 0x24:	subfontname = "hira";
				subfontindex = 2;
				subfontcode = lb - 0x20;
				break;
		case 0x25:	subfontname = "kata";
				subfontindex = 3;
				subfontcode = lb - 0x20;
				break;
		case 0x26:	subfontname = "greek";
				subfontindex = 4;
				subfontcode = lb - 0x20;
				break;
		case 0x27:	subfontname = "russian";
				subfontindex = 5;
				subfontcode = lb - 0x20;
				break;
		case 0x28:	subfontname = "keisen";
				subfontindex = 6;
				subfontcode = lb - 0x20;
				break;
		default:	c = (ub - 0x30) * COLUMN + (lb - 0x21);
				buf[1] = 'a' + c/256;
				subfontname = buf;
				subfontcode = c % 256;
				subfontindex = c/256 + 7;
	}
	return 1;
}
readfontinfo(font,fe) Font *font; FontEntry *fe;
{
	char fontpath[MAXPATHNAME];
	char *s;
	char *dirname;
	FILE *fp = NULL;
	int fontsize;
	char fontfamily[100];

	sscanf(font->name,"%[^0123456789]%d",fontfamily,&fontsize);
	for(s = fontdir; *s; s++){
		while(isspace(*s)) s++;
		dirname = s;
		while(*s != ':' && *s != '\0'){
			s++;
		}
		*s = '\0';
		sprintf(fontpath,"%s/%s%s%d.%dpxl",dirname,
			fontfamily,subfontname,fontsize,DPI*5);
		if(fp = fopen(fontpath,"r")){
			fe->font_type = FT_PXL;
			strcpy(fe->name,fontpath);
			ReadPXLFontInfo(fe,fp);
			fclose(fp);
			break;
		}
		sprintf(fontpath,"%s/%s%s%d.%dpk",dirname,
			fontfamily,subfontname,fontsize,DPI);
		if(fp = fopen(fontpath,"r")){
			fe->font_type = FT_PK;
			strcpy(fe->name,fontpath);
			ReadPKFontInfo(fe,fp);
			fclose(fp);
			break;
		}
	}
	if(fp == NULL) return 0;
	return 1;
}
ReadPKFontInfo(fe,fp) FontEntry *fe; FILE *fp;
{
	register int cmd, i, t;

	i = 0;
	do {
		if(fseek(fp, --i, 2) == -1)
			fatal("can't seek to the end of pk file\n");
	} while((t=NoSignExtend(fp, 1)) == PK_NO_OP);
	fseek(fp, 0, 0);
	if(t != PK_POST ||
	       NoSignExtend(fp,1) != PK_PRE || NoSignExtend(fp,1) != PK_ID)
		fatal("bad pk file\n");
	fseek(fp, NoSignExtend(fp, 1), 1);		/* comment */
	fe->designsize = NoSignExtend(fp, 4);		/* design size */
	NoSignExtend(fp, 4);				/* checksum */
	fseek(fp, 8, 1);				/* hppp,vppp */

	for( ; ; ) {
		switch(cmd = NoSignExtend(fp, 1)) {

		case PK_XXX1: case PK_XXX2: case PK_XXX3: case PK_XXX4:
			fseek(fp, NoSignExtend(fp, cmd-PK_XXX1+1), 1);
			continue;

		case PK_YYY:
			NoSignExtend(fp,4);
			continue;

		case PK_NO_OP:
			continue;

		case PK_POST:
			break;

		default:
			if(cmd <= PK_FLAG)
				pkcharoffset(cmd, fe, fp);
			else
				fatal("illegal pk command %d\n", cmd);
			continue;
	}
	break;
    }
}

pkcharoffset(flag, fe, fp) 
int flag;
register FontEntry *fe;
register FILE *fp;
{
    int off, pl, cc;
    register int n;
    register CharEntry *tcharptr;/* temporary char_entry pointer  */

    off = ftell(fp) - 1;
    if((n = flag&7) == 7) {	/* long form */
	pl = NoSignExtend(fp, 4) - 8;	/* without tfmw,dx */
	if((cc = NoSignExtend(fp, 4)) > LASTFNTCHAR) {
	    warn("cc %d in pk file\n", cc);
	    fseek(fp, pl+8, 1);
	    return;
	}
	tcharptr = &(fe->ch[cc]);
	tcharptr->tfmw = ((float)NoSignExtend(fp, 4)*(float)fe->s) /
	    (float)(1<<20);
	NoSignExtend(fp, 4); 	/* dx */
	n = 4;
    } else if(n >= 4) {		/* extended short form */
	pl = (n-4)*65536 + NoSignExtend(fp, 2) - 3;	/* without tfmw */
	tcharptr = &(fe->ch[NoSignExtend(fp, 1)]);
	tcharptr->tfmw = ((float)NoSignExtend(fp, 3)*(float)fe->s) /
	    (float)(1<<20);
	n = 2;
    } else {			/* short form */
	pl = n*256 + NoSignExtend(fp, 1) - 3;	/* without tfmw */
	tcharptr = &(fe->ch[NoSignExtend(fp, 1)]);
	tcharptr->tfmw = ((float)NoSignExtend(fp, 3)*(float)fe->s) /
	    (float)(1<<20);
	n = 1;
    }
    NoSignExtend(fp, n); 		/* dy or dm */
    tcharptr->width = NoSignExtend(fp, n);
    tcharptr->height = NoSignExtend(fp, n);
    tcharptr->xOffset = SignExtend(fp, n);
    tcharptr->yOffset = SignExtend(fp, n);
    tcharptr->nbpl = (tcharptr->width + 7) >> 3;
    tcharptr->fileOffset = off;
    fseek(fp, pl-5*n, 1);
}

int	width, height, dyn_f;
int	b_width, black;
char	*pixel;
char	pat[8] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};
FILE	*pkfp;

/*	character shape definition	*/

loadpkchar(fe, c) FontEntry *fe; int c;
{
    int flag, n, l;
    register CharEntry *ce;

    ce = &(fe->ch[c]);
    if((pkfp = fopen(fe->name,"r")) == NULL){
	warn("can't open pkfile %s",fe->name);
	return 0;
    }
    fseek(pkfp, ce->fileOffset, 0);
    flag = NoSignExtend(pkfp, 1);
    dyn_f = flag/16;
    black = (flag&8) >> 3;
    if((n = flag&7) == 7) {	/* long form */
	l = 16;
	n = 4;
    } else if(n >= 4) {		/* extended short form */
	l = 6;
	n = 2;
    } else {			/* short form */
	l = 5;
	n = 1;
    }
    fseek(pkfp, l+5*n, 1);
    width = ce->width;
    height = ce->height;
    b_width = ce->nbpl;
    if((pixel = (char *)calloc(height, b_width)) == NULL)
	fatal("Unable to allocate memory for char\n");
    if(dyn_f == 14)
	unpack_raster();
    else
	unpack_run();
    ce->pixptr = pixel;
    fclose(pkfp);
    return 1;
}

/*	get nybble	*/

#define get_nyb()  ((nyb_f^=1)? (nyb_w=NoSignExtend(pkfp,1))/16: nyb_w&15)

/*	get run_count/repeat_count	*/

#define get_run()\
	for( ; ; ) {\
		if((i=get_nyb()) >= 14) {\
			rep = i*2-29;\
			continue;\
		} else if(i > dyn_f)\
			run = (i-dyn_f-1)*16 + get_nyb() + dyn_f + 1;\
		else if(i > 0)\
			run = i;\
		else /* i==0 */ {\
			do	++i;\
			while((run = get_nyb()) == 0);\
			while(--i >= 0)\
				run = run*16 + get_nyb();\
			run += (13-dyn_f)*16 + dyn_f - 15;\
		}\
		if(rep >= 0)\
			break;\
		rep = run;\
	}\

/*	unpack run_encoded packet	*/

unpack_run()
{
    register char *p, *q;
    register i, run, rep, h, h1, v;
    int	nyb_f, nyb_w;

    nyb_f = 0;
    p = pixel;
    black ^= 1;
    run = 0;
    for(v=0; v < height; v += rep+1) {
	rep = 0;
	for(h=0; h < width; h=h1) {
	    if(run == 0) {
		get_run();
		black ^= 1;
	    }
	    h1 = min(h+run, width);
	    if(black) {
		h1 = min(h1, (h+8)&~7);
		p[h/8] |= pat[h&7];
		if(h1 & 7)
		    p[h1/8] &= ~pat[h1&7];
	    }
	    run -= h1-h;
	}
	q = p;	p += b_width;
	for(h=rep*b_width; --h >= 0; )
	    *p++ = *q++;
    }
}

/*	unpack raster packet	*/

unpack_raster()
{
    register char *p;
    register n, m, t, h, v;

    p = pixel;
    n = 0;
    for(v=0; v < height; ++v) {
	for(h=0; h < width; h+=m) {
	    if(n == 0) {
		t = NoSignExtend(pkfp, 1);
		n = 8;
	    }
	    m = min(n, 8-(h&7));
	    m = min(m, width-h);
	    p[h/8] |= t >> (h&7);
	    t <<= m; n -= m;
	}
	if(width & 7)
	    p[width/8] &= ~pat[width&7];
	p += b_width;
    }
}

#define PXLID 1001

ReadPXLFontInfo(fe, fp) FontEntry *fe; FILE *fp;
{
	register t,fdir;
	int nchars;
	int i;
	CharEntry *cp;

	if (NoSignExtend(fp, 4) != PXLID){
		warn("PXL ID not match\n");
		return 0;
	}
	fseek(fp, -20, 2);
	t = NoSignExtend(fp, 4);

	fe->magnification = NoSignExtend(fp, 4);

	fe->designsize = NoSignExtend(fp, 4);
    
	t = ftell(fp) - 12;
	fseek(fp, fdir = NoSignExtend(fp, 4) * 4, 0);

	nchars = (t-fdir)/(4*4);

	if(nchars > NPXLCHARS) {
		return 0;
	} else if(nchars != NHALFPXLCHARS && nchars != NPXLCHARS) {
		warn(stderr,"# font chars not match\n");
	}
    
	for (i = 0; i < nchars; i++) {
		cp = &(fe->ch[i]);
		cp->width = NoSignExtend(fp, 2);
		cp->height = NoSignExtend(fp, 2);
		cp->xOffset = SignExtend(fp, 2);
		cp->yOffset = SignExtend(fp, 2);
		cp->nbpl = ((cp->width + 31) >> 5) * 4;
		if((cp->fileOffset = NoSignExtend(fp, 4) * 4) == 0)
		    cp->fileOffset = NONEXISTANT;
		cp->tfmw = ((float)NoSignExtend(fp, 4)*(float)fe->s) /
		    (float)(1<<20);
	}
}
loadpxlchar(fe, c) FontEntry *fe; int c;
{
	char *pixel;
	int nbytes;
	register CharEntry *ce;
	FILE *fp;

	if((fp = fopen(fe->name,"r")) == NULL){
		return 0;
	}
	ce = &(fe->ch[c]);
	fseek(fp, ce->fileOffset, 0);
	nbytes = ce->nbpl * ce->height;
	pixel = (char *)Malloc(nbytes);
	fread(pixel, 1, nbytes, fp);
	ce->pixptr = pixel;
	fclose(fp);
	return 1;
}


int
NoSignExtend(fp, n)	/* return n byte quantity from file fd */
register FILE *fp;	/* file pointer    */
register int n;		/* number of bytes */

{
    register int x;	/* number being constructed */

    x = 0;
    while (n--)  {
	x <<= 8;
	x |= getc(fp);
    }
    return(x);
}


/*-->SignExtend*/
/**********************************************************************/
/****************************  SignExtend  ****************************/
/**********************************************************************/

int
SignExtend(fp, n)   /* return n byte quantity from file fd */
register FILE *fp;  /* file pointer    */
register int n;     /* number of bytes */

{
    int n1;         /* number of bytes	    */
    register int x; /* number being constructed */

    x = getc(fp);   /* get first (high-order) byte */
    n1 = n--;
    while (n--)  {
	x <<= 8;
	x |= getc(fp);
    }

    /* NOTE: This code assumes that the right-shift is an arithmetic, rather
    than logical, shift which will propagate the sign bit right.   According
    to Kernighan and Ritchie, this is compiler dependent! */

    x<<=32-8*n1;
    x>>=32-8*n1;  /* sign extend */

    return(x);
}

int hconv = 1;
char *digit = "0123456789ABCDEF";
emitchar(fe, c)		/* output a character bitmap */
FontEntry *fe;
int c;
{
    register CharEntry *ce;
    int bytewidth, i;
    register int j;
    register unsigned char *sl;
    register int cc;
    float cw;       /* char width, in "dots" - we rely on PostScript maintaining sufficient accuracy */
    
    /* Output in PostScript coord system (y +ive up, x +ive right)
       (0,0) of char bitmap at lower left.  Output scan lines
       from bottom to top */

    ce = &(fe->ch[c]);

    fprintf(outfile,"[<");
    bytewidth = (ce->width + 7) >> 3;
    for(i = ce->height-1, cc = 2;  i >= 0;  i--) {
	sl = (unsigned char *)(ce->pixptr + i*ce->nbpl);
	for(j = 0;  j < bytewidth;  j++, sl++) {
	    if( cc > 100 ) {
		fprintf(outfile,"\n  ");   cc = 2; }
#define EMITH(h)  putc(*(digit+((h>>4)&0xF)),outfile),\
		  putc(*(digit+(h&0xF)),outfile)
	    EMITH(*sl);
	    cc += 2;
	}
    }
    cw = (float)ce->tfmw / (float)hconv;
    fprintf(outfile,"> %d %d %d %d %.3f] def\n", 
	 bytewidth<<3, ce->height, ce->xOffset, (((int)ce->height)-ce->yOffset)-1, cw,
	 ce->pschar);
}

static closetexfont(font) Font *font;
{
	if(font->fp) fclose(font->fp);
	if(font->name) free(font->name);
	if(font->d) free(font->d);
	free(font);
}

Font *
opentexfont(fontname) char *fontname;
{
	register Font *f;
	KanjiFontEntry *kfe;
	int texfontsize;
	int dotsize;
	register i;
	char *dir,*getenv();

	if((dir = getenv("TEXFONTS")) != NULL)
		fontdir = dir;
	if(! fontmatch(fontname,texfonts))
		return NULL;
	f = (Font *)Malloc(sizeof(Font));
	f->name = (char *)Malloc(strlen(fontname)+1);
	strcpy(f->name,fontname);
	f->_definefont = definetexfont;
	f->_closefont = closetexfont;
	if((kfe = (KanjiFontEntry *)(f->d)) == NULL){
		kfe = (KanjiFontEntry *)Malloc(sizeof(KanjiFontEntry));
		if(kfe == NULL)
			return NULL;
		f->d = (char *)kfe;
	}
	f->fp = NULL;
	sscanf(f->name,"%*[^0123456789]%d",&texfontsize);
	dotsize = (strmatch(fontname,"jis") ?
		texfontsize : DPI * texfontsize / 72 );
	fprintf(outfile,"$kanjips begin \n");
	fprintf(outfile,"/%s %d newdotfont\n",fontname,dotsize);
	fprintf(outfile,"end \n");
	for(i=0; i<KANJIFONTS; i++){
		(*kfe)[i].name[0] = '\0';
		(*kfe)[i].s = 1;
	}
	return f;
}
#endif /* JTEX */
