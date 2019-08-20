#include <stdio.h>
#include "config.h"
#if SHARPOA

#include "kanjips.h"

#define OAFONTDIR "/ext/knj/"
#define HEADERSIZE 01000	/* header size of OA kanji font file */
#define BLOCKSIZE 010000	/* block size of OA kanji font file */

static char *oafonts[] = {
	SHARPOAFONTS,
	NULL
};

typedef int Loc[ROW][COLUMN];

static
defineoafont(font,code) Font *font; int code;
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
	fseek(font->fp,(*(Loc *)(font->d))[ub-0x21][lb-0x21],0);
	for(i=0; i<dotfontsize; i++){
		for(j=0; j<column; j++){
			fprintf(outfile,"%02x",fgetc(font->fp));
		}
	}
	fprintf(outfile,"> %d %d 0 0 1 ] def\n",dotfontsize,dotfontsize);
	return 1;
}

static
closeoafont(font) Font *font;
{
	if(font->fp) fclose(font->fp);
	if(font->name) free(font->name);
	if(font->d) free(font->d);
	free(font);
}

Font *
openoafont(fontname) char *fontname;
{
	int dotfontsize;
	register Font *f;
	int column;	/* bytes%column */
	int nbytes;	/* bytes per character */
	int count = 0;
	int i,j,k,ub,lb;

	if(! fontmatch(fontname,oafonts))
		return NULL;
	f = (Font *)Malloc(sizeof(Font));
	f->name = (char *)Malloc(strlen(fontname)+1);
	strcpy(f->name,fontname);
	f->_definefont = defineoafont;
	f->_closefont = closeoafont;
	sscanf(fontname,"%*[^0123456789]%d",&dotfontsize);
	fprintf(outfile,"$kanjips begin \n");
	fprintf(outfile,"/%s -%d newdotfont\n",fontname,dotfontsize);
	fprintf(outfile,"end \n");
	if(f->fp)
		fclose(f->fp);
	if((f->fp = openfontfile(OAFONTDIR,fontname,"")) == NULL)
		return NULL;
	f->d = (char *)Malloc(sizeof(Loc));
	column = (dotfontsize+7)/8;
	nbytes = dotfontsize * dotfontsize / 8;
	for(i=0;i<HEADERSIZE;i++) fgetc(f->fp); /* skip header */
	while((ub = fgetc(f->fp)) != EOF){
		if(count+nbytes < BLOCKSIZE){
			count += nbytes+8;
			lb = fgetc(f->fp);
			if(ub < 0x21 || ub >= 0x74) break;
			(*((Loc *)(f->d)))[ub-0x21][lb-0x21] = ftell(f->fp);
			for(j = 0; j < dotfontsize; j++){
				for(k = 0; k < column; k++){
					fgetc(f->fp);
				}
			}
			/* skip trailer */
			for(i=0;i<6;i++) fgetc(f->fp);
		}
		else{ /* skip until next block */
			for(count++; count<BLOCKSIZE; count++) fgetc(f->fp);
			count = 0;
		}
	}
	return f;
}
#endif /* SHARPOA */
