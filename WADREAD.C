#include <malloc.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "sndserver.h"
#include "wadread.h"

int *sfxlengths;

typedef struct wadinfo_struct
{
  char	identification[4];		                 
  int	numlumps;
  int	infotableofs;
} wadinfo_t;

typedef struct filelump_struct
{
  int	filepos;
  int	size;
  char	name[8];
} filelump_t;

typedef struct lumpinfo_struct
{
  int	handle;
  int	filepos;
  int	size;
  char	name[8];
} lumpinfo_t;

lumpinfo_t	*lumpinfo;		                                
int		numlumps;

void		**lumpcache;

#define strcmpi strcasecmp

#ifndef __BIG_ENDIAN__

#define LONG(x) (x)
#define SHORT(x) (x)

#else

#define LONG(x) ((long)SwapLONG((unsigned long) (x)))
#define SHORT(x) ((short)SwapSHORT((unsigned short) (x)))

unsigned long SwapLONG(unsigned long x)
{
  return
    (x>>24)
    | ((x>>8) & 0xff00)
    | ((x<<8) & 0xff0000)
    | (x<<24);
}

unsigned short SwapSHORT(unsigned short x)
{
  return
    (x>>8) | (x<<8);
}

#endif

static void derror(char *msg)
{
  fprintf(stderr, "\nwadread error: %s\n", msg);
  exit(-1);
}

void strupr (char *s)
{
  while (*s)
    *s++ = toupper(*s);
}

int filelength (int handle)
{
  struct stat	fileinfo;
  
  if (fstat (handle,&fileinfo) == -1)
    fprintf (stderr, "Error fstating\n");

  return fileinfo.st_size;
}

void openwad(char *wadname)
{

  int wadfile, tableoffset, tablelength, tablefilelength;
  int i;
  wadinfo_t header;
  filelump_t *filetable;

  // open and read the wadfile header
  wadfile = open(wadname, O_RDONLY);
  if (wadfile < 0) derror("Could not open wadfile");
  read(wadfile, &header, sizeof header);
  if (strncmp(header.identification, "IWAD", 4))
    derror("wadfile has weirdo header");
  numlumps = LONG(header.numlumps);
  tableoffset = LONG(header.infotableofs);
  tablelength = numlumps * sizeof(lumpinfo_t);
  tablefilelength = numlumps * sizeof(filelump_t);
  lumpinfo = (lumpinfo_t *) malloc(tablelength);
  filetable = (filelump_t *) ((char*)lumpinfo + tablelength - tablefilelength);

  // get the lumpinfo table
  lseek(wadfile, tableoffset, SEEK_SET);
  read(wadfile, filetable, tablefilelength);

  // process the table to make the endianness right and shift it down
  for (i=0 ; i<numlumps ; i++)
  {
    strncpy(lumpinfo[i].name, filetable[i].name, 8);
    lumpinfo[i].handle = wadfile;
    lumpinfo[i].filepos = LONG(filetable[i].filepos);
    lumpinfo[i].size = LONG(filetable[i].size);
//    fprintf(stderr, "lump [%.8s] exists\n", lumpinfo[i].name);
  }

}

void *loadlump(char *lumpname, int *size)
{

  int i;
  void *lump;

  for (i=0 ; i<numlumps ; i++)
  {
    if (!strncasecmp(lumpinfo[i].name, lumpname, 8))
      break;
  }

  if (i == numlumps)
  {
    fprintf(stderr, "Could not find lumpname [%s]\n", lumpname);
    lump = 0;
  } else {
    lump = (void *) malloc(lumpinfo[i].size);
    lseek(lumpinfo[i].handle, lumpinfo[i].filepos, SEEK_SET);
    read(lumpinfo[i].handle, lump, lumpinfo[i].size);
    *size = lumpinfo[i].size;
  }

  return lump;

}

void *getsfx(char *sfxname, int *len)
{

  unsigned char *sfx;
  unsigned char *paddedsfx;
  int i, size, paddedsize;
  char name[20];

  sprintf(name, "ds%s", sfxname);

  sfx = (unsigned char *) loadlump(name, &size);

  // pad the sound effect out to the mixing buffer size
  paddedsize = ((size-8 + (SAMPLECOUNT-1)) / SAMPLECOUNT) * SAMPLECOUNT;
  paddedsfx = (unsigned char *) realloc(sfx, paddedsize+8);
  for (i=size ; i<paddedsize+8 ; i++)
    paddedsfx[i] = 128;

  *len = paddedsize;
  return (void *) (paddedsfx + 8);

}
