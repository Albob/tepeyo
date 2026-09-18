/*
 * Tepeyollotl Engine
 * Enjmin 2008
 * @author: Daniel "MsK`" Borges <msk@lywenn.eu.org>
 */

#ifndef _SOUND_SYSTEM_H_
#define _SOUND_SYSTEM_H_

#include <nds.h>

#define SoundCaCaChannels 16
#define SoundCaCaSampleRate 22016

typedef struct _SoundChannel {
	vu32 request : 1; // Request sound playing
	vu32 playing : 1; // Test if sound is playing on this channel
	vu32 loop : 1; // Set loop mode
	vu32 dataLength : 29; // Sound buffer length
	vu16 * data; // Sound buffer
	vu8 volume; // Volume (minimum=0, maximum=127)
	vu8 panning; // Panning (left=0, middle=64, right=127)
} SoundChannel;

typedef struct _SoundCaCa {
	SoundChannel channels[SoundCaCaChannels];
} SoundCaCa;

#define SharedSoundCaCa ((SoundCaCa *)((u32)(IPC)+sizeof(TransferRegion)))

#define NOCHANNEL -1
int SoundGetFreeChannel(int offset);
// 0 = minimum, 127 = maximum
inline void SoundSetVolume(int channel, int volume) { SharedSoundCaCa->channels[channel].volume = volume; }
// 0 = left, 64 = middle, 127 = right
inline void SoundSetPanning(int channel, int panning) { SharedSoundCaCa->channels[channel].panning = panning; }

#endif // _SOUND_SYSTEM_H_


