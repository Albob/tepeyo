/*
 * Tepeyollotl Engine
 * Enjmin 2008
 * @author: Daniel "MsK`" Borges <msk@lywenn.eu.org>
 */

#ifndef _STREAMED_SOUND_H_
#define _STREAMED_SOUND_H_

#include <nds.h>
#include <stdio.h>

// In half words
#define StreamedSoundBufferLength 1536
#define StreamedSoundChannel 0

// Don't instanciate !
class StreamedSound {
	friend void StreamedSoundVBL(void);

	// TIMER1 ticks (used to compute the number of samples played since last VBL)
	int lastTick;
	// Ring buffer
	u16 buffer[StreamedSoundBufferLength] ALIGN(32);
	u16 * bufferEnd;
	u16 * bufRingPtr;
	// Streamed file
	FILE * file;

	// effects
	typedef enum { FADEOUT, NONE, FADEIN } Effect;
	Effect effect;

public:
	StreamedSound();

	void load(const char * filename);

	void play();
	void stop();

	// 0 = minimum, 127 = maximum
	void setVolume(int n);
	int getVolume();

	// 0 = left, 64 = middle, 127 = right
	void setPanning(int n);
	int getPanning();

	void fadeout();
	void fadein();

	void update();
};

// singleton style
StreamedSound * getStreamedSound();

#endif // _STREAMED_SOUND_H_

