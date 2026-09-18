/*
 * Modified version to suit Tepeyollotl needs
 * Copyright (C) 2007
 *   Daniel Borges (MsK`)
 */

/*---------------------------------------------------------------------------------

	default ARM7 core

	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>

#include "../../common/SoundSystem.h"

#define SOUND_FMT(format) ((format)<<29)

void VblankHandler(void)
{
	u32 i;

	for ( i = 0 ; i < SoundCaCaChannels ; i++ ) {
		// Process sound request
		if ( SharedSoundCaCa->channels[i].request ) {
			if ( SCHANNEL_CR(i) == 0 ) {
				SCHANNEL_TIMER(i) = SOUND_FREQ(SoundCaCaSampleRate);
				SCHANNEL_SOURCE(i) = (u32) SharedSoundCaCa->channels[i].data;
				SCHANNEL_LENGTH(i) = SharedSoundCaCa->channels[i].dataLength / 4; // length in words
				if ( SharedSoundCaCa->channels[i].loop )
					SCHANNEL_CR(i) = SCHANNEL_ENABLE | SOUND_REPEAT
								| SOUND_VOL(SharedSoundCaCa->channels[i].volume)
								| SOUND_PAN(SharedSoundCaCa->channels[i].panning)
								| SOUND_FMT(1); // PCM16
				else {
					SCHANNEL_CR(i) = SCHANNEL_ENABLE | SOUND_ONE_SHOT
								| SOUND_VOL(SharedSoundCaCa->channels[i].volume)
								| SOUND_PAN(SharedSoundCaCa->channels[i].panning)
								| SOUND_FMT(1); // PCM16
				}
			}
			if ( SCHANNEL_VOL(i) != SharedSoundCaCa->channels[i].volume )
				SCHANNEL_VOL(i) = SharedSoundCaCa->channels[i].volume & 0x7F;
			if ( SCHANNEL_PAN(i) != SharedSoundCaCa->channels[i].panning )
				SCHANNEL_PAN(i) = SharedSoundCaCa->channels[i].panning & 0x7F;
			// Stop one shot sounds that are finished
			if ( SharedSoundCaCa->channels[i].loop == 0 && (SCHANNEL_CR(i) & SCHANNEL_ENABLE) == 0 )
				SharedSoundCaCa->channels[i].request = false;
		} else {
			SCHANNEL_CR(i) = 0;
		}
		SharedSoundCaCa->channels[i].playing = SCHANNEL_CR(i) & SCHANNEL_ENABLE;
	}
}

void VcountHandler()
{
	static int lastbut = -1;
	touchPosition tempPos;
	
	uint16 but=0, x=0, y=0, xpx=0, ypx=0, z1=0, z2=0;

	but = REG_KEYXY;

	if (!( (but ^ lastbut) & (1<<6))) {
		tempPos = touchReadXY();

		if ( tempPos.x == 0 || tempPos.y == 0 ) {
			but |= (1 <<6);
			lastbut = but;
		} else {
			x = tempPos.x;
			y = tempPos.y;
			xpx = tempPos.px;
			ypx = tempPos.py;
			z1 = tempPos.z1;
			z2 = tempPos.z2;
		}
	} else {
		lastbut = but;
		but |= (1<<6);
	}
	
	// X key up
	but |= BIT(0);
	// Blow for dummies, mapped on X key
	u8 mic = MIC_ReadData();
	if ( mic > 226 || mic < 30 )
		but &= ~BIT(0); // X key down

	IPC->touchX		= x;
	IPC->touchY		= y;
	IPC->touchXpx	= xpx;
	IPC->touchYpx	= ypx;
	IPC->touchZ1	= z1;
	IPC->touchZ2	= z2;
	IPC->buttons	= but;
}

int main(int argc, char ** argv)
{
	// read User Settings from firmware
	readUserSettings();

	//enable sound
	powerON(POWER_SOUND);
	writePowerManagement(PM_CONTROL_REG,
			( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE )
			| PM_SOUND_AMP );
	SOUND_CR = SOUND_ENABLE | SOUND_VOL(0x7F);

	MIC_On();

	irqInit();

	// Start the RTC tracking IRQ
	initClockIRQ();

	SetYtrigger(80);
	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT);

	// Keep the ARM7 mostly idle
	while (1)
		swiWaitForVBlank();
}
