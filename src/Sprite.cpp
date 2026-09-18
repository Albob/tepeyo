/** 
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * @author: Daniel "MsK`" Borges <msk@lywenn.eu.org>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#include "Sprite.h"
#include <string.h>
#include "Engine.h"

Sprite::Sprite(u16* vramSlot)
  : y(0), animate(0), m_playbackward(0), m_currentFrame(0), m_frameCount(0), m_animationSpeed(0),
	m_timer(0), priority(128), m_file(0), m_vramSlot(vramSlot), m_visible(true)
{
	u16 defaultSprite[4] = {
		ATTR0_NORMAL | ATTR0_BMP | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(192),
		ATTR1_SIZE_64 | OBJ_X(256),
		ATTR2_PRIORITY(2) | ATTR2_ALPHA(15), // prio1 = between bg3/2 and bg0/1
		0,
	};

	m_filename[0] = 0;

	memcpy(& m_top, defaultSprite, sizeof(SpriteEntry));
	memcpy(& m_bottom, defaultSprite, sizeof(SpriteEntry));

	m_top.tileIdx = m_bottom.tileIdx = 
		(int) (vramSlot - SPRITE_GFX) / 64;
		// 128B is vram sprite boundary in normal 1D BMP mode
}

void
Sprite::update()
{
	//Engine::log("Sprite::update()");
	//Engine::log("state: %d/%d, %d/%d", m_timer, m_animationSpeed, m_currentFrame, m_frameCount);
	if ( animate ) {
		if ( m_timer <= 0 ) {
		  if (!m_playbackward)
			{
			  m_currentFrame++;
			  if ( m_currentFrame >= m_frameCount )
				m_currentFrame = 0;
			}
		  else
			{
			  m_currentFrame--;
			  if ( m_currentFrame < 0 )
				m_currentFrame = m_frameCount - 1;
			}
			m_timer = m_animationSpeed;
			loadFrame(m_currentFrame);
		} else
			m_timer--;
	} else
		loadFrame(m_currentFrame);
}

void
Sprite::setPosition(int x_top, int y_top, int x_bottom, int y_bottom)
{
	if ( m_visible == false ) {
		x_top = 256;
		x_bottom = 256;
		y_top = 192;
		y_bottom = 192;
	}

	y = y_top;

	if ( x_top >= SCREEN_WIDTH || x_top <= -64
		|| y_top >= SCREEN_HEIGHT || y_top <= -64 ) { // off-screen
			m_top.posX = 256;
	} else {
		if ( x_top < 0 )
			m_top.posX = x_top + 512;
		else
			m_top.posX = x_top;
		if ( y_top < 0 )
			m_top.posY = y_top + 256;
		else
			m_top.posY = y_top;
	}

	if ( x_bottom >= SCREEN_WIDTH || x_bottom <= -64
		|| y_bottom >= SCREEN_HEIGHT || y_bottom <= -64 ) { // off-screen
			m_bottom.posX = 256;
	} else {
		if ( x_bottom < 0 )
			m_bottom.posX = x_bottom + 512;
		else
			m_bottom.posX = x_bottom;
		if ( y_bottom < 0 )
			m_bottom.posY = y_bottom + 256;
		else
			m_bottom.posY = y_bottom;
	}

	/*Engine::log("setPosition(x_top=%d, y_top=%d, x_bottom=%d, y_bottom=%d)\n"
		"m_top(%d,%d), m_bottom(%d,%d)",
		x_top, y_top, x_bottom, y_bottom,
		m_top.posX, m_top.posY, m_bottom.posX, m_bottom.posY
		);*/
}

void
Sprite::loadFrame(int frame)
{
	if ( frame == m_loadedFrame )
		return;

	static u16 buffer[64*64] ALIGN(32);
	static const int HEADER = 16; // header size
	static const int FRAME = 64*64*2; // frame size
	//FILE * f = fopen(m_filename, "rb");
	//if (!f)
	//{
//		Engine::log("Couldn't open the sprite");
//		return;
//	}

	//Engine::log("Sprite::loadFrame(%s, %d)", m_filename, frame);

	// go to frame then read it
	fseek(m_file, HEADER + frame * FRAME, SEEK_SET);
	fread(buffer, FRAME, 1, m_file);

#ifdef _NDSBUILD_
	// Flush cache
	DC_FlushRange(buffer, FRAME);

	// HiSpeed Copy - hope it will work this time...
	// Upper screen
	dmaCopy(buffer, m_vramSlot, FRAME);
	// Lower screen
	dmaCopy(buffer, m_vramSlot + 0x100000, FRAME);
#else
	// On PC : m_vramSlot points to a zone accessible from both screens
	memcpy(m_vramSlot, buffer, FRAME);
#endif
	//fclose(f);

	m_loadedFrame = frame;
}

void
Sprite::load(const char * filename)
{
    char filepath[64];
	strcpy(filepath, "/tepeyo/spr/");
	strcat(filepath, filename);
	Engine::log("Sprite::load(%s)", filepath);

	if (strcmp(m_filename, filepath) == 0)
	  return; //file already loaded

	if ( m_filename[0] != 0 ) {
		fclose(m_file);
		m_file = 0;
		m_filename[0] = 0;
	}

	strcpy(m_filename, filepath);

	m_file = fopen(m_filename, "rb");

	if (!m_file)
	{
		Engine::log("Couldn't open %s", filepath);
		return;
	}

	fseek(m_file, 8, SEEK_SET); // skip width and height from header
	fread(& m_frameCount, sizeof(int), 1, m_file);
	fread(& m_animationSpeed, sizeof(int), 1, m_file);

	//fclose(f);

	if ( m_currentFrame >= m_frameCount )
		m_currentFrame = 0;

	m_loadedFrame = -1; // force loading
}

void Sprite::reset()
{
	m_timer = 0;
	m_animationSpeed = 0;
	m_currentFrame = 0;
	m_frameCount = 1;
	animate = false;
	m_filename[0] = 0;
}

void Sprite::setVisibility(bool ok)
{
	if ( ok ) {
		m_visible = true;
	} else {
		setPosition(256, 192, 256, 192);
		m_visible = false;
	}
}
