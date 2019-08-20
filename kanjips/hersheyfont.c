#include <stdio.h>
#include "config.h"
#if HERSHEY

#include "kanjips.h"

static char *hersheyfonts[] = {
	HERSHEYFONTS,
	NULL
};

Font *
openhersheyfont(fontname) char *fontname;
{
	register Font *f;

	if(! fontmatch(fontname,hersheyfonts))
		return NULL;
	f = (Font *)Malloc(sizeof(Font));
	f->name = (char *)Malloc(strlen(fontname)+1);
	strcpy(f->name,fontname);
	f->_definefont = 
	f->_closefont = NULL;
	return f;
}
#endif /* HERSHEY */
