#include "doomdef.h"
#include "p_local.h"


//==================================================================
//
//						TELEPORTATION
//
//==================================================================
int	EV_Teleport( line_t *line,int side,mobj_t *thing )
{
	int		i;
	int		tag;
	mobj_t	*m,*fog;
	unsigned	an;
	thinker_t	*thinker;
	sector_t	*sector;
	fixed_t		oldx, oldy, oldz;
		
	if (thing->flags & MF_MISSILE)
		return 0;			// don't teleport missiles
		
	if (side == 1)		// don't teleport if hit back of line,
		return 0;		// so you can get out of teleporter
	
	tag = line->tag;
	for (i = 0; i < numsectors; i++)
		if (sectors[ i ].tag == tag )
		{
			thinker = thinkercap.next;
			for (thinker = thinkercap.next ; thinker != &thinkercap 
			; thinker = thinker->next)
			{
				if (thinker->function != P_MobjThinker)
					continue;		// not a mobj
				m = (mobj_t *)thinker;
				if (m->type != MT_TELEPORTMAN )
					continue;		// not a teleportman
				sector = m->subsector->sector;
				if (sector-sectors != i )
					continue;		// wrong sector

				oldx = thing->x;
				oldy = thing->y;
				oldz = thing->z;
				
				if (!P_TeleportMove (thing, m->x, m->y))
					return 0;

				thing->z = thing->floorz;
				if (thing->player)
					thing->player->viewz = thing->z+thing->player->viewheight;
				
// spawn teleport fog at source and destination

				fog = P_SpawnMobj (oldx, oldy, oldz, MT_TFOG);
				S_StartSound (fog, sfx_telept);
				an = m->angle >> ANGLETOFINESHIFT;
				fog = P_SpawnMobj (m->x+20*finecosine[an], m->y+20*finesine[an]
					, thing->z, MT_TFOG);
				S_StartSound (fog, sfx_telept);
				if (thing->player)
					thing->reactiontime = 18;	// don't move for a bit
				thing->angle = m->angle;
				thing->momx = thing->momy = thing->momz = 0;
				return 1;
			}	
		}
	return 0;
}

