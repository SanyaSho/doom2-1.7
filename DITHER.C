// dither.c

#include "dither.h"

#define	NUMGRAYS	49
unsigned char	dither[NUMGRAYS][4];
unsigned char	*ditherpalette[256];

int		order[16][2] = {
{0,0}, {2,2}, {2,0}, {0,2}, {1,1}, {3,3}, {3,1}, {1,3},
{1,0}, {3,2}, {3,0}, {1,2}, {0,1}, {2,3}, {2,1}, {0,3} };

/*
===============
=
= MakeDither
=
= Creates NUMGRAYS bit patterns
= Must be called before any updates
===============
*/

void MakeDither (void)
{
	int		b,g,o,x,y,dgray;
	unsigned char	dith[4];
	char	pal[768];
	
// the dither starts out all black	
	dgray = 0;
	for (b=0 ; b<4 ; b++)
		dither[dgray][b] = dith[b] = 0;
	dgray++;
	
	for (g=1 ; g<= 3 ; g++)
	{
		for (o=0 ; o<=15 ; o++)
		{
		// set one more pixel to the new gray level
			x = order[o][0]*2;
			y = order[o][1];
			dith[y] &= ~(3<<x);
			dith[y] |= (g<<x);
		// copy out the current state of the dither
			for (b=0 ; b<4 ; b++)
				dither[dgray][b] = dith[b];
			dgray++;
		}
	}
	
// set a defualt palette
	if (!ditherpalette[0])
	{
		for (b=0 ; b<768 ; b++)
			pal[b] = b/3;
		SetDitherPalette (pal);
	}
}


/*
===============
=
= SetDitherPalette
=
= sets ditherpalette based on the passed 8/8/8 rgb palette
===============
*/

void SetDitherPalette (unsigned char *palette)
{
    float 	red, green, blue;
	float	gray;
	int		igray;
	int		i;

	for (i=0 ; i<256 ; i++)
	{
		red = *palette++ / 256.0;
		green = *palette++ / 256.0;
		blue = *palette++ / 256.0;
		gray = red*0.299 + green*0.587 + blue*0.114;
		igray = gray*NUMGRAYS;	// can never equal NUMGRAYS
		ditherpalette[i] = dither[igray];
	}
}

/*
===============
=
= DitherUpdate?
=
= Fills the dest buffer with a dithered representation of the VGA source
===============
*/

void DitherUpdate4 (unsigned char *source, unsigned char *dest, int width, int height)
{
	int		y;
	unsigned char	*ditherpal, *stop;
	int		width2, width3;
	
	width2 = width*2;
	width3 = width*3;

	for (y=0 ; y<height ; y++)
	{
		stop = dest+width;
		do
		{
			ditherpal = ditherpalette[*source++];
			dest[0] = ditherpal[0];
			dest[width] = ditherpal[1];
			dest[width2] = ditherpal[2];
			dest[width3] = ditherpal[3];
		} while (++dest != stop);
		dest += width3;
	}
}


void DitherUpdate2 (unsigned char *source, unsigned char *dest, int width, int height)
{
	int		y,row,skip;
	unsigned char	*ditherpal, *ditherpal2, *stop;
	
	width &= ~1;		// width must be even
	skip = width>>1;
	
// OPTIMIZE: this could be faster by making multiple copies of the dither
// with the apropriate bits already masked off
	for (y=0 ; y<height ; y++)
	{
		row = (y&1)<<1;
		stop = dest+skip;
		do
		{
			ditherpal = ditherpalette[*source++];
			ditherpal2 = ditherpalette[*source++];
			dest[0] = (ditherpal[row]&0xf0) | (ditherpal2[row]&0x0f);
			dest[skip] =  (ditherpal[row+1]&0xf0) | (ditherpal2[row+1]&0x0f);
		} while (++dest != stop);
		dest += skip;
	}
}


void DitherUpdate1 (unsigned char *source, unsigned char *dest, int width, int height)
{
	int		y, row, skip;
	unsigned char	*ditherpal, *ditherpal2, *ditherpal3, *ditherpal4, *stop;
	
	width &= ~4;		// width must be multiple of 4
	skip = width>>2;
	
// OPTIMIZE: this could be faster by making multiple copies of the dither
// with the apropriate bits already masked off
	for (y=0 ; y<height ; y++)
	{
		row = y&3;
		stop = dest+skip;
		do
		{
			ditherpal = ditherpalette[*source++];
			ditherpal2 = ditherpalette[*source++];
			ditherpal3 = ditherpalette[*source++];
			ditherpal4 = ditherpalette[*source++];
			dest[0] = (ditherpal[row]&0xc0) | (ditherpal2[row]&0x30)
				| (ditherpal3[row]&0x0c) | (ditherpal4[row]&0x03);
		} while (++dest != stop);
	}
}


