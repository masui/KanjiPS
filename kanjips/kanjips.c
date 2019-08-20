/*
	Kanji PostScript Filter

				1989 2/21 MASUI Toshiyuki
	
	%%DefineFonts: fontname
*/
#include <stdio.h>
#include <ctype.h>
#include "config.h"
#include "kanjips.h"

static Font *fonts[MAXFONTS];	/* font data structure */
static int nfonts = 0;		/* number of Kanji fonts being used */

unsigned char *zs;	/* for state machine zztrans() */

FILE *infile = stdin,*outfile = stdout;

extern Font *openfont();

main(argc,argv) char **argv;
{
	unsigned char linebuf[BUFMAX];
	int line;

	switch(argc){
		case 1:
			break;
		case 2:
			if((infile = fopen(argv[1],"r")) == NULL){
				fprintf(stderr,"Can't open %s\n",argv[1]);
				exit(1);
			}
			break;
		default:
			fprintf(stderr,"kanjips [filename]\n");
			exit(1);
	}
	zzinit(0);	/* reset the state machine */
	for(line = 0; fgets(linebuf,BUFMAX,infile); line++){
		if(line == 1){
			outheader();
		}
		if(strmatch(linebuf,CDSTR)){
			fprintf(outfile,"%s",linebuf);
			conform(linebuf);
		}
		else { /* normal line */
			for(zs=linebuf; *zs; zs++){
				zztrans(*zs,0); /* process 1 line */
			}
		}
	}
}
conform(s) unsigned char *s;
/*
	Treat a comment line in Conforming Documents
*/
{
	register Font *font;
	unsigned char *fontname;

	s += strlen(CDSTR);
	while(isspace(*s)) s++;
	if(strmatch(s,DFSTR)){ /* %%DocumentFonts */
		s += strlen(DFSTR);
		forgetfont();
		for(nfonts = 0; *s && nfonts < MAXFONTS; s++){
			while(isspace(*s)) s++;
			fontname = s;
			while(!isspace(*s)) s++;
			*s = '\0';
			if(strmatch(fontname,"(atend)")){
				fprintf(stderr,
					"Used fonts must be declared first!\n");
				exit(1);
			}
			if((font = openfont(fontname)) == NULL){
				fprintf(stderr,
					"font %s not found\n",fontname);
				exit(1);
			}
			fonts[nfonts++] = font;
		}
		forgetdef();
	}
	else if(strmatch(s,PGSTR)){ /* %%Page */
		forgetdef();
	}
}
forgetfont()
{
	register i;
	register Font *font;
	for(i=0; i< nfonts; i++){
		if((font = fonts[i]) == NULL) continue;
		closefont(font);
		fonts[i] = NULL;
	}
}

static int registcodes[MAXREGIST];	/* remembered font codes */
static int ncodes = 0;			/* number of remembered fonts */

static regist(code) int code;
{
	register i;
	for(i=0; i<ncodes; i++){
		if(registcodes[i] == code) return;
	}
	fprintf(outfile,"$kdata begin\n");
	for(i=0; i<nfonts; i++){
		definefont(fonts[i],code);
	}
	fprintf(outfile,"end\n");
	registcodes[ncodes++] = code;
}
static forgetdef()
{
	ncodes = 0;	/* forget about sent data */
}
/*
	State Machine Definition
*/
static char strbuf[BUFMAX];
#define ZZMAXSEQ 1024
#define PRINT() ZZCHAR[ZZCHARS]='\0'; fprintf(outfile,"%s",ZZCHAR);
#define CAT() ZZCHAR[ZZCHARS]='\0'; strcat(strbuf,ZZCHAR);
#define COPY() ZZCHAR[ZZCHARS]='\0'; strcpy(strbuf,ZZCHAR);
#define PUSHBACK() zs--
#define ZZDEFAULT fprintf(outfile,"<?>")
static int jiscode[] = {
	/* sp   maru   kagi  kakko   ten   nakaten   wo     A */
/* a */	0xa1a0,0xa1a3,0xa1d6,0xa1e7,0xa1a2,0xa1a6,0xa5f2,0xa5a1,
	/* I      U      E      O     YA     YU     YO     TU */
	0xa5a3,0xa5a5,0xa5a7,0xa5a9,0xa5e3,0xa5e5,0xa5e7,0xa5c3,
	/* bou    a      i      u      e      o     ka     ki */
/* b */	0xa1bc,0xa5a2,0xa5a4,0xa5a6,0xa5a8,0xa5aa,0xa5ab,0xa5ad,
	/* ku    ke     ko     sa     si     su     se     so */
	0xa5af,0xa5b1,0xa5b3,0xa5b5,0xa5b7,0xa5b9,0xa5bb,0xa5bd,
	/* ta    ti     tu     te     to     na     ni     nu */
/* c */	0xa5bf,0xa5c1,0xa5c4,0xa5c6,0xa5c8,0xa5ca,0xa5cb,0xa5cc,
	/* ne    no     ha     hi     hu     he     ho     ma */
	0xa5cd,0xa5ce,0xa5cf,0xa5d2,0xa5d5,0xa5d8,0xa5db,0xa5de,
	/* mi    mu     me     mo     ya     yu     yo     ra */
/* d */	0xa5df,0xa5e0,0xa5e1,0xa5e2,0xa5e4,0xa5e6,0xa5e8,0xa5e9,
	/* ri    ru     re     ro     wa      n  dakuten handaku */
	0xa5ea,0xa5eb,0xa5ec,0xa5ed,0xa5ef,0xa5f3,0xa1ab,0xa1ac
};
#define BEGIN zzsubstate[zzn] =

#define ZZSUBSTATES 4
int zzsubstate[ZZSUBSTATES];

#define HEX 1
#define JIS 2
#define STRING 3


#ifndef ZZMAXSEQ
#define ZZMAXSEQ 20
#endif

#ifndef ZZMACHINES
#define ZZMACHINES 1
#endif

struct zzstm0{
	int zzstate0;
	unsigned char zzseq0[ZZMAXSEQ];
	int zznth0;
};

struct zzstm0 zzstms0[ZZMACHINES];

#define ZZCHARS (zzstms0[zzn].zznth0)
#define ZZCHAR (zzstms0[zzn].zzseq0)

#ifndef ZZDEFAULT
#define ZZDEFAULT putchar(ZZCHAR[0]);
#endif


zzaction0(zzact0,zzn) int zzact0,zzn;
{
switch(zzact0){
case 0: ZZDEFAULT;
break;
case 1:
{ PRINT(); }
break;
case 2:
{ unsigned char c = ZZCHAR[ZZCHARS-1];
					  ZZCHAR[ZZCHARS-1] = '\0';
					  PRINT();
					  PUSHBACK(); }
break;
case 3:
{ PRINT(); }
break;
case 4:
{ PRINT(); }
break;
case 5:
{ PRINT(); }
break;
case 6:
{ COPY(); BEGIN STRING; }
break;
case 7:
{ PRINT(); BEGIN HEX; }
break;
}
return 0; /* normal return value == 0 */
}

static unsigned char zzctype0[] = {
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  1,  0,  0,  0,  0,  3,  0,  0,  4,  5,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,  0,  5,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  0,  8,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } ;

static short zznstate0[] = {
	  1,  3,  3,  2,  4,  0,  5,  6,  7,  1,  8,  8,  8,  8,  8,  8,
	  8,  8,  2,  2,  9,  2,  2,  2,  2,  2,  2 } ;

static short zzpstate0[] = {
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2 } ;

static short zznindex0[] = {
	  0,  9, 18 } ;

static short zzdeftrans0[] = {
	  0,  0,  0 } ;

static short zzout0[] = {
	  1,  6,  7,  3,  4,  2,  5 } ;

zzinit0(n) int n;
{
	zzstms0[n].zzstate0 = zzstms0[n].zznth0 = 0;
}

zztrans0(c,n) int c; int n;
{
	int *state;
	int index; 
 	state = &(zzstms0[n].zzstate0);
	if(zzdeftrans0[*state])
		*state = zzdeftrans0[*state];
	else{
		index = zznindex0[*state] + zzctype0[c];
		if(index < 0 || index >= 27) *state=0;
		else *state = (zzpstate0[index] == *state ? zznstate0[index] : 0 );
	}
 	(zzstms0[n].zzseq0)[zzstms0[n].zznth0++] = (unsigned char) c;
	if(*state == 0){ /* default action */
		zzaction0(0,n);
		zzstms0[n].zznth0 = 0;
		return 0;
	}
	if(*state > 2){ /* accept */
		zzaction0(zzout0[*state-3],n);
		*state = 0;
		zzstms0[n].zznth0 = 0;
		return 1;
	}
	return 0;
}

#undef ZZCHARS
#undef ZZCHAR


#ifndef ZZMAXSEQ
#define ZZMAXSEQ 20
#endif

#ifndef ZZMACHINES
#define ZZMACHINES 1
#endif

struct zzstm1{
	int zzstate1;
	unsigned char zzseq1[ZZMAXSEQ];
	int zznth1;
};

struct zzstm1 zzstms1[ZZMACHINES];

#define ZZCHARS (zzstms1[zzn].zznth1)
#define ZZCHAR (zzstms1[zzn].zzseq1)

#ifndef ZZDEFAULT
#define ZZDEFAULT putchar(ZZCHAR[0]);
#endif


zzaction1(zzact1,zzn) int zzact1,zzn;
{
switch(zzact1){
case 0: ZZDEFAULT;
break;
case 1:
{ PRINT(); }
break;
case 2:
{ PRINT(); }
break;
case 3:
{ PRINT(); BEGIN 0; }
break;
}
return 0; /* normal return value == 0 */
}

static unsigned char zzctype1[] = {
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  2,  0,
	  0,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } ;

static short zznstate1[] = {
	  1,  2,  3 } ;

static short zzpstate1[] = {
	  0,  0,  0 } ;

static short zznindex1[] = {
	  0 } ;

static short zzdeftrans1[] = {
	  0 } ;

static short zzout1[] = {
	  2,  1,  3 } ;

zzinit1(n) int n;
{
	zzstms1[n].zzstate1 = zzstms1[n].zznth1 = 0;
}

zztrans1(c,n) int c; int n;
{
	int *state;
	int index; 
 	state = &(zzstms1[n].zzstate1);
	if(zzdeftrans1[*state])
		*state = zzdeftrans1[*state];
	else{
		index = zznindex1[*state] + zzctype1[c];
		if(index < 0 || index >= 3) *state=0;
		else *state = (zzpstate1[index] == *state ? zznstate1[index] : 0 );
	}
 	(zzstms1[n].zzseq1)[zzstms1[n].zznth1++] = (unsigned char) c;
	if(*state == 0){ /* default action */
		zzaction1(0,n);
		zzstms1[n].zznth1 = 0;
		return 0;
	}
	if(*state > 0){ /* accept */
		zzaction1(zzout1[*state-1],n);
		*state = 0;
		zzstms1[n].zznth1 = 0;
		return 1;
	}
	return 0;
}

#undef ZZCHARS
#undef ZZCHAR


#ifndef ZZMAXSEQ
#define ZZMAXSEQ 20
#endif

#ifndef ZZMACHINES
#define ZZMACHINES 1
#endif

struct zzstm2{
	int zzstate2;
	unsigned char zzseq2[ZZMAXSEQ];
	int zznth2;
};

struct zzstm2 zzstms2[ZZMACHINES];

#define ZZCHARS (zzstms2[zzn].zznth2)
#define ZZCHAR (zzstms2[zzn].zzseq2)

#ifndef ZZDEFAULT
#define ZZDEFAULT putchar(ZZCHAR[0]);
#endif


zzaction2(zzact2,zzn) int zzact2,zzn;
{
switch(zzact2){
case 0: ZZDEFAULT;
break;
case 1:
{ BEGIN STRING; }
break;
case 2:
{
					  sprintf(strbuf+strlen(strbuf),
						"\\%03o\\%03o",
						ZZCHAR[0]+0x80,ZZCHAR[1]+0x80);
					  regist(((ZZCHAR[0]+0x80)<<8)+
						ZZCHAR[1]+0x80);
					}
break;
}
return 0; /* normal return value == 0 */
}

static unsigned char zzctype2[] = {
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } ;

static short zznstate2[] = {
	  1,  3,  1,  2 } ;

static short zzpstate2[] = {
	  0,  0,  0,  3 } ;

static short zznindex2[] = {
	  0,  0,  0,  1 } ;

static short zzdeftrans2[] = {
	  0,  4,  5,  0 } ;

static short zzout2[] = {
	  2,  1 } ;

zzinit2(n) int n;
{
	zzstms2[n].zzstate2 = zzstms2[n].zznth2 = 0;
}

zztrans2(c,n) int c; int n;
{
	int *state;
	int index; 
 	state = &(zzstms2[n].zzstate2);
	if(zzdeftrans2[*state])
		*state = zzdeftrans2[*state];
	else{
		index = zznindex2[*state] + zzctype2[c];
		if(index < 0 || index >= 4) *state=0;
		else *state = (zzpstate2[index] == *state ? zznstate2[index] : 0 );
	}
 	(zzstms2[n].zzseq2)[zzstms2[n].zznth2++] = (unsigned char) c;
	if(*state == 0){ /* default action */
		zzaction2(0,n);
		zzstms2[n].zznth2 = 0;
		return 0;
	}
	if(*state > 3){ /* accept */
		zzaction2(zzout2[*state-4],n);
		*state = 0;
		zzstms2[n].zznth2 = 0;
		return 1;
	}
	return 0;
}

#undef ZZCHARS
#undef ZZCHAR


#ifndef ZZMAXSEQ
#define ZZMAXSEQ 20
#endif

#ifndef ZZMACHINES
#define ZZMACHINES 1
#endif

struct zzstm3{
	int zzstate3;
	unsigned char zzseq3[ZZMAXSEQ];
	int zznth3;
};

struct zzstm3 zzstms3[ZZMACHINES];

#define ZZCHARS (zzstms3[zzn].zznth3)
#define ZZCHAR (zzstms3[zzn].zzseq3)

#ifndef ZZDEFAULT
#define ZZDEFAULT putchar(ZZCHAR[0]);
#endif


zzaction3(zzact3,zzn) int zzact3,zzn;
{
switch(zzact3){
case 0: ZZDEFAULT;
break;
case 1:
{
					  CAT(); 
					  fprintf(outfile,"%s",strbuf);
					  BEGIN 0;
					}
break;
case 2:
{ CAT(); }
break;
case 3:
{ CAT(); }
break;
case 4:
{ BEGIN JIS; }
break;
case 5:
{  /* EUC Kanji */
					  sprintf(strbuf+strlen(strbuf),
						"\\%03o\\%03o",
					       ZZCHAR[0],ZZCHAR[1]);
					  regist((ZZCHAR[0]<<8)+ZZCHAR[1]);
					}
break;
case 6:
{  /* EUC Kana */
					  sprintf(strbuf+strlen(strbuf),
						"\\%03o\\%03o",
					       ZZCHAR[0],ZZCHAR[1]);
					  regist(jiscode[ZZCHAR[1]-0xa0]);
					}
break;
case 7:
{ CAT(); }
break;
}
return 0; /* normal return value == 0 */
}

static unsigned char zzctype3[] = {
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,
	  0,  0,  0,  0,  2,  0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,
	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  0 } ;

static short zznstate3[] = {
	  9,  7,  9,  8,  9,  4,  2,  3, 13, 13, 13, 13,  6, 13, 13, 13,
	  1 } ;

static short zzpstate3[] = {
	  0,  0,  0,  0,  0,  0,  0,  0,  4,  4,  4,  4,  4,  4,  4,  4,
	  7 } ;

static short zznindex3[] = {
	  0,  0,  0,  0,  8,  0,  0, 14 } ;

static short zzdeftrans3[] = {
	  0, 12, 10, 11,  0, 14,  5,  0 } ;

static short zzout3[] = {
	  1,  7,  6,  5,  4,  3,  2 } ;

zzinit3(n) int n;
{
	zzstms3[n].zzstate3 = zzstms3[n].zznth3 = 0;
}

zztrans3(c,n) int c; int n;
{
	int *state;
	int index; 
 	state = &(zzstms3[n].zzstate3);
	if(zzdeftrans3[*state])
		*state = zzdeftrans3[*state];
	else{
		index = zznindex3[*state] + zzctype3[c];
		if(index < 0 || index >= 17) *state=0;
		else *state = (zzpstate3[index] == *state ? zznstate3[index] : 0 );
	}
 	(zzstms3[n].zzseq3)[zzstms3[n].zznth3++] = (unsigned char) c;
	if(*state == 0){ /* default action */
		zzaction3(0,n);
		zzstms3[n].zznth3 = 0;
		return 0;
	}
	if(*state > 7){ /* accept */
		zzaction3(zzout3[*state-8],n);
		*state = 0;
		zzstms3[n].zznth3 = 0;
		return 1;
	}
	return 0;
}

#undef ZZCHARS
#undef ZZCHAR

#if c_plusplus
typedef int (*ZZFUNC)(int,int);
#else
typedef int (*ZZFUNC)();
#endif
static ZZFUNC zzfuncs[] = {
	zztrans0,
	zztrans1,
	zztrans2,
	zztrans3,
	0
};

zzinit(zzn) int zzn;
{
	zzinit0(zzn);
	zzinit1(zzn);
	zzinit2(zzn);
	zzinit3(zzn);
	zzsubstate[zzn] = 0;
}

zztrans(zzact,zzn) int zzact,zzn;
{
	return (*zzfuncs[zzsubstate[zzn]])(zzact,zzn);
}

