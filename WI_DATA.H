#ifndef __WIDATA__
#define __WIDATA__

#include "wi_stuff.h"

//     GLOBAL LOCATIONS

#define WI_TITLEY	2
#define WI_SPACINGY	33

//	SINGPLE-PLAYER STUFF

#define SP_STATSX	50
#define SP_STATSY	50

#define SP_TIMEX	16
#define SP_TIMEY	(SCREENHEIGHT-32)

//	NET GAME STUFF

#define NG_STATSY	50
#define NG_STATSX	(32 + SHORT(star->width)/2 + 32*!dofrags)

#define NG_SPACINGX	64

//	DEATHMATCH STUFF

#define DM_MATRIXX	42
#define DM_MATRIXY	68

#define DM_SPACINGX	40

#define DM_TOTALSX	269

#define DM_KILLERSX	10
#define DM_KILLERSY	100
#define DM_VICTIMSX	5
#define DM_VICTIMSY	50


static point_t lnodes[NUMEPISODES][NUMMAPS] =
{

  // Episode 0 World Map

  {
    { 185, 164 }, // location of level 0 (CJ)
    { 148, 143 }, // location of level 1 (CJ)
    { 69, 122 }, // location of level 2 (CJ)
    { 209, 102 }, // location of level 3 (CJ)
    { 116, 89 }, // location of level 4 (CJ)
    { 166, 55 }, // location of level 5 (CJ)
    { 71, 56 }, // location of level 6 (CJ)
    { 135, 29 }, // location of level 7 (CJ)
    { 71, 24 } // location of level 8 (CJ)
  },

  // Episode 1 World Map should go here

  {
    { 254, 25 }, // location of level 0 (CJ)
    { 97, 50 }, // location of level 1 (CJ)
    { 188, 64 }, // location of level 2 (CJ)
    { 128, 78 }, // location of level 3 (CJ)
    { 214, 92 }, // location of level 4 (CJ)
    { 133, 130 }, // location of level 5 (CJ)
    { 208, 136 }, // location of level 6 (CJ)
    { 148, 140 }, // location of level 7 (CJ)
    { 235, 158 } // location of level 8 (CJ)
  },

  // Episode 2 World Map should go here

  {
    { 156, 168 }, // location of level 0 (CJ)
    { 48, 154 }, // location of level 1 (CJ)
    { 174, 95 }, // location of level 2 (CJ)
    { 265, 75 }, // location of level 3 (CJ)
    { 130, 48 }, // location of level 4 (CJ)
    { 279, 23 }, // location of level 5 (CJ)
    { 198, 48 }, // location of level 6 (CJ)
    { 140, 25 }, // location of level 7 (CJ)
    { 281, 136 } // location of level 8 (CJ)
  }

};

// animation locations for episode 0 (1)

static anim_t epsd0animinfo[] =
{
  { ANIM_ALWAYS, TICRATE/3, 3, { 224, 104 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 184, 160 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 112, 136 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 72, 112 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 88, 96 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 64, 48 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 192, 40 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 136, 16 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 80, 16 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 64, 24 } }
};

static anim_t epsd1animinfo[] =
{
  { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 1 },
  { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 2 },
  { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 3 },
  { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 4 },
  { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 5 },
  { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 6 },
  { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 7 },
  { ANIM_LEVEL, TICRATE/3, 3, { 192, 144 }, 8 },
  { ANIM_LEVEL, TICRATE/3, 1, { 128, 136 }, 8 }
};

static anim_t epsd2animinfo[] =
{
  { ANIM_ALWAYS, TICRATE/3, 3, { 104, 168 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 40, 136 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 160, 96 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 104, 80 } },
  { ANIM_ALWAYS, TICRATE/3, 3, { 120, 32 } },
  { ANIM_ALWAYS, TICRATE/4, 3, { 40, 0 } }
};

static int NUMANIMS[NUMEPISODES] =
{
  sizeof(epsd0animinfo)/sizeof(anim_t),
  sizeof(epsd1animinfo)/sizeof(anim_t),
  sizeof(epsd2animinfo)/sizeof(anim_t)
};

static anim_t *anims[NUMEPISODES] =
{
  epsd0animinfo,
  epsd1animinfo,
  epsd2animinfo
};

#endif
