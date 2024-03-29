#ifndef __WADREAD_H__
#define __WADREAD_H__

//
//  Opens the wadfile specified.  Must be called before any calls to
//  loadlump() or getsfx().
//

void openwad(char *wadname);

//
//  Gets a sound effect from the wad file.  The pointer points to the
//  start of the data.  Returns a 0 if the sfx was not
//  found.  Sfx names should be no longer than 6 characters.  All data is
//  rounded up in size to the nearest MIXBUFFERSIZE and is padded out with
//  0x80's.  Returns the data length in len.
//

void *getsfx(char *sfxname, int *len);
#endif
