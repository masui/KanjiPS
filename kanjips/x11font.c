#include <stdio.h>
#include "config.h"
#if X11

#include "kanjips.h"

static char *x11fonts[] = {
	XFONTS,
	NULL
};
#define BDFEXT ".bdf"

typedef int Loc[ROW][COLUMN];

static
definex11font(font,code) Font *font; int code;
{
	register i;
	register ub,lb;
	int dotfontsize;
	int w,h,offx,offy;
	char buf[BUFMAX];

	ub = (code >> 8) & 0x7f; lb = code & 0x7f;
	if(font->fp == NULL) return 0;
	sscanf(font->name,"%*[^0123456789]%d",&dotfontsize);
	fprintf(outfile,"%s [<",kcode(code,font->name));
	fseek(font->fp,(*(Loc *)(font->d))[ub-0x21][lb-0x21],0);
	for(; fgets(buf,BUFMAX,font->fp);){
		if(strmatch(buf,"BBX")){
			sscanf(buf,"%*s %d %d %d %d",&w,&h,&offx,&offy);
		}
		if(strmatch(buf,"BITMAP")){
			for(i=0; i<h; i++){
				fgets(buf,BUFMAX,font->fp);
				fprintf(outfile,"%s",buf);
			}
			fprintf(outfile,"> %d %d %d %d %d ] def\n",
				w,h,-offx,-offy,1);
			break;
		}
	}
	return 1;
}

static
closex11font(font) Font *font;
{
	if(font->fp) fclose(font->fp);
	if(font->name) free(font->name);
	if(font->d) free(font->d);
	free(font);
}

Font *
openx11font(fontname) char *fontname;
{
	int dotfontsize;
	register Font *f;
	char buf[BUFMAX];
	int ub,lb;
	int pos,cpos;
	int code;

	if(! fontmatch(fontname,x11fonts))
		return NULL;
	f = (Font *)Malloc(sizeof(Font));
	f->name = (char *)Malloc(strlen(fontname)+1);
	strcpy(f->name,fontname);
	f->_definefont = definex11font;
	f->_closefont = closex11font;
	sscanf(fontname,"%*[^0123456789]%d",&dotfontsize);
	fprintf(outfile,"$kanjips begin \n");
	fprintf(outfile,"/%s -%d newdotfont\n",fontname,dotfontsize);
	fprintf(outfile,"end \n");
	if(f->fp)
		fclose(f->fp);
	if((f->fp = openfontfile(XFONTPATH,fontname,BDFEXT)) == NULL)
		return NULL;
	f->d = (char *)Malloc(sizeof(Loc));
	for(pos = ftell(f->fp); fgets(buf,BUFMAX,f->fp); pos = ftell(f->fp)){
		if(strmatch(buf,"STARTCHAR")) cpos = pos;
		if(strmatch(buf,"ENCODING")){
			sscanf(buf,"%*s %d",&code);
			ub = (code >> 8); lb = code & 0xff ;
			(*(Loc *)(f->d))[ub-0x21][lb-0x21] = cpos;
		}
	}
	return f;
}
#endif /* X11 */
