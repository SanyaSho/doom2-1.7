//
//  Dave's utilities including doubly-linked lists & simple state machines.
//

#include "dutils.h"
#include "doomdef.h"

// extern byte screens[][SCREENWIDTH*SCREENHEIGHT];

//
//                       DOUBLY-LINKED LIST PACKAGE
//

dllist_t *dll_NewList(void)
{
  dllist_t *rc;
  rc = (dllist_t *) Z_Malloc(sizeof(dllist_t), PU_STATIC, 0);
  rc->head = rc->tail = 0;
  return rc;
}

dlnode_t *dll_AddEndNode(dllist_t *list, void *data)
{
  dlnode_t *rc;
  if (!list) I_Error("Bad list in dll_AddEndNode");		// DEBUG
  rc = (dlnode_t *) Z_Malloc(sizeof(dlnode_t), PU_STATIC, 0);
  rc->data = data;
  rc->next = 0;
  rc->prev = list->tail;
  if (list->tail)
  {
    list->tail->next = rc;
    list->tail = rc;
  }
  else list->tail = list->head = rc;
  return rc;
}

dlnode_t *dll_AddStartNode(dllist_t *list, void *data)
{
  dlnode_t *rc;
  if (!list) I_Error("Bad list in dll_AddStartNode");		// DEBUG
  rc = (dlnode_t *) Z_Malloc(sizeof(dlnode_t), PU_STATIC, 0);
  rc->data = data;
  rc->next = list->head;
  rc->prev = 0;
  if (list->head) list->head->prev = rc;
  else list->tail = rc;
  list->head = rc;
  return rc;
}

void *dll_DelNode(dllist_t *list, dlnode_t *node)
{
  void *rc;

  if (!list)					// DEBUG
    I_Error("Bad list in dll_DelNode");		// DEBUG
  if (!list->head)				// DEBUG
    I_Error("Empty list in dll_DelNode");	// DEBUG

  rc = node->data;

  if (node->prev) node->prev->next = node->next;
  if (node->next) node->next->prev = node->prev;

  if (node == list->head) list->head = node->next;
  if (node == list->tail) list->tail = node->prev;

  Z_Free(node);

  return rc;
}

void *dll_DelEndNode(dllist_t *list)
{
  return dll_DelNode(list, list->tail);
}

void *dll_DelStartNode(dllist_t *list)
{
  return dll_DelNode(list, list->head);
}

//
//                       CHEAT SEQUENCE PACKAGE
//

static int firsttime = 1;
static unsigned char cheat_xlate_table[256];

//
// Returns a 1 if the cheat was successful, 0 if failed.
//

int cht_CheckCheat(cheatseq_t *cht, char key)
{
  int i;
  int rc = 0;

  if (firsttime)
  {
    firsttime = 0;
    for (i=0;i<256;i++) cheat_xlate_table[i] = SCRAMBLE(i);
  }

  if (!cht->p) cht->p = cht->sequence; // initialize if first time

  if (*cht->p == 0) *(cht->p++) = key;
  else if (cheat_xlate_table[(unsigned char)key] == *cht->p) cht->p++;
  else cht->p = cht->sequence;

  if (*cht->p == 1) cht->p++;
  else if (*cht->p == 0xff) // end of sequence character
  {
    cht->p = cht->sequence;
    rc = 1;
  }

  return rc;
}

void cht_GetParam(cheatseq_t *cht, char *buffer)
{

  unsigned char *p, c;

  p = cht->sequence;
  while (*(p++) != 1);
  do { c = *p; *(buffer++) = c; *(p++) = 0; } while (c && *p!=0xff );
  if (*p==0xff) *buffer = 0;

}

//
//                       SCREEN WIPE PACKAGE
//

static boolean go = 0; // when zero, stop the wipe
static byte *wipe_scr_start, *wipe_scr_end, *wipe_scr;

void wipe_shittyColMajorXform(short *array, int width, int height)
{
  int x, y;
  short *dest;

  dest = (short *) Z_Malloc(width*height*2, PU_STATIC, 0);

  for(y=0;y<height;y++)
    for(x=0;x<width;x++)
      dest[x*height+y] = array[y*width+x];

  memcpy(array, dest, width*height*2);

  Z_Free(dest);

}

int wipe_initColorXForm(int width, int height, int ticks)
{
  memcpy(wipe_scr, wipe_scr_start, width*height);
  return 0;
}

int wipe_doColorXForm(int width, int height, int ticks)
{
  boolean changed;
  byte *w, *e;
  int newval;

  changed = false;
  w = wipe_scr;
  e = wipe_scr_end;
  while (w!=wipe_scr+width*height)
  {
    if (*w != *e)
    {
      if (*w > *e)
      {
	newval = *w - ticks;
	if (newval < *e) *w = *e;
	else *w = newval;
	changed = true;
      }
      else if (*w < *e)
      {
	newval = *w + ticks;
	if (newval > *e) *w = *e;
	else *w = newval;
	changed = true;
      }
    }
    w++; e++;
  }

  return !changed;

}

int wipe_exitColorXForm(int width, int height, int ticks)
{
  return 0;
}

static int *y;

int wipe_initMelt(int width, int height, int ticks)
{
  int i, r;

  //
  // copy start screen to main screen
  //
  memcpy(wipe_scr, wipe_scr_start, width*height);

  //
  // makes this wipe faster (in theory) to have stuff in column-major format
  //
  wipe_shittyColMajorXform((short*)wipe_scr_start, width/2, height);
  wipe_shittyColMajorXform((short*)wipe_scr_end, width/2, height);

  //
  // setup initial column positions (y<0 => not ready to scroll yet)
  // 
  y = (int *) Z_Malloc(width*sizeof(int), PU_STATIC, 0);
  y[0] = -(M_Random()%16);
  for (i=1;i<width;i++)
  {
    r = (M_Random()%3) - 1;
    y[i] = y[i-1] + r;
    if (y[i] > 0) y[i] = 0;
    else if (y[i] == -16) y[i] = -15;
  }

  return 0;
}

int wipe_doMelt(int width, int height, int ticks)
{
  int i, j, dy, idx;
  short *s, *d;
  boolean done = true;

  width/=2;

  while (ticks--)
  {
    for (i=0;i<width;i++)
    {
      if (y[i]<0) { y[i]++; done = false; }
      else if (y[i] < height)
      {
	dy = (y[i] < 16) ? y[i]+1 : 8;
	if (y[i]+dy >= height) dy = height - y[i];
	s = &((short *)wipe_scr_end)[i*height+y[i]];
	d = &((short *)wipe_scr)[y[i]*width+i];
	idx = 0;
	for (j=dy;j;j--)
	{
	  d[idx] = *(s++);
	  idx += width;
	}
	y[i] += dy;
	s = &((short *)wipe_scr_start)[i*height];
	d = &((short *)wipe_scr)[y[i]*width+i];
	idx = 0;
	for (j=height-y[i];j;j--)
	{
	  d[idx] = *(s++);
	  idx += width;
	}
	done = false;
      }
    }
  }

  return done;

}

int wipe_exitMelt(int width, int height, int ticks)
{
  Z_Free(y);
  return 0;
}

int wipe_StartScreen(int x, int y, int width, int height)
{
  wipe_scr_start = screens[2];
  I_ReadScreen(wipe_scr_start);
  return 0;
}

int wipe_EndScreen(int x, int y, int width, int height)
{
  wipe_scr_end = screens[3];
  I_ReadScreen(wipe_scr_end);
  V_DrawBlock(x, y, 0, width, height, wipe_scr_start); // restore start scr.
  return 0;
}

int wipe_ScreenWipe(int wipeno, int x, int y, int width, int height, int ticks)
{
  int rc;
  static int (*wipes[])(int, int, int) =
  {
    wipe_initColorXForm, wipe_doColorXForm, wipe_exitColorXForm,
    wipe_initMelt, wipe_doMelt, wipe_exitMelt
  };

  void V_MarkRect(int, int, int, int);

  // initial stuff
  if (!go)
  {
    go = 1;
//    wipe_scr = (byte *) Z_Malloc(width*height, PU_STATIC, 0); // DEBUG
    wipe_scr = screens[0];
    (*wipes[wipeno*3])(width, height, ticks);
  }

  // do a piece of wipe-in
  V_MarkRect(0, 0, width, height);
  rc = (*wipes[wipeno*3+1])(width, height, ticks);
//  V_DrawBlock(x, y, 0, width, height, wipe_scr); // DEBUG

  // final stuff
  if (rc)
  {
    go = 0;
    (*wipes[wipeno*3+2])(width, height, ticks);
  }

  return !go;

}
