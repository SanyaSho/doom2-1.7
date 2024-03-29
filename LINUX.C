#include "sndserver.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/soundcard.h>
#include "irix.h"

int	audio_fd;

void myioctl(int fd, int command, int *arg)
{   
    int rc;
	extern int errno;
    rc = ioctl(fd, command, arg);  
	if (rc < 0)
	{
		fprintf(stderr, "ioctl(dsp,%d,arg) failed\n", command);
		fprintf(stderr, "errno=%d\n", errno);
	}
}

void I_InitMusic(void)
{
}

void I_InitSound(int samplerate, int samplesize)
{

    int i;
                
    audio_fd = open("/dev/dsp", O_WRONLY);
    if (audio_fd<0)
        fprintf(stderr, "Could not open /dev/dsp\n");
         
                     
    i = 11 | (2<<16);                                           
    myioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &i);
                    
    myioctl(audio_fd, SNDCTL_DSP_RESET, 0);
    i=11025;
    myioctl(audio_fd, SNDCTL_DSP_SPEED, &i);
    i=1;    
    myioctl(audio_fd, SNDCTL_DSP_STEREO, &i);
            
	myioctl(audio_fd, SNDCTL_DSP_GETFMTS, &i);
    if (i&=AFMT_S16_LE)    
        myioctl(audio_fd, SNDCTL_DSP_SETFMT, &i);
    else
        fprintf(stderr, "Could not play signed 16 data\n");

}

void I_SubmitOutputBuffer(void *samples, int samplecount)
{
	write(audio_fd, samples, samplecount*4);
}

void I_ShutdownSound(void)
{

	close(audio_fd);

}

void I_ShutdownMusic(void)
{
}
