#ifndef __HU_STUFF_H__
#define __HU_STUFF_H__

#define HU_FONTSTART    '!'             // the first font characters
#define HU_FONTEND      '_'             // the last font characters
#define HU_FONTSIZE     (HU_FONTEND - HU_FONTSTART + 1) // # of glyphs in font

#define HU_BROADCAST	5

#define HU_MSGREFRESH	KEY_ENTER
#define HU_MSGX		0
#define HU_MSGY		0
#define HU_MSGWIDTH	64		// in characters
#define HU_MSGHEIGHT	1		// in lines
#define HU_MSGTIMEOUT	(4*TICRATE)

#define HU_TITLE	(mapnames[(gameepisode-1)*9+gamemap-1])
#define HU_TITLE2	(mapnames2[gamemap-1])
#define HU_TITLEHEIGHT	1
#define HU_TITLEX	0
#define HU_TITLEY	(167 - SHORT(hu_font[0]->height))

#define HU_INPUTTOGGLE	't'
#define HU_INPUTX	HU_MSGX
#define HU_INPUTY	(HU_MSGY + HU_MSGHEIGHT*(SHORT(hu_font[0]->height) + 1))
#define HU_INPUTWIDTH	64
#define HU_INPUTHEIGHT	1

#endif
