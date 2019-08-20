#include <stdio.h>
#include "config.h"
#if GMW

#include "kanjips.h"

static char *gmwfonts[] = {
	GMWFONTS,
	NULL
};

typedef struct {
	int	magic;	/* should be 0502 */
	int	size;	/* bitmap size */
	int	width;	/* font width */
	int	height;	/* font height */
} GmwFontHeader;
#define KMAGIC 0502

static
definegmwfont(font,code) Font *font; int code;
{
	register i,j,c;
	register ub,lb;
	int pos[COLUMN];
	char cbuf[(COLUMN*100+16)/8],*s;
	GmwFontHeader *g = (GmwFontHeader *)(font->d);
	int nbytes = (g->width * COLUMN + 31)/32*4;
	int cb = (g->width + 7)/8;
	int ndots;
	int skip, skew;
	static int mask[] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };

	ub = (code >> 8) & 0x7f; lb = code & 0x7f;
	ndots = (lb - 0x21) * g->width;
	skip = ndots / 8;
	skew = ndots % 8;
	if(font->fp == NULL) return 0;
	fprintf(outfile,"%s [<",kcode(code,font->name));
	fseek(font->fp,sizeof(GmwFontHeader),0);
	fread(pos,sizeof(int),COLUMN,font->fp);
	if(pos[ub-0x21] >= 0){
		fseek(font->fp,pos[ub-0x21],1);
		for(i=0; i < g->height; i++){
			fread(cbuf,sizeof(char),nbytes,font->fp);
			for(s=cbuf+skip, j=0; j<cb; j++, s++){
				c = ((*s & mask[skew]) << skew) |
				    (skew ?
				    ((*(s+1) & ~mask[skew] & 0xff) >>(8-skew)):
				    0 ) ;
				fprintf(outfile,"%02x",c & 0xff);
			}
		}
	}
	fprintf(outfile,"> %d %d 0 0 1 ] def\n",g->width,g->height);
	return 1;
}

static
closegmwfont(font) Font *font;
{
	if(font->fp) fclose(font->fp);
	if(font->name) free(font->name);
	if(font->d) free(font->d);
	free(font);
}

Font *
opengmwfont(fontname) char *fontname;
{
	register Font *f;
	GmwFontHeader *g;

	if(! fontmatch(fontname,gmwfonts)) 
		return NULL;
	f = (Font *)Malloc(sizeof(Font));
	f->name = (char *)Malloc(strlen(fontname)+1);
	strcpy(f->name,fontname);
	f->_definefont = definegmwfont;
	f->_closefont = closegmwfont;
	f->d = (char *)Malloc(sizeof(GmwFontHeader));
	if(f->fp)
		fclose(f->fp);
	if((f->fp = openfontfile(GMWFONTPATH,fontname,"")) == NULL)
		return NULL;
	fread(f->d, sizeof(GmwFontHeader), 1, f->fp);
	g = (GmwFontHeader *)(f->d);
	if(g->magic != KMAGIC)
		return NULL;
	fprintf(outfile,"$kanjips begin \n");
	fprintf(outfile,"/%s -%d newdotfont\n",fontname,g->width);
	fprintf(outfile,"end \n");
	return f;
}
#endif /* GMW */
