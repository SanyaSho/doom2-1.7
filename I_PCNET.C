// I_pcnet.m 
 
#include <dos.h> 
#include "DoomDef.h" 
 
 
/* 
============================================================================= 
 
						IPX PACKET DRIVER 
 
============================================================================= 
*/ 
 
 
#define DPMI_INT        0x31 
 
 
typedef unsigned char BYTE; 
typedef unsigned short WORD; 
typedef unsigned long LONG; 
 
typedef struct IPXPacketStructure 
{ 
	WORD    PacketCheckSum;                 /* high-low */ 
	WORD    PacketLength;                   /* high-low */ 
	BYTE    PacketTransportControl; 
	BYTE    PacketType; 
 
	BYTE    dNetwork[4];            /* high-low */ 
	BYTE    dNode[6];                       /* high-low */ 
	BYTE    dSocket[2];                     /* high-low */ 
 
	BYTE    sNetwork[4];            /* high-low */ 
	BYTE    sNode[6];                       /* high-low */ 
	BYTE    sSocket[2];                     /* high-low */ 
} IPXPacket; 
 
 
typedef struct 
{ 
	BYTE    network[4];                     /* high-low */ 
	BYTE    node[6];                        /* high-low */ 
} localadr_t; 
 
typedef struct ECBStructure 
{ 
	WORD    Link[2];                        /* offset-segment */ 
	WORD    ESRAddress[2];          /* offset-segment */ 
	BYTE    InUseFlag; 
	BYTE    CompletionCode; 
	WORD    ECBSocket;                      /* high-low */ 
	BYTE    IPXWorkspace[4];        /* N/A */ 
	BYTE    DriverWorkspace[12];    /* N/A */ 
	BYTE    ImmediateAddress[6];    /* high-low */ 
	WORD    FragmentCount;          /* low-high */ 
//      struct ECBFragment      FragmentDescriptor[1]; 
 
	WORD    fAddress[2];            /* offset-segment */ 
	WORD    fSize;                          /* low-high */ 
} ECB; 
 
typedef struct 
{ 
	unsigned        edi, esi, ebp, reserved, ebx, edx, ecx, eax; 
	unsigned short  flags, es, ds, fs, gs, ip, cs, sp, ss; 
} dpmiregs_t; 
 
extern  dpmiregs_t      dpmiregs; 
 
int     socketid = 0x869c;              // broadcast socket number 
 
typedef struct 
{ 
	ECB                     ecb; 
	IPXPacket       ipx; 
	netstruc_t      data; 
	int                     checksum; 
} packet_t; 
 
#define NUMPACKETS      10 
 
packet_t        *packets;       // [NUMPACKETS] 
 
extern  union   REGS    regs;           // scratch for int calls 
extern  struct  SREGS   segregs; 
 
unsigned        enteripx[2]; 
 
#define REAL_OFF(x)     ((unsigned)(x)&15) 
#define REAL_SEG(x)     ((unsigned)(x)>>4) 
 
int             localnetid, remotenetid; 
 
localadr_t      *localipxadr; 
 
void DPMIInt (int i); 
 
 
 
//=========================================================================== 
 
 
 
int OpenSocket(short socketNumber) 
{ 
	regs.w.bx = 0; 
	regs.h.al = 0;                  // longevity 
	regs.w.dx = socketNumber; 
	int386(0x7A,&regs,&regs); 
	if (regs.h.al) 
		I_Error ("OpenSocket: 0x%x",regs.h.al); 
	return regs.w.dx; 
} 
 
 
void ListenForPacket(ECB *ecb) 
{ 
	memset (&dpmiregs,0,sizeof(dpmiregs)); 
	dpmiregs.esi = REAL_OFF(ecb); 
	dpmiregs.es = REAL_SEG(ecb); 
	dpmiregs.ebx = 4; 
	dpmiregs.ip = enteripx[0]; 
	dpmiregs.cs = enteripx[1]; 
 
//printf ("listen\n"); 
	DPMIInt (0x7a); 
//printf ("done\n"); 
	if (dpmiregs.eax & 0xff) 
		I_Error ("ListenForPacket: 0x%x",dpmiregs.eax&0xff); 
} 
 
 
void GetLocalAddress (void) 
{ 
 
// 
// allocate dos memory for info 
// 
	localipxadr = (localadr_t *)I_AllocLow (sizeof(localadr_t)); 
 
 
// 
// get the address from ipx 
// 
	memset (&dpmiregs,0,sizeof(dpmiregs)); 
	dpmiregs.esi = REAL_OFF(localipxadr); 
	dpmiregs.es = REAL_SEG(localipxadr); 
	dpmiregs.ebx = 9; 
	dpmiregs.ip = enteripx[0]; 
	dpmiregs.cs = enteripx[1]; 
 
	DPMIInt (0x7a); 
	if (dpmiregs.eax & 0xff) 
		I_Error ("Get inet addr: 0x%x",dpmiregs.eax&0xff); 
 
	localnetid = *(int *)localipxadr->node; 
	localnetid ^= *(int *)&localipxadr->node[2]; 
 
	printf ("localnetid: 0x%x\n",localnetid); 
} 
 
/* 
==================== 
= 
= I_ShutdownNet 
= 
==================== 
*/ 
 
void I_ShutdownNet (void) 
{ 
 
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
	int     i,j; 
	int     socketnum; 
 
// 
// get IPX function address 
// 
	memset(&dpmiregs,0,sizeof(dpmiregs)); 
	dpmiregs.eax = 0x7a00; 
	DPMIInt (0x2f); 
	if ( (dpmiregs.eax&0xff) != 0xff) 
		I_Error ("IPX not detected\n"); 
 
	enteripx[0] = (dpmiregs.edi)&0xffff; 
	enteripx[1] = dpmiregs.es; 
 
//      printf ("ipxcall: 0x%x:0x%x\n",enteripx[1],enteripx[0]); 
 
 
 
// 
// allocate dos memory for packets 
// 
	packets = (packet_t *)I_AllocLow (NUMPACKETS*sizeof(packet_t)); 
 
// 
// allocate a socket for sending and receiving 
// 
	i = M_CheckParm ("-port"); 
	if (i>0 && i<myargc-1) 
	{ 
		socketid = atoi (myargv[i+1]); 
		printf ("using alternate port %i for network\n",socketid); 
	} 
	socketnum = OpenSocket ( (socketid>>8) + ((socketid&255)<<8) ); 
//      printf ("socketnum: 0x%x\n",socketnum); 
 
	GetLocalAddress (); 
 
// 
// set up a receiving ECB 
// 
	memset (packets,0,NUMPACKETS*sizeof(packet_t)); 
 
	for (i=1 ; i<NUMPACKETS ; i++) 
	{ 
		packets[i].ecb.InUseFlag = 0x1d; 
		packets[i].ecb.ECBSocket = socketnum; 
		packets[i].ecb.FragmentCount = 1; 
		packets[i].ecb.fAddress[0] = REAL_OFF(&packets[i].ipx); 
		packets[i].ecb.fAddress[1] = REAL_SEG(&packets[i].ipx); 
		packets[i].ecb.fSize = sizeof(packet_t)-sizeof(ECB); 
 
		ListenForPacket (&packets[i].ecb); 
	} 
 
// 
// set up a sending ECB 
// 
	memset (&packets[0],0,sizeof(packets[0])); 
 
	packets[0].ecb.ECBSocket = socketnum; 
	packets[0].ecb.FragmentCount = 1; 
	packets[0].ecb.fAddress[0] = REAL_OFF(&packets[0].ipx); 
	packets[0].ecb.fAddress[1] = REAL_SEG(&packets[0].ipx); 
	packets[0].ecb.fSize = sizeof(packet_t)-sizeof(ECB); 
	for (j=0 ; j<6 ; j++) 
	{ 
		packets[0].ipx.dNode[j] = 0xff; // broadcast node 
		packets[0].ecb.ImmediateAddress[j] = 0xff; 
	} 
	packets[0].ipx.dSocket[0] = socketnum&255; 
	packets[0].ipx.dSocket[1] = socketnum>>8; 
 
// 
// sync 
// 
//SendPacket (); 
 
//      NetSync (); 
//getchar(); 
} 
 
 
/* 
============== 
= 
= SendPacket 
= 
============== 
*/ 
 
void SendPacket (void) 
{ 
	int             j, checksum; 
 
//printf ("send\n"); 
	while (packets[0].ecb.InUseFlag) 
	{ 
//printf ("Send: inuse: 0x%x\n",packets[0].ecb.InUseFlag); 
	} 
//printf ("not in use\n"); 
 
// put the data into an ipx packet 
	memcpy (&packets[0].data, &netbuffer, sizeof(netbuffer)); 
	checksum = 0; 
	for (j=0 ; j<sizeof(netstruc_t) ; j++) 
		checksum += *(((byte *)&packets[0].data) + j); 
	packets[0].checksum = checksum; 
 
	memset (&dpmiregs,0,sizeof(dpmiregs)); 
	dpmiregs.esi = REAL_OFF(&packets[0]); 
	dpmiregs.es = REAL_SEG(&packets[0]); 
	dpmiregs.ebx = 3; 
	dpmiregs.ip = enteripx[0]; 
	dpmiregs.cs = enteripx[1]; 
 
	DPMIInt (0x7a); 
//printf ("done\n"); 
	if (dpmiregs.eax & 0xff) 
		I_Error ("SendPacket: 0x%x",dpmiregs.eax&0xff); 
} 
 
 
/* 
============== 
= 
= GetPacket 
= 
= Returns false if no packet is waiting 
= 
============== 
*/ 
 
boolean GetPacket (void) 
{ 
	int             packetnum ; 
	int             i, j,checksum, besttic; 
 
	besttic = MAXINT; 
	packetnum = -1; 
 
	for ( i = 1 ; i < NUMPACKETS ; i++) 
	{ 
		if (packets[i].ecb.InUseFlag) 
			continue; 
 
		if (packets[i].data.starttic < besttic) 
		{ 
			besttic = packets[i].data.starttic; 
			packetnum = i; 
		} 
	} 
 
	if (besttic == MAXINT) 
		return false;                           // no packets 
 
	if (packetnum == -1) 
		I_Error ("GetPacket: packetnum == -1"); 
 
// 
// got a good packet 
// 
	if (packets[packetnum].ecb.CompletionCode) 
		I_Error ("GetPacket: ecb.ComletionCode = 0x%x",packets[packetnum].ecb.CompletionCode); 
 
	remotenetid = *(int *)packets[packetnum].ipx.sNode; 
	remotenetid ^= *(int *)&packets[packetnum].ipx.sNode[2]; 
 
// copy out the data 
	checksum = 0; 
	for (j=0 ; j<sizeof(netstruc_t) ; j++) 
		checksum += *(((byte *)&packets[packetnum].data) + j); 
	if (checksum != packets[packetnum].checksum) 
		I_Error ("GetPacket: bad checksum"); 
	memcpy (&netbuffer, &packets[packetnum].data, sizeof(netbuffer)); 
 
// repost the ECB 
 
	ListenForPacket (&packets[packetnum].ecb); 
	return true; 
} 
 
 
void SerialRead (void) 
{ 
} 
 
void SerialWrite (void) 
{ 
} 
 
void SerialInit (void) 
{ 
} 
 
void SerialShutdown (void) 
{ 
} 
 
