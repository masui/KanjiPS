#define YES 1
#define NO 0

#ifndef LOCALFONTDIR
#define LOCALFONTDIR "/usr/local/font/"
#endif

#define ROW (0x74-0x21)
#define COLUMN (0x7f-0x21)

#define CDSTR "%%"
#define DFSTR "DocumentFonts:"
#define PGSTR "Page:"

#define BUFMAX 1000
#define MAXFONTNAME 100
#define MAXPATHNAME 256
#define MAXFONTS 20
#define MAXREGIST 750

/*
	font structure
*/
typedef struct {
	char *name;		/* font name */
	FILE *fp;		/* font file pointer */
	char *d;		/* pointer to location data, etc. */
	int (*_definefont)();	/* output each character definition */
	int (*_closefont)();	/* delete font structure */
} Font;
extern FILE *infile,*outfile;

#define definefont(font,code) \
	(font->_definefont ? font->_definefont(font,code) : YES)
#define closefont(font) (font->_closefont ? font->_closefont(font) : YES)

#define strmatch(s,t) (strncmp(s,t,strlen(t)) == 0)

extern char *kcode();
extern FILE *openfontfile();
