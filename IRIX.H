#ifndef __IRIX_H__
#define __IRIX_H__

void I_InitMusic(void);
void I_InitSound(int samplerate, int samplesound);

void I_SubmitOutputBuffer(void *samples, int samplecount);

void I_ShutdownSound(void);
void I_ShutdownMusic(void);

#endif

