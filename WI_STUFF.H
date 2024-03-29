#ifndef __WBSTUFF__
#define __WBSTUFF__

#include "doomdef.h"

#define NUMEPISODES 3
#define NUMMAPS 9
#define PAUSELEN (TICRATE*2) // in tics
#define SCORESTEP 100
#define SHOWNEXTLOCDELAY 4 // in seconds
#define SHOWLASTLOCDELAY SHOWNEXTLOCDELAY // in seconds
#define ANIMPERIOD 32 // in tics
#define STARDIST 10 // pixel distance from "(YOU)" to "PLAYER N"

#define FB 0
#define WK 1

// States for the intermission

typedef enum { NoState = -1,
               StatCount,
               ShowNextLoc
	     } stateenum_t;

// States for single-player

#define SP_KILLS	0
#define SP_ITEMS	2
#define SP_SECRET	4
#define SP_FRAGS	6 
#define SP_TIME		8 
#define SP_PAR		ST_TIME
#define SP_PAUSE	1

typedef struct
{
  int x, y;
} point_t;

typedef enum { ANIM_ALWAYS, ANIM_RANDOM, ANIM_LEVEL } animenum_t;

typedef struct
{
  animenum_t type;
  int period; // period in tics between animations
  int nanims; // number of animation frames
  point_t loc;// location of animation
  int data1; // ALWAYS: n/a, RANDOM: period deviation (<256), LEVEL: level
  int data2; // ALWAYS: n/a, RANDOM: random base period, LEVEL: n/a
  patch_t *p[3]; // actual graphics for frames of animations

  // following must be initialized to zero before use!
  int nexttic;// next value of bcnt (used in conjunction with period)
  int lastdrawn; // last drawn animation frame
  int ctr;    // next frame number to animate
  int state;  // used by RANDOM and LEVEL when animating
} anim_t;

#endif
