/*
 * Tepeyollotl Engine
 * Enjmin 2008
 * @author: Daniel "MsK`" Borges <msk@lywenn.eu.org>
 */

#include "BufferedSound.h"

#include <stdio.h>
#include "../common/SoundSystem.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Engine.h"

#define END 0
#define NOCHANNEL -1

// Buffers
#define SIZEu16(s) ((s)*1024/2)
#define SIZE(s) ((s)*1024)
static u16 buf400K[SIZEu16(400)];
static u16 buf200K1[SIZEu16(200)];
static u16 buf200K2[SIZEu16(200)];
static u16 buf100K[SIZEu16(100)];
static u16 buf62K1[SIZEu16(62)];
static u16 buf62K2[SIZEu16(62)];
static u16 buf50K1[SIZEu16(50)];
static u16 buf30K2[SIZEu16(30)];

// Pool
SoundPool::SoundBuffer SoundPool::pool[] = {
	{ buf400K, SIZE(400), NOCHANNEL },
	{ buf200K1, SIZE(200), NOCHANNEL },
	{ buf200K2, SIZE(200), NOCHANNEL },
	{ buf100K, SIZE(100), NOCHANNEL },
	{ buf62K1, SIZE(62), NOCHANNEL },
	{ buf62K2, SIZE(62), NOCHANNEL },
	{ buf50K1, SIZE(50), NOCHANNEL },
	{ buf30K2, SIZE(30), NOCHANNEL },
	{ END, END, NOCHANNEL },
};

u16 * SoundPool::getSoundBuffer(int channel, u32 size)
{
	SoundBuffer * buffer = 0, * ptr = &pool[0];

	// Look for the littlest buffer available for given size
	while ( ptr->buffer != END ) {
		if ( ptr->channel == NOCHANNEL && ptr->size >= size )
			if ( buffer == 0 || buffer->size > ptr->size )
				buffer = ptr;
		ptr++;
	}

	// If one found, mark as used
	if ( buffer )
		buffer->channel = channel;

	return buffer->buffer;
}

void SoundPool::update()
{
	SoundBuffer * ptr = & pool[0];

	// Free channels not used anymore
	while ( ptr->buffer != END ) {
		if ( ptr->channel != NOCHANNEL 
				&& SharedSoundCaCa->channels[ptr->channel].playing == 0
				&& SharedSoundCaCa->channels[ptr->channel].request == 0 )
			ptr->channel = NOCHANNEL;
		ptr++;
	}
}

// Let's rock
int BufferedSoundPlay(SoundFile * snd, bool loop, int volume, int panning)
{
	int channel;
	u16 * buffer;

	channel = SoundGetFreeChannel(1); // start at channel 1, because channel 0 is for streamed sound
	if ( channel != NOCHANNEL ) {
		buffer = SoundPool::getSoundBuffer(channel, snd->size);
		if ( buffer ) {
			// Load sound
			if ( snd->file == 0 )
				snd->file = fopen(snd->filename, "rb");
			if ( snd->file ) {
				fseek(snd->file, 0, SEEK_SET);
				fread(buffer, 1, snd->size, snd->file);
				// Play it !
				SharedSoundCaCa->channels[channel].data = buffer;
				SharedSoundCaCa->channels[channel].dataLength = snd->size;
				SharedSoundCaCa->channels[channel].loop = loop;
				SharedSoundCaCa->channels[channel].volume = volume;
				SharedSoundCaCa->channels[channel].panning = panning;
				SharedSoundCaCa->channels[channel].request = 1;
			}
#ifdef _DEBUG
			else {
				Engine::log("%s not found", snd->filename);
			}
#endif
			return channel;
		}
#ifdef _DEBUG
		else {
			Engine::log("No buffer available to play %s (%d)", snd->filename, snd->size);
		}
#endif
	}
#ifdef _DEBUG
	else {
		Engine::log("No channel available to play %s", snd->filename);
	}
#endif

	return NOCHANNEL;
}

void BufferedSoundStop(int channel)
{
	if ( channel != NOCHANNEL )
		SharedSoundCaCa->channels[channel].request = false;
}

