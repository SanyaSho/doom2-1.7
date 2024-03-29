#ifndef __AMOIDS__
#define __AMOIDS__

#define AMO_MAXBULLETS 10
#define AMO_MAXROCKS 20
#define AMO_MAXPOLYPOINTS 8

typedef struct
{
  mpoint_t p[AMO_MAXPOLYPOINTS];
} amo_polygon_t;

typedef struct
{
  fixed_t x, y, r2;
  int angle;
  amo_polygon_t poly;
} amo_rock_t;

typedef struct
{
  fixed_t x, y, vx, vy;
} amo_bullet_t;

#endif
