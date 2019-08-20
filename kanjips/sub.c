#include <stdio.h>
#include <ctype.h>
#include "config.h"
#include "kanjips.h"

fatal(s,a,b,c) char *s,*a,*b,*c;
{
	fprintf(stderr,s,a,b,c);
	exit(0);
}
warn(s,a,b,c) char *s,*a,*b,*c;
{
	fprintf(stderr,s,a,b,c);
}
Malloc(size) int size;
{
	int i;
	if((i = malloc(size)) == NULL)
		fatal("malloc fail\n");
	return i;
}
FILE *openfontfile(pathlist,fontname,ext) char *pathlist,*fontname,*ext;
{
	char *s;
	char *dirname;
	char fontpath[MAXPATHNAME];
	int pathlen;
	FILE *fp;

	for(s = pathlist; *s; s++){
		while(isspace(*s)) s++;
		dirname = s;
		while(*s != ':' && *s != '\0'){
			s++;
		}
		pathlen = (int)(s - dirname);
		strncpy(fontpath,dirname,pathlen);
		sprintf(fontpath+pathlen,"/%s%s",fontname,ext);
		if(fp = fopen(fontpath,"r"))
			return fp;
	}
	sprintf(fontpath,"%s/%s%s",LOCALFONTDIR,fontname,ext);
	if(fp = fopen(fontpath,"r"))
		return fp;
	return NULL;
}
fontmatch(fontname,fonts) char *fontname, **fonts;
{
	register char **s;
	for(s = fonts; *s; s++){
		if(strmatch(fontname,*s))
			return YES;
	}
	return NO;
}
