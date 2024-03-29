
#ifndef __SOUNDSTH__
#define __SOUNDSTH__

#define S_MAX_VOLUME	127
#define S_CLIPPING_DIST (1200*0x10000) // when to clip out sounds
#define S_CLOSE_DIST    (200*0x10000)  // when sounds should be max'd out
#define S_ATTENUATOR    ((S_CLIPPING_DIST-S_CLOSE_DIST)>>FRACBITS)
#define NORM_PITCH	128
#define NORM_PRIORITY	64
#define NORM_VOLUME	snd_MaxVolume
#define S_PITCH_PERTURB	1
#define NORM_SEP	128
#define S_STEREO_SWING	(96*0x10000)
#define S_IFRACVOL	30 // % attenuation from front to back
#define NA		0

#define S_NUMCHANNELS	2

// See gensounds.h and gensounds.c for what soundst.h is made of.

typedef struct
{
  char *name;		// up to 6-character name
  int lumpnum;		// lump number of music
  void *data;		// music data
  int handle;		// music handle once registered
} musicinfo_t;

typedef struct sfxinfo_struct sfxinfo_t;
struct sfxinfo_struct
{
  char *name;		// up to 6-character name
  int singularity;	// Sfx singularity (only one at a time)
  int priority;		// Sfx priority
  sfxinfo_t *link;	// referenced sound if a link
  int pitch;		// pitch if a link
  int volume;		// volume if a link
  void *data;		// sound data
  int usefulness;	// this is checked every second to see if sound
			// can be thrown out (if 0, then decrement, if -1,
			// then throw out, if > 0, then it's in use)
  int lumpnum;		// lump number of sfx
};

typedef struct
{
  sfxinfo_t *sfxinfo;		// sound information (if null, channel avail.)
  void *origin;		// origin of sound
  int handle;			// handle of the sound being played
} channel_t;

enum {Music, Sfx, SfxLink};
enum {PC=1, Adlib=2, SB=4, Midi=8}; // cards available
enum {sfxThrowOut=-1, sfxNotUsed=0};

// Initialize the sound code at start of level
//
void S_Start(void);

// Start sound for thing at <origin> using <sound_id> from sounds.h
//
extern void S_StartSound(void *origin, int sound_id);

// Will start a sound at a given volume.
extern void S_StartSoundAtVolume(void *origin, int sound_id, int volume);

// Stop sound for thing at <origin>
//
extern void S_StopSound(void *origin);

// Start music using <music_id> from sounds.h
//
extern void S_StartMusic(int music_id);

// Start music using <music_id> from sounds.h, and set whether looping
//
extern void S_ChangeMusic(int music_id, int looping);

// Stops the music
//
extern void S_StopMusic(void);

void S_PauseSound(void);
void S_ResumeSound(void);

// Updates music & sounds
//
extern void S_UpdateSounds(void *listener);

void S_SetMusicVolume(int volume);
void S_SetSfxVolume(int volume);

// Initializes sound stuff, including volume
//
void S_Init(int , int);

//--------
//SOUND IO
//--------
#define	FREQ_LOW		0x40
#define FREQ_NORM		0x80
#define FREQ_HIGH		0xff

void I_SetMusicVolume(int volume);
void I_SetSfxVolume(int volume);

//  MUSIC I/O
//

void I_PauseSong(int handle);
void I_ResumeSong(int handle);

void I_PlaySong(int handle, int looping);
// called by anything that wishes to start music.
// plays a song, and when the song is done, starts playing it again in
// an endless loop.

void I_StopSong(int handle);
// stops a song over 3 seconds.

int I_RegisterSong(void *data);
// registers a song handle to song data

void I_UnRegisterSong(int handle);
// see above then think backwards

int I_QrySongPlaying(int handle);
// is the song playing?

//  SFX I/O
//

void I_SetChannels(int channels);

int I_GetSfxLumpNum (sfxinfo_t *);

int I_StartSound (int id, void *data, int vol, int sep, int pitch, int priority);
// Starts a sound in a particular sound channel

void I_UpdateSoundParams(int handle, int vol, int sep, int pitch);
// Updates the volume, separation, and pitch of a sound channel

void I_StopSound(int handle);
// Stops a sound channel

int I_SoundIsPlaying(int handle);
// called by S_*()'s to see if a channel is still playing.  Returns 0
// if no longer playing, 1 if playing.

extern sfxinfo_t	S_sfx[];	// the complete set of sound effects
extern musicinfo_t	S_music[];	// the complete set of music

#endif
