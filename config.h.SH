case $CONFIG in
'')
    if test ! -f config.sh; then
	ln ../config.sh . || \
	ln ../../config.sh . || \
	ln ../../../config.sh . || \
	(echo "Can't find config.sh."; exit 1)
	echo "Using config.sh from above..."
    fi
    . ./config.sh
    ;;
esac
echo "Extracting config.h (with variable substitutions)"
sed <<!GROK!THIS! >config.h -e 's!^#undef!/\*#undef!'
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
#$d_eunice	EUNICE		/**/
#$d_eunice	VMS		/**/

/* index:
 *	This preprocessor symbol is defined, along with rindex, if the system
 *	uses the strchr and strrchr routines instead.
 */
/* rindex:
 *	This preprocessor symbol is defined, along with index, if the system
 *	uses the strchr and strrchr routines instead.
 */
#$d_index	index strchr	/* cultural */
#$d_index	rindex strrchr	/*  differences? */

#$d_flex FLEX	/**/

#$d_lex8 LEX8	/**/

#$d_perl PERL	/**/

#define DPI	$dpi			/**/

#define GMW $gmw			/**/
#define GMWFONTPATH	"$gmwfontpath"	/**/
#define GMWFONTS	$gmwfontlist	/**/

#define JTEX $jtex			/**/
#define TEXFONTPATH	"$texfontpath"	/**/
#define JTEXFONTS	$jtexfontlist	/**/

#define LOCALFONTDIR	"$localfont"			/**/

#define LWFONTS	$lwfontlist	/**/

#define SHARP $sharp			/**/
#define SHARPFONTPATH	"$sharpfontpath"	/**/
#define SHARPFONTS	$sharpfontlist	/**/

#define SHARPOA $sharpoa			/**/
#define SHARPOAFONTPATH	"$sharpoafontpath"	/**/
#define SHARPOAFONTS	$sharpoafontlist	/**/

#define X11 $x11			/**/
#define XFONTPATH	"$xfontpath"	/**/
#define XFONTS	$xfontlist	/**/

!GROK!THIS!
