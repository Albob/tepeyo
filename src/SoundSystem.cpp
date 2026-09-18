#include "../common/SoundSystem.h"

int SoundGetFreeChannel(int offset)
{
	// channel 1 is for streamed sounds, every other channel for buffered sounds
	for ( int i = offset ; i < 16 ; i++ )
		if ( ! SharedSoundCaCa->channels[i].playing && ! SharedSoundCaCa->channels[i].request )
			return i;
	return NOCHANNEL;
}

