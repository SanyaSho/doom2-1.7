// the asteroids-ish game inside of the automap

#include "DoomDef.h"
#include "AM_map.h"
#include "AM_oids.h"

fixed_t ff_x, ff_y, ff_w, ff_h;

amo_bullet_t amo_bullets[AMO_MAXBULLETS];

boolean AM_oInRock(amo_rock_t *rock, fixed_t x, fixed_t y)
{
  fixed_t dx2, dy2;
  boolean rc = false;

  dx2 = FixedMul(x-rock->x, x-rock->x);
  dy2 = FixedMul(y-rock->y, y-rock->y);
  if (dx2+dy2 <= rock->r2) rc = true;

  return rc;
}

void AM_oUpdateBullets(void)
{
  int i;
  amo_bullet_t *a;

  for (a=amo_bullets;a<amo_bullets + AMO_MAXBULLETS;a++)
  {
    if (a->x != -1)
    {
      // move bullet
      a->x += a->vx;
      a->y += a->vy;
      // destroy bullet if it left the screen
      if (a->x < ff_x || a->x >= ff_x + ff_w ||  a->y < ff_y
	  || a->y >= ff_y + ff_h)
      {
	a->x = -1;
	continue;
      }
      // check for collision with any rock
      for (r=amo_rocks;r<amo_rocks + AMO_MAXROCKS;r++)
      {
	if (r->x != -1 && AM_oInRock(r, a->x, a->y))
	{
	  a->x = r->x = -1; // if collision, destroy bullet and rock
	  break;
	}
      }
    }
  }
}

void AM_oTicker(void)
{
  AM_oUpdateBullets();
  AM_oUpdateShip();
  AM_oUpdateRocks();
  AM_oUpdateShrapnel();
}

void AM_oDrawer(void)
{
  
}

void AM_oInitData(void)
{
  int i;
  for (i=0;i<AMO_MAXBULLETS;i++) amo_bullets[i].x = -1;
  for (i=0;i<AMO_MAXROCKS;i++) amo_rocks[i].x = -1;
  ff_x = f_x << FRACBITS;
  ff_y = f_y << FRACBITS;
  ff_w = f_w << FRACBITS;
  ff_h = f_h << FRACBITS;
}

void AM_oStart(void)
{
  AM_oInitData();
}
