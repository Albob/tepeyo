/*
 * Tepeyollotl Engine
 * Enjmin 2008
 * @author: Daniel "MsK`" Borges <msk@lywenn.eu.org>
 */

#include "StreamedSound.h"

#include "../common/SoundSystem.h"

static StreamedSound ssound ALIGN(32);

StreamedSound * getStreamedSound()
{
	return & ssound;
}

StreamedSound::StreamedSound()
{
	file = 0;
	SharedSoundCaCa->channels[StreamedSoundChannel].loop = 1;
	SharedSoundCaCa->channels[StreamedSoundChannel].dataLength =
		StreamedSoundBufferLength * sizeof(u16);
	SharedSoundCaCa->channels[StreamedSoundChannel].data = buffer;
	SharedSoundCaCa->channels[StreamedSoundChannel].panning = 64;
	bufferEnd = buffer + StreamedSoundBufferLength;
	bufRingPtr = buffer;
	effect = NONE;
}

void StreamedSound::load(const char * filename)
{
	stop();
	file = fopen(filename, "rb");
	if ( file ) {
		fread(buffer, 2, StreamedSoundBufferLength, file);
		bufRingPtr = buffer;
	}
}

void StreamedSound::play()
{
	if ( file ) {
		SharedSoundCaCa->channels[StreamedSoundChannel].request = 1;
		swiWaitForVBlank();
		lastTick = TIMER1_DATA;
	}
}

void StreamedSound::stop()
{
	if ( file ) {
		fclose(file);
		file = 0;
		SharedSoundCaCa->channels[StreamedSoundChannel].request = 0;
	}
}

void StreamedSound::update()
{
	int tick, samples, read, s;

	if ( file ) {
		tick = TIMER1_DATA;
		samples = tick - lastTick;
		if ( samples < 0 )
			samples += 0xFFFF;
		lastTick = tick;
		do {
			s = samples;
			if ( bufRingPtr + s >= bufferEnd )
				s = bufferEnd - bufRingPtr;
			read = fread(bufRingPtr, 2, s, file);
			// loop file
			if ( read < s ) {
				fseek(file, 0, SEEK_SET);
				fread(bufRingPtr + read, 2, s - read, file);
			}
			bufRingPtr += s;
			samples -= s;
			// ring back
			if ( s < samples ) {
				bufRingPtr = buffer;
				s = samples - s;
				if ( s >= StreamedSoundBufferLength )
					s = StreamedSoundBufferLength-1;
				read = fread(bufRingPtr, 2, s, file);
				// loop file
				if ( read < s ) {
					fseek(file, 0, SEEK_SET);
					fread(bufRingPtr + read, 2, s - read, file);
				}
				bufRingPtr += s;
				samples -= s;
			}
		} while ( samples > 0 );

		// fades done at same speed as video fade
		if ( effect == FADEOUT ) {
			if ( getVolume() == 0 )
				effect = NONE;
			setVolume(getVolume() - 8);
		} else if ( effect == FADEIN ) {
			if ( getVolume() == 127 )
				effect = NONE;
			setVolume(getVolume() + 8);
		}
	}
}

int StreamedSound::getVolume()
{
	return SharedSoundCaCa->channels[StreamedSoundChannel].volume;
}

void StreamedSound::setVolume(int volume)
{
	if ( volume < 0 )
		volume = 0;
	if ( volume > 127 )
		volume = 127;
	SharedSoundCaCa->channels[StreamedSoundChannel].volume = volume;
}

void StreamedSound::fadeout()
{
	effect = FADEOUT;
}

void StreamedSound::fadein()
{
	effect = FADEIN;
}

int StreamedSound::getPanning()
{
	return SharedSoundCaCa->channels[StreamedSoundChannel].panning;
}

void StreamedSound::setPanning(int pan)
{
	if ( pan < 0 )
		pan = 0;
	if ( pan > 127 )
		pan = 127;
	SharedSoundCaCa->channels[StreamedSoundChannel].panning = pan;
}

void StreamedSoundVBL(void)
{
	ssound.update();
}

