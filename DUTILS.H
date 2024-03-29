//
//  Dave's utilities including doubly-linked lists & simple state machines.
//

#ifndef __DLLIST_H__
#define __DLLIST_H__

#define RANGECHECKING

//
//			 RANGE-CHECKING PACAKGE
//

#ifdef RANGECHECKING

// Does an inclusive range check of var.
#define RNGCHECK(var, a, b) if ((var)<(a) || (var)>(b)) \
  I_Error("%s=%d in %s:%d", #var, var, __FILE__, __LINE__)

// Does a null-check and negative-check on pointer.
#define PTRCHECK(ptr) if (!(ptr) || ((int)(ptr) < 0)) \
  I_Error("%s=0x%lx in %s:%d", #ptr, (unsigned int)(ptr), __FILE__, __LINE__)

#endif

//
//                       DOUBLY-LINKED LIST PACKAGE
//

typedef struct dlnode_struct dlnode_t;

struct dlnode_struct
{
  void *data;
  dlnode_t *prev, *next;
};

typedef struct
{
  dlnode_t *head;
  dlnode_t *tail;
} dllist_t;

dllist_t *dll_NewList(void);
dlnode_t *dll_AddEndNode(dllist_t *list, void *data);
dlnode_t *dll_AddStartNode(dllist_t *list, void *data);
void *dll_DelNode(dllist_t *list, dlnode_t *node);
void *dll_DelEndNode(dllist_t *list);
void *dll_DelStartNode(dllist_t *list);

//
//                       CHEAT SEQUENCE PACKAGE
//

#define SCRAMBLE(a) \
  ((((a)&1)<<7) + (((a)&2)<<5) + ((a)&4) + (((a)&8)<<1) \
  + (((a)&16)>>1) + ((a)&32) + (((a)&64)>>5) + (((a)&128)>>7))

typedef struct
{
  unsigned char *sequence;
  unsigned char *p;
} cheatseq_t;

int cht_CheckCheat(cheatseq_t *cht, char key);
void cht_GetParam(cheatseq_t *cht, char *buffer);

//
//                       SCREEN WIPE PACKAGE
//

enum
{
  wipe_ColorXForm,	// simple gradual pixel change for 8-bit only
  wipe_Melt,		// weird screen melt
  wipe_NUMWIPES
};

int wipe_StartScreen(int x, int y, int width, int height);
int wipe_EndScreen(int x, int y, int width, int height);
int wipe_ScreenWipe(int wipeno, int x, int y, int width, int height, int ticks);

#endif
