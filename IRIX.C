#include "sndserver.h"
#include <stdlib.h>
#include <stdio.h>
#include <audio.h>
#include "irix.h"

#define al_QUEUESIZE	SAMPLECOUNT*2
//#define al_QUEUESIZE	SAMPLECOUNT

extern int snd_verbose;

extern int myargc;
extern char **myargv;

static	ALconfig		al_config;
static	ALport			al_outport;

void I_InitMusic(void)
{
}

void I_InitSound(int samplerate, int samplesize)
{

	long paramset[16*2];

	if (samplesize == 24)
		samplesize = 32;	// takes care of padding for 24-bit sound

	ALseterrorhandler(0);		// turn off audio library error handler
	al_config = ALnewconfig();	// create new config for specific rate & format

//	ALsetchannels(al_config, AL_MONO);			// mono
	ALsetchannels(al_config, AL_STEREO);			// stereo
	ALsetqueuesize(al_config, al_QUEUESIZE);		// submission queue size
	ALsetsampfmt(al_config, AL_SAMPFMT_TWOSCOMP);	// 2's complement fmt
	ALsetwidth(al_config, samplesize/8);			// word size

	al_outport = ALopenport("iddigout1", "w", al_config);

	if (!al_outport)
	{
		fprintf(stderr, "Could not open audio port\n");
		exit(-1);
	}

	paramset[0] = AL_OUTPUT_RATE;
	paramset[1] = samplerate;
	ALsetparams(AL_DEFAULT_DEVICE, paramset, 2);

	paramset[0*2] = AL_INPUT_SOURCE;
	paramset[1*2] = AL_LEFT_INPUT_ATTEN;
	paramset[2*2] = AL_RIGHT_INPUT_ATTEN;
	paramset[3*2] = AL_INPUT_RATE;
	paramset[4*2] = AL_OUTPUT_RATE;
	paramset[5*2] = AL_LEFT_SPEAKER_GAIN;
	paramset[6*2] = AL_RIGHT_SPEAKER_GAIN;
	paramset[7*2] = AL_INPUT_COUNT;
	paramset[8*2] = AL_OUTPUT_COUNT;
	paramset[9*2] = AL_UNUSED_COUNT;
	paramset[10*2] = AL_SYNC_INPUT_TO_AES;
	paramset[11*2] = AL_SYNC_OUTPUT_TO_AES;
	paramset[12*2] = AL_MONITOR_CTL;
	paramset[13*2] = AL_LEFT_MONITOR_ATTEN;
	paramset[14*2] = AL_RIGHT_MONITOR_ATTEN;

	ALgetparams(AL_DEFAULT_DEVICE, paramset, 15*2);

	if (snd_verbose)
	{
		fprintf(stderr, "AL_INPUT_SOURCE=%d\n", paramset[0*2+1]);
		fprintf(stderr, "AL_LEFT_INPUT_ATTEN=%d\n", paramset[1*2+1]);
		fprintf(stderr, "AL_RIGHT_INPUT_ATTEN=%d\n", paramset[2*2+1]);
		fprintf(stderr, "AL_INPUT_RATE=%d\n", paramset[3*2+1]);
		fprintf(stderr, "AL_OUTPUT_RATE=%d\n", paramset[4*2+1]);
		fprintf(stderr, "AL_LEFT_SPEAKER_GAIN=%d\n", paramset[5*2+1]);
		fprintf(stderr, "AL_RIGHT_SPEAKER_GAIN=%d\n", paramset[6*2+1]);
		fprintf(stderr, "AL_INPUT_COUNT=%d\n", paramset[7*2+1]);
		fprintf(stderr, "AL_OUTPUT_COUNT=%d\n", paramset[8*2+1]);
		fprintf(stderr, "AL_UNUSED_COUNT=%d\n", paramset[9*2+1]);
		fprintf(stderr, "AL_SYNC_INPUT_TO_AES=%d\n", paramset[10*2+1]);
		fprintf(stderr, "AL_SYNC_OUTPUT_TO_AES=%d\n", paramset[11*2+1]);
		fprintf(stderr, "AL_MONITOR_CTL=%d\n", paramset[12*2+1]);
		fprintf(stderr, "AL_LEFT_MONITOR_ATTEN=%d\n", paramset[13*2+1]);
		fprintf(stderr, "AL_RIGHT_MONITOR_ATTEN=%d\n", paramset[14*2+1]);

		fprintf(stderr, "ALgetchannels()=%d\n", ALgetchannels(al_config));
		fprintf(stderr, "ALgetqueuesize()=%d\n", ALgetqueuesize(al_config));
		fprintf(stderr, "ALgetsampfmt()=%d\n", ALgetsampfmt(al_config));
		fprintf(stderr, "ALgetwidth()=%d\n", ALgetwidth(al_config));
	}

}

signed char debugbuffer[1024][8];

void I_SubmitOutputBuffer(void *samples, int samplecount)
{

	int i;

/*
	for (i=0 ; i<1024 ; i++)
	{
		debugbuffer[i][0] = ((signed char *) samples)[i];
		debugbuffer[i][1] = ((signed char *) samples)[i+1];
	}
	ALwritesamps(al_outport, debugbuffer, 1024);
*/

	ALwritesamps(al_outport, samples, samplecount*2);

}

void I_ShutdownSound(void)
{

	ALfreeconfig(al_config);
	ALcloseport(al_outport);

}

void I_ShutdownMusic(void)
{
}
