/*
 * Tepeyollotl Engine
 * Enjmin 2008
 * @author: Daniel "MsK`" Borges <msk@lywenn.eu.org>
 */

#ifndef _BUFFERED_SOUND_H_
#define _BUFFERED_SOUND_H_

#include <nds.h>
#include <stdio.h>

struct SoundFile {
	const char * filename;
	u32 size;
	FILE * file;
};
extern SoundFile soundFiles[];

#define SND_TONATIUH_BOIS			0
#define SND_TONATIUH_CAILLOUX		1
//#define SND_TONATIUH_PIERRE			2
#define SND_TONATIUH_TERRE			3
#define SND_TONATIUH_DEFUSION		4
#define SND_TONATIUH_FUSION			5
#define SND_TONATIUH_FUSIONFINALE	6
#define SND_TONATIUH_CHUTE 			7
#define SND_TONATIUH_CORNE 			8
#define SND_TONATIUH_METEORITE		9
#define SND_TONATIUH_TELEPORT		10
#define SND_TONATIUH_RESPIRATION	11
#define SND_TONATIUHFUSION_BOIS		12
#define SND_TONATIUHFUSION_CAILLOUX 13
#define SND_TONATIUHFUSION_INERTE	14
//#define SND_TONATIUHFUSION_PIERRE	15
#define SND_TONATIUHFUSION_TERRE	16
#define SND_RELIC_ATTERIT			17
#define SND_RELIC_CHARGE 			18
#define SND_RELIC_GLISSE 			19
#define SND_RELIC_HAND				20
#define SND_RELIC_JETTE				21
#define SND_RELIC_POSEAUTEL			22
//#define SND_RELIC_PREND				23
#define SND_RELIC_RECHARGE 			24
#define SND_RELIC_ROTATION 			25
#define SND_ALTAR_CLOCHE0 			26
#define SND_ALTAR_CLOCHE1 			27
#define SND_ALTAR_CLOCHE2 			28
#define SND_ALTAR_CLOCHE3 			29
#define SND_ALTAR_ENERGY			30
#define SND_DOOR					31
#define SND_BERSERK_PAS 			32
#define SND_CHOC_METEORIT			33
#define SND_AFFICHE					34
#define SND_TONATIUH_FFINALE1		35
#define SND_TONATIUH_FFINALE2		36
#define SND_TONATIUH_FFINALE3		37

#define SND_LOOP true

class SoundPool {
	friend int BufferedSoundPlay(SoundFile *, bool, int, int);

	struct SoundBuffer {
		u16 * buffer;
		u32 size;
		s8 channel;
	};

	static SoundBuffer pool[];

	static u16 * getSoundBuffer(int channel, u32 size);

public:
	static void update();
};

int BufferedSoundPlay(SoundFile * snd, bool loop = false, int volume = 127, int panning = 64);
inline int BufferedSoundPlay(int id, bool loop = false, int volume = 127, int panning = 64) {
	return BufferedSoundPlay(& soundFiles[id], loop, volume, panning);
}

// Stop given channel
void BufferedSoundStop(int channel);

#endif // _BUFFERED_SOUND_H_



