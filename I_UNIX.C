// I_unix.c, Doom for UNIX

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include "doomdef.h"

int isCyberPresent;

void NetSend (void);
boolean NetListen (void);

typedef union { short a; unsigned char b[sizeof(short)]; } s_endian;
typedef union { long a; unsigned char b[sizeof(long)]; } l_endian;

static s_endian endianness = { 1 };

int	mb_used;

FILE *sndserver=0;
char *sndserver_filename;

void I_ReadCyberCmd(ticcmd_t *cmd)
{
}
void I_Tactile(int on, int off, int total)
{
}
ticcmd_t emptycmd;
ticcmd_t *I_BaseTiccmd(void)
{
	return &emptycmd;
}

short ShortSwap (short dat)
{

//  if (endianness.b[0])
//    return dat; // do not flip little-endian
//  else
    return ((dat&0xff)<<8) | ((dat>>8)&0xff);

}

long LongSwap (long dat)
{

//  if (endianness.b[0])
//    return dat; // do not flip little-endian
//  else // otherwise flip it
//    return	((l_endian) dat).b[0]
//    		+ (((l_endian) dat).b[1] << 8)
//    		+ (((l_endian) dat).b[2] << 16)
//    		+ (((l_endian) dat).b[3] << 24);
    return	(((unsigned)dat)>>24) |
    		((dat>>8)&0xff00) |
    		((dat<<8)&0xff0000) |
    		(dat<<24);

}

#if !defined(__WATCOMC__) || !defined(__i386) || defined(NORMALUNIX)

fixed_t  FixedMul (fixed_t a, fixed_t b)
{
  return ((long long) a * (long long) b) >> FRACBITS;
}

fixed_t	FixedDiv2 (fixed_t a, fixed_t b)
{
	long	long	c;
    
    c = (fixed_t)( ((double)a/FRACUNIT) / ((double)b/FRACUNIT) *FRACUNIT);
//	c = ((long long)a<<FRACBITS) / b;
	if (c >= 0x80000000)
		I_Error ("FixedDiv: divide by zero");
	return (fixed_t)c;

}

#endif

int  I_GetHeapSize (void)
{
  return mb_used*1024*1024;
}

byte *I_ZoneBase (int *size)
{
	*size = mb_used*1024*1024;
	return (byte *) malloc (*size);
}

/*
=================
=
= I_GetTime
=
= returns time in 1/70th second tics
=================
*/

int  I_GetTime (void)
{
  struct timeval tp;
  struct timezone tzp;
  int		newtics;
  static int basetime=0;
  
  gettimeofday(&tp, &tzp);
  if (!basetime)
	basetime = tp.tv_sec;
  newtics = (tp.tv_sec-basetime)*TICRATE + tp.tv_usec*TICRATE/1000000;
  return newtics;
}

/*
 *		MUSIC & SFX API
 */

void I_SetMusicVolume(int volume) { }
void I_SetSfxVolume(int volume) { }

// GENERAL SFX API

void I_SetChannels(int channels) { }
int I_GetSfxLumpNum(sfxinfo_t *sfx)
{
  char namebuf[9];
  sprintf(namebuf, "ds%s", sfx->name);
  return W_GetNumForName(namebuf);
}
int I_StartSound (int id, void *data, int vol, int sep, int pitch, int priority)
{
	if (sndserver)
	{
		fprintf(sndserver, "p%2.2x%2.2x%2.2x%2.2x\n", id, pitch, vol, sep);
		fflush(sndserver);
	}
}
void I_StopSound (int handle) { }
int I_SoundIsPlaying(int handle) { return gametic < handle; }
void I_UpdateSoundParams(int handle, int vol, int sep, int pitch) { }

// MUSIC API

static int looping=0, musicdies=-1;

void I_StopSong(int handle) { looping = 0; musicdies = 0; }
void I_PauseSong (int handle) { }
void I_ResumeSong (int handle) { }
void I_PlaySong(int handle, int looping)
{
  musicdies = gametic + TICRATE*30;
}
void I_UnRegisterSong(int handle) {}
int I_RegisterSong(void *data) { return 1; }
int I_QrySongPlaying(int handle)
{
  return looping || musicdies > gametic;
}

void I_ShutdownSound(void)
{
	if (sndserver)
	{
		fprintf(sndserver, "q\n");
		fflush(sndserver);
	}
}

/*
====================
=
= I_Init
=
====================
*/

void I_Init (void)
{

	char buffer[256];

	sprintf(buffer, "%s -quiet", sndserver_filename);
//	sprintf(buffer, "%s", sndserver_filename);
  // start sound process
  if ( ! access(sndserver_filename, X_OK) )
	  sndserver = popen(buffer, "w");
  else
    fprintf(stderr, "Could not start sound server [%s]\n", sndserver_filename);

  I_InitGraphics();

}

/*
================
=
= I_Quit
=
================
*/

void I_ShutdownGraphics(void);

void I_Quit (void)
{
  D_QuitNetGame ();
  I_ShutdownGraphics();
  I_ShutdownSound();
  M_SaveDefaults ();
  exit(0);
}

void I_WaitVBL(int count)
{
#ifdef SGI
  sginap(1); // how come no usleep()?  hell if i know..
#else
  usleep (count * (1000000/70) ); // why the fuck should I pause?
#endif
}

void I_BeginRead(void)
{
}

void I_EndRead(void)
{
}

byte *I_AllocLow(int length)
{
  byte    *mem;
        
  mem = (byte *)malloc (length);
  memset (mem,0,length);
  return mem;
}

/*
==============================================================================

							NETWORKING

==============================================================================
*/
#include <netdb.h>
#include <sys/ioctl.h>

int	DOOMPORT =	(IPPORT_USERRESERVED +0x1d );


int			sendsocket, insocket;

struct	sockaddr_in	sendaddress[MAXNETNODES];

void	(*netget) (void);
void	(*netsend) (void);


/*
===============
=
= UDPsocket
=
===============
*/

int UDPsocket (void)
{
	int	s;
	
// allocate a socket
	s = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s<0)
		I_Error ("can't create socket: %s",strerror(errno));
		
	return s;
}

/*
===============
=
= BindToLocalPort
=
===============
*/

void BindToLocalPort (int s, int port)
{
	int					v;
	struct	sockaddr_in	address;
	
	memset (&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = port;
			
	v = bind (s, (void *)&address, sizeof(address));
	if (v == -1)
		I_Error ("BindToPort: bind: %s", strerror(errno));
}


/*
==============
=
= PacketSend
=
==============
*/

void PacketSend (void)
{
	int			c;
	doomdata_t	sw;
				
// byte swap
	sw.checksum = htonl(netbuffer->checksum);
	sw.player = netbuffer->player;
	sw.retransmitfrom = netbuffer->retransmitfrom;
	sw.starttic = netbuffer->starttic;
	sw.numtics = netbuffer->numtics;
	for (c=0 ; c< netbuffer->numtics ; c++)
	{
		sw.cmds[c].forwardmove = netbuffer->cmds[c].forwardmove;
		sw.cmds[c].sidemove = netbuffer->cmds[c].sidemove;
		sw.cmds[c].angleturn = htons(netbuffer->cmds[c].angleturn);
		sw.cmds[c].consistancy = htons(netbuffer->cmds[c].consistancy);
		sw.cmds[c].chatchar = netbuffer->cmds[c].chatchar;
		sw.cmds[c].buttons = netbuffer->cmds[c].buttons;
	}
		
//printf ("sending %i\n",gametic);		
	c = sendto (sendsocket , &sw, doomcom->datalength
	,0,(void *)&sendaddress[doomcom->remotenode]
	,sizeof(sendaddress[doomcom->remotenode]));
	
	if (c == -1)
		I_Error ("SendPacket error: %s",strerror(errno));
}

/*
==============
=
= PacketGet
=
==============
*/

void PacketGet (void)
{
	int			i,c;
	struct		sockaddr_in	fromaddress;
	int			fromlen;
	doomdata_t	sw;
				
	fromlen = sizeof(fromaddress);
	c = recvfrom (insocket, &sw, sizeof(sw), 0
		, (struct sockaddr *)&fromaddress, &fromlen );
	if (c == -1 )
	{
		if (errno != EWOULDBLOCK)
			I_Error ("GetPacket: %s",strerror(errno));
		doomcom->remotenode = -1;		// no packet
		return;
	}

// find remote node number
	for (i=0 ; i<doomcom->numnodes ; i++)
		if ( fromaddress.sin_addr.s_addr == sendaddress[i].sin_addr.s_addr )
			break;

	if (i == doomcom->numnodes)
	{	// packet isn't from one of the players (new game broadcast)
		doomcom->remotenode = -1;		// no packet
		return;
	}
	
	doomcom->remotenode = i;			// good packet from a game player
	doomcom->datalength = c;
	
// byte swap
	netbuffer->checksum = ntohl(sw.checksum);
	netbuffer->player = sw.player;
	netbuffer->retransmitfrom = sw.retransmitfrom;
	netbuffer->starttic = sw.starttic;
	netbuffer->numtics = sw.numtics;
	for (c=0 ; c< netbuffer->numtics ; c++)
	{
		netbuffer->cmds[c].forwardmove = sw.cmds[c].forwardmove;
		netbuffer->cmds[c].sidemove = sw.cmds[c].sidemove;
		netbuffer->cmds[c].angleturn = ntohs(sw.cmds[c].angleturn);
		netbuffer->cmds[c].consistancy = ntohs(sw.cmds[c].consistancy);
		netbuffer->cmds[c].chatchar = sw.cmds[c].chatchar;
		netbuffer->cmds[c].buttons = sw.cmds[c].buttons;
	}

}



int GetLocalAddress (void)
{
	char				hostname[1024];
	struct hostent		*hostentry;	// host information entry
	int			v;

// get local address
	v = gethostname (hostname, sizeof(hostname));
	if (v == -1)
		I_Error ("GetLocalAddress : gethostname: ",sys_errlist[errno]);
	
	hostentry = gethostbyname (hostname);
	if (!hostentry)
		I_Error ("GetLocalAddress : gethostbyname: couldn't get local host");
		
	return *(int *)hostentry->h_addr_list[0];
}


/*
====================
=
= I_InitNetwork
=
====================
*/

void I_InitNetwork (void)
{
	boolean		trueval = true;
	int			i,p;
	struct hostent		*hostentry;	// host information entry
	
	doomcom = malloc (sizeof (*doomcom) );
	memset (doomcom, 0, sizeof(*doomcom) );
		
//
// set up for network
//
	i = M_CheckParm ("-dup");
	if (i && i< myargc-1)
	{
		doomcom->ticdup = myargv[i+1][0]-'0';
		if (doomcom->ticdup < 1)
			doomcom->ticdup = 1;
		if (doomcom->ticdup > 9)
			doomcom->ticdup = 9;
	}
	else
		doomcom-> ticdup = 1;
	
	if (M_CheckParm ("-extratic"))
		doomcom-> extratics = 1;
	else
		doomcom-> extratics = 0;
		
	p = M_CheckParm ("-port");
	if (p && p<myargc-1)
	{
		DOOMPORT = atoi (myargv[p+1]);
		printf ("using alternate port %i\n",DOOMPORT);
	}

	
//
// parse network game options -net <consoleplayer> <host> <host> ...
//
	i = M_CheckParm ("-net");
	if (!i)
	{
	//
	// single player game
	//
		netgame = false;
		doomcom->id = DOOMCOM_ID;
		doomcom->numplayers = doomcom->numnodes = 1;
		doomcom->deathmatch = false;
		doomcom->consoleplayer = 0;
		return;
	}

	netsend = PacketSend;
	netget = PacketGet;
	netgame = true;
	
//
// parse player number and host list
//
	doomcom->consoleplayer = myargv[i+1][0]-'1';

	doomcom->numnodes = 1;		// this node for sure
	
	i++;
	while (++i < myargc && myargv[i][0] != '-')
	{
		sendaddress[doomcom->numnodes].sin_family = AF_INET;
		sendaddress[doomcom->numnodes].sin_port = htons(DOOMPORT);
		if (myargv[i][0] == '.')
		{
			sendaddress[doomcom->numnodes].sin_addr.s_addr
				= inet_addr (myargv[i]+1);
		}
		else
		{
			hostentry = gethostbyname (myargv[i]);
			if (!hostentry)
				I_Error ("gethostbyname: couldn't find %s", myargv[i]);
			sendaddress[doomcom->numnodes].sin_addr.s_addr
				= *(int *)hostentry->h_addr_list[0];
		}
		doomcom->numnodes++;
	}
	
	doomcom->id = DOOMCOM_ID;
	doomcom->numplayers = doomcom->numnodes;
	
//	
// build message to receive	
//
	insocket = UDPsocket ();
	BindToLocalPort (insocket,htons(DOOMPORT));
	ioctl (insocket, FIONBIO, &trueval);

	sendsocket = UDPsocket ();
}


void I_NetCmd (void)
{
	if (doomcom->command == CMD_SEND)
	{
		netsend ();
	}
	else if (doomcom->command == CMD_GET)
	{
		netget ();
	}
	else
		I_Error ("Bad net cmd: %i\n",doomcom->command);
}

