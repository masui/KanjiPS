#include <stdio.h>
#include "config.h"
#if SHARP

#include "kanjips.h"
static char *outlinefonts[] = {
	SHARPFONTS,
	NULL
};

#define HEADEREXT ".fhd"	/* extension of outline font header */
#define DATAEXT ".fdt"		/* extension of outline font data */

typedef enum{	/* graphic object type */
	LINE=0,ARC,CIRCLE,POINT,ARCN=10
} Type;

static currentdefined = NO;	/* whether "currentpoint" is set or not */

#define swap(x,y) { unsigned char c = (x); (x) = (y); (y) = c; }
#define fswap(x,y){ float f = (x); (x) = (y); (y) = f; }
#define sign(x) ((x) >= 0.0 ? 1.0 : -1.0)
/*
	for data conversion between VAX and other machines
*/
typedef union {
	unsigned char c[4];
	int i;
	float f;
} Data;

#ifndef vax	/* sun, pyramid, etc. */
convint(p) Data *p;
{
	swap(p->c[0],p->c[3]);
	swap(p->c[1],p->c[2]);
}
convfloat(p) Data *p;
{
	swap(p->c[0],p->c[1]);
	swap(p->c[2],p->c[3]);
	if(p->c[0]) p->c[0]--;
}
#endif vax

#define TYPE(x) ((x) & 0x0f)
#define DEPTH(x) (((x) >> 5) & 0x3f)
#define COUNT(x) (((x) >> 12) & 0x3ff)

static
defineoutlinefont(font,code) Font *font; int code;
{
	register i,j;
	register ub,lb;
	int location;
#define RECSIZE 6	/* longwords/record */
	Data d[RECSIZE];
	int count,newcount,datasize;

	ub = (code >> 8) & 0x7f; lb = code & 0x7f;
	if(font->fp == NULL) return 0;
	location = (*((Data (*)[ROW][COLUMN])(font->d)))[ub-0x21][lb-0x21].i;
	if(location == 0) return 1; /* do nothing */
	fseek(font->fp,(location-1)*RECSIZE*sizeof(int),0);
	fread(d,sizeof(int),RECSIZE,font->fp); /* get data size */
#ifndef vax
	convint(&d[0]);
	for(i=1;i<RECSIZE;i++) 
		convfloat(&d[i]);
#endif
	datasize = d[0].i;
	fread(d,sizeof(int),RECSIZE,font->fp); /* skip dummy record*/
	fprintf(outfile,"%s [\n",kcode(code,font->name));
	fprintf(outfile,"{ newpath\n");
	for(i=0;i<datasize;i++){
		fread(d,sizeof(int),RECSIZE,font->fp);
#ifndef vax
		convint(&d[0]);
		for(j=1;j<RECSIZE;j++)
			convfloat(&d[j]);
#endif
		if(i == 0) count = COUNT(d[0].i);
		newcount = COUNT(d[0].i);
		if(count != newcount){
			fprintf(outfile,"closepath } E {\n");
			currentdefined = NO;
			count = newcount;
		}
		switch(TYPE(d[0].i)){
			case LINE:
				line(d[1].f,d[2].f,d[3].f,d[4].f);
				break;
			case ARC:
				arc(d[1].f,d[2].f,d[3].f,d[4].f,d[5].f);
				break;
			case ARCN:
				arcn(d[1].f,d[2].f,d[3].f,d[4].f,d[5].f);
				break;
			case CIRCLE:
				circle(d[1].f,d[2].f,d[5].f);
				break;
			case POINT:
				point(d[1].f,d[2].f);
				break;
		}
	}
	fprintf(outfile,"closepath eofill} E ] cvx def\n");
	currentdefined = NO;
	return 1;
}
/*
	Outline Font Subroutines
*/
#define PI 3.14159265359
#define rad2degree(arc) ((arc) / PI * 180.0)
double sin(),cos();

static
point(){} /* never used */

static
circle(x,y,r) float x,y,r;
{
	arc(x,y,r,0.0,360.0);
}
static
line(x1,y1,x2,y2) float x1,y1,x2,y2;
{
	if(!currentdefined){
		fprintf(outfile,"%.1f %.1f moveto\n",x1*1000,y1*1000);
		currentdefined = YES;
	}
	fprintf(outfile,"%.1f %.1f lineto\n",x2*1000,y2*1000);
}
typedef enum { NORMAL,REVERSE } Direction;
static
arc(x,y,arcs,arce,r) float x,y,arcs,arce,r;
{
	if(!currentdefined){
		fprintf(outfile,"%.1f %.1f moveto\n",
			(x+r*cos(arcs))*1000,(y+r*sin(arcs))*1000);
		currentdefined = YES;
	}
	if(arce < arcs) arce += 2 * PI; /* make sure arce > arcs */
	arcdraw(NORMAL,x,y,arcs,arce,r);
}
static
arcn(x,y,arcs,arce,r) float x,y,arcs,arce,r;
{
	if(!currentdefined){
		fprintf(outfile,"%.1f %.1f moveto\n",(x+r*cos(arce))*1000,(y+r*sin(arce))*1000);
		currentdefined = YES;
	}
	if(arce < arcs) arce += 2 * PI; /* make sure arce > arcs */
	arcdraw(REVERSE,x,y,arcs,arce,r);
}
static
arcdraw(dir,x,y,arcs,arce,r) Direction dir; float x,y,arcs,arce,r;
{
	if(dir == NORMAL)
		fprintf(outfile,"%.1f %.1f %.1f %.1f %.1f arc\n",
			x*1000,y*1000,r*1000,rad2degree(arcs),rad2degree(arce));
	else
		fprintf(outfile,"%.1f %.1f %.1f %.1f %.1f arcn\n",
			x*1000,y*1000,r*1000,rad2degree(arce),rad2degree(arcs));
}

static closeoutlinefont(font) Font *font;
{
	if(font->fp) fclose(font->fp);
	if(font->name) free(font->name);
	if(font->d) free(font->d);
	free(font);
}

Font *
openoutlinefont(fontname) char *fontname;
{
	register Font *f;
	register i,j;
	FILE *fp;

	if(! fontmatch(fontname,outlinefonts))
		return NULL;
	f = (Font *)Malloc(sizeof(Font));
	f->name = (char *)Malloc(strlen(fontname)+1);
	strcpy(f->name,fontname);
	f->_definefont = defineoutlinefont;
	f->_closefont = closeoutlinefont;
	f->d = (char *)Malloc(sizeof(Data[ROW][COLUMN]));
	fprintf(outfile,"$kanjips begin \n");
	fprintf(outfile,"/%s newoutlinefont\n",fontname);
	fprintf(outfile,"end \n");
	if((fp = openfontfile(SHARPFONTPATH,fontname,HEADEREXT)) == NULL)
		return NULL;
	fread(f->d,sizeof(Data),ROW*COLUMN,fp);
	fclose(fp);
	if(f->fp)
		fclose(f->fp);
	if((f->fp = openfontfile(SHARPFONTPATH,fontname,DATAEXT)) == NULL)
		return NULL;
#ifndef vax
	for(i=0;i<ROW;i++){
		for(j=0;j<COLUMN;j++){
			convint(&(*((Data (*)[ROW][COLUMN])(f->d)))[i][j]);
		}
	}
#endif
	return f;
}
#endif /* SHARP */
