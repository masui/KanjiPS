/* config.h
 * This file was produced by running the config.h.SH script, which
 * gets its values from config.sh, which is generally produced by
 * running Configure.
 *
 * Feel free to modify any of this as the need arises.  Note, however,
 * that running config.h.SH again will wipe out any changes you've made.
 * For a more permanent change edit config.sh and rerun config.h.SH.
 */


/* EUNICE:
 *	This symbol, if defined, indicates that the program is being compiled
 *	under the EUNICE package under VMS.  The program will need to handle
 *	things like files that don't go away the first time you unlink them,
 *	due to version numbering.  It will also need to compensate for lack
 *	of a respectable link() command.
 */
/* VMS:
 *	This symbol, if defined, indicates that the program is running under
 *	VMS.  It is currently only set in conjunction with the EUNICE symbol.
 */
/*#undef	EUNICE		/**/
/*#undef	VMS		/**/

/* index:
 *	This preprocessor symbol is defined, along with rindex, if the system
 *	uses the strchr and strrchr routines instead.
 */
/* rindex:
 *	This preprocessor symbol is defined, along with index, if the system
 *	uses the strchr and strrchr routines instead.
 */
/*#undef	index strchr	/* cultural */
/*#undef	rindex strrchr	/*  differences? */

/*#undef FLEX	/**/

/*#undef LEX8	/**/

/*#undef PERL	/**/

#define DPI	300			/**/

#define GMW 0			/**/
#define GMWFONTPATH	""	/**/
#define GMWFONTS		/**/

#define JTEX 0			/**/
#define TEXFONTPATH	""	/**/
#define JTEXFONTS		/**/

#define LOCALFONTDIR	"/usr/local/font"			/**/

#define LWFONTS	"Times","Helvetica","Courier","Symbol"	/**/

#define SHARP 0			/**/
#define SHARPFONTPATH	""	/**/
#define SHARPFONTS		/**/

#define SHARPOA 0			/**/
#define SHARPOAFONTPATH	""	/**/
#define SHARPOAFONTS		/**/

#define X11 1			/**/
#define XFONTPATH	"/usr/local/src/kterm"	/**/
#define XFONTS	"k14"	/**/

