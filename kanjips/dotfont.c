#include <stdio.h>
#include "config.h"
#include "kanjips.h"

static char *dotfonts[] = {
	"dot",
	"jis",
	NULL
};

static
definedotfont(font,code) Font *font; int code;
{
	register i,j;
	register ub,lb;
	int dotfontsize,column;

	ub = (code >> 8) & 0x7f; lb = code & 0x7f;
	if(font->fp == NULL) return 0;
	sscanf(font->name,"%*[^0123456789]%d",&dotfontsize);
	fprintf(outfile,"%s [<",kcode(code,font->name));
	if(ub == 0x21 && lb == 0x21){ /* space */
		ub = 0x22; lb = 0x44;
	}
	column = (dotfontsize+7)/8;
	fseek(font->fp,
	   (COLUMN*(ub-0x21)+(lb-0x21))*dotfontsize*column,0);
	for(i=0; i<dotfontsize; i++){
		for(j=0; j<column; j++){
			fprintf(outfile,"%02x",fgetc(font->fp));
		}
	}
	fprintf(outfile,"> %d %d 0 0 1 ] def\n",dotfontsize,dotfontsize);
	return 1;
}

static
closedotfont(font) Font *font;
{
	if(font->fp) fclose(font->fp);
	if(font->name) free(font->name);
	free(font);
}

Font *
opendotfont(fontname) char *fontname;
{
	int dotfontsize;
	register Font *f;

	if(! fontmatch(fontname,dotfonts))
		return NULL;
	f = (Font *)Malloc(sizeof(Font));
	f->name = (char *)Malloc(strlen(fontname)+1);
	strcpy(f->name,fontname);
	f->_definefont = definedotfont;
	f->_closefont = closedotfont;
	f->d = NULL;
	sscanf(fontname,"%*[^0123456789]%d",&dotfontsize);
	fprintf(outfile,"$kanjips begin \n");
	fprintf(outfile,"/%s -%d newdotfont\n",fontname,dotfontsize);
	fprintf(outfile,"end \n");
	if(f->fp)
		fclose(f->fp);
	if((f->fp = openfontfile(LOCALFONTDIR,fontname,"")) == NULL)
		return NULL;
	return f;
}
