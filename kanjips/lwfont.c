#include <stdio.h>
#include "config.h"
#include "kanjips.h"

static char *lwfonts[] = {
	LWFONTS,
	NULL
};

Font *
openlwfont(fontname) char *fontname;
{
	register Font *f;

	if(! fontmatch(fontname,lwfonts))
		return NULL;
	f = (Font *)Malloc(sizeof(Font));
	f->name = (char *)Malloc(strlen(fontname)+1);
	strcpy(f->name,fontname);
	f->_definefont = 
	f->_closefont = NULL;
	return f;
}
