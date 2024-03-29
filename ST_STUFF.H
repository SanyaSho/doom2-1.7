#ifndef __STSTUFF_H__
#define __STSTUFF_H__

// For damage/bonus red-/gold-shifts
#define    STARTREDPALS    1
#define    STARTBONUSPALS  9
#define    NUMREDPALS      8
#define    NUMBONUSPALS    4
#define		RADIATIONPAL	13

// N/256*100% probability that the normal face state will change
#define ST_FACEPROBABILITY 96

// for Responder
#define    ST_TOGGLECHAT   KEY_ENTER

// Location/size of status bar
#define ST_WIDTH    SCREENWIDTH
#define ST_HEIGHT   32
#define ST_X        0
#define ST_X2       104
#define ST_Y        (SCREENHEIGHT - ST_HEIGHT)
#define ST_FX	    143
#define ST_FY	    169

// Should be set to patch width for tall numbers later on
#define ST_TALLNUMWIDTH (tallnum[0]->width)

// number of status faces
#define ST_NUMPAINFACES 5
#define ST_NUMSTRAIGHTFACES 3
#define ST_NUMTURNFACES 2
#define ST_NUMSPECIALFACES 3
#define ST_FACESTRIDE (ST_NUMSTRAIGHTFACES+ST_NUMTURNFACES+ST_NUMSPECIALFACES)
#define ST_NUMEXTRAFACES 2
#define ST_NUMFACES (ST_FACESTRIDE*ST_NUMPAINFACES+ST_NUMEXTRAFACES)
#define ST_TURNOFFSET (ST_NUMSTRAIGHTFACES)
#define ST_OUCHOFFSET (ST_TURNOFFSET + ST_NUMTURNFACES)
#define ST_EVILGRINOFFSET (ST_OUCHOFFSET + 1)
#define ST_RAMPAGEOFFSET (ST_EVILGRINOFFSET + 1)
#define ST_GODFACE (ST_NUMPAINFACES*ST_FACESTRIDE)
#define ST_DEADFACE (ST_GODFACE+1)
#define ST_FACESX   143
#define ST_FACESY   168
#define ST_EVILGRINCOUNT (2*TICRATE)
#define ST_STRAIGHTFACECOUNT (TICRATE/2)
#define ST_TURNCOUNT (1*TICRATE)
#define ST_OUCHCOUNT (1*TICRATE)
#define ST_RAMPAGEDELAY (2*TICRATE)
#define ST_MUCHPAIN 20

// location and size of statistics, justified according to widget type
#define ST_AMMOWIDTH    3	// AMMO number pos.
#define	ST_AMMOX        44
#define	ST_AMMOY        171
#define ST_HEALTHWIDTH  3	// HEALTH number pos.
#define	ST_HEALTHX      90
#define	ST_HEALTHY      171
#define ST_ARMSX	111
#define ST_ARMSY	172
#define ST_ARMSBGX	104
#define ST_ARMSBGY	168
#define ST_ARMSXSPACE	12
#define ST_ARMSYSPACE	10
#define ST_FRAGSX	138
#define ST_FRAGSY	171
#define ST_FRAGSWIDTH	2
#define ST_ARMORWIDTH   3	// ARMOR number pos.
#define	ST_ARMORX       221
#define	ST_ARMORY       171
#define ST_KEY0WIDTH    8
#define ST_KEY0HEIGHT   5
#define	ST_KEY0X        239
#define	ST_KEY0Y        171
#define ST_KEY1WIDTH    ST_KEY0WIDTH
#define	ST_KEY1X        239
#define	ST_KEY1Y        181
#define ST_KEY2WIDTH    ST_KEY0WIDTH
#define	ST_KEY2X        239
#define	ST_KEY2Y        191
#define ST_AMMO0WIDTH   3
#define ST_AMMO0HEIGHT  6
#define ST_AMMO0X       288
#define ST_AMMO0Y       173
#define ST_AMMO1WIDTH   ST_AMMO0WIDTH
#define ST_AMMO1X       288
#define ST_AMMO1Y       179
#define ST_AMMO2WIDTH   ST_AMMO0WIDTH
#define ST_AMMO2X       288
#define ST_AMMO2Y       191
#define ST_AMMO3WIDTH   ST_AMMO0WIDTH
#define ST_AMMO3X       288
#define ST_AMMO3Y       185
#define ST_MAXAMMO0WIDTH   3
#define ST_MAXAMMO0HEIGHT  5
#define ST_MAXAMMO0X       314
#define ST_MAXAMMO0Y       173
#define ST_MAXAMMO1WIDTH   ST_MAXAMMO0WIDTH
#define ST_MAXAMMO1X       314
#define ST_MAXAMMO1Y       179
#define ST_MAXAMMO2WIDTH   ST_MAXAMMO0WIDTH
#define ST_MAXAMMO2X       314
#define ST_MAXAMMO2Y       191
#define ST_MAXAMMO3WIDTH   ST_MAXAMMO0WIDTH
#define ST_MAXAMMO3X       314
#define ST_MAXAMMO3Y       185
#define ST_WEAPON0X	   110 // pistol
#define ST_WEAPON0Y	   172
#define ST_WEAPON1X	   122 // shotgun
#define ST_WEAPON1Y	   172
#define ST_WEAPON2X	   134 // chain gun
#define ST_WEAPON2Y	   172
#define ST_WEAPON3X	   110 // missile launcher
#define ST_WEAPON3Y	   181
#define ST_WEAPON4X	   122 // plasma gun
#define ST_WEAPON4Y	   181
#define ST_WEAPON5X	   134 // bfg
#define ST_WEAPON5Y	   181
#define ST_WPNSX	   109 // WPNS title
#define ST_WPNSY	   191
#define ST_DETHX	   109 // DETH title
#define ST_DETHY	   191

// incoming messages window location
//#define ST_MSGTEXTX	   (viewwindowx)
//#define ST_MSGTEXTY	   (viewwindowy+viewheight-18)
#define ST_MSGTEXTX	   0
#define ST_MSGTEXTY	   0
#define ST_MSGWIDTH	   52  // in characters
#define ST_MSGHEIGHT	   1   // in lines
#define ST_OUTTEXTX	   0
#define ST_OUTTEXTY	   6
#define ST_OUTWIDTH	   52  // in characters
#define ST_OUTHEIGHT	   1   // in lines
#define ST_MAPWIDTH	   (strlen(mapnames[(gameepisode-1)*9+(gamemap-1)]))
#define ST_MAPTITLEX	   (SCREENWIDTH - ST_MAPWIDTH * ST_CHATFONTWIDTH)
#define ST_MAPTITLEY	   0
#define ST_MAPHEIGHT	   1

// states for status bar code
typedef enum
{
  AutomapState,
  FirstPersonState
} st_stateenum_t;

// states for the chat code
typedef enum
{
  StartChatState,
  WaitDestState,
  GetChatState
} st_chatstateenum_t;

extern weaponinfo_t weaponinfo[]; // found in P_pspr.c

boolean ST_Responder(event_t *ev);

#endif
