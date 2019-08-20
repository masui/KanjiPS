#define NONEXISTANT	-1

typedef struct {		/* character entry */
	unsigned short width, height;/* width and height in pixels */
	short xOffset, yOffset;	/* x offset and y offset in pixels */
	short psfont, pschar;
	unsigned short nbpl;		/* # of bytes per pixel data line */
	union {
		int fileOffset;
		char *pixptr;
	} where;
	int tfmw;			/* TFM width */
} CharEntry;

extern CharEntry cetable[];

extern int magnification, designsize;

#define  TRUE             1
#define  FALSE            0
#define  FIRSTFNTCHAR     0
#define  LASTFNTCHAR    255
#define  NFNTCHARS      256
