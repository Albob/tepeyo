#include "16cSprite.h" 
 
#include <string.h> 
#include <stdio.h> 
 
Sprite16c::Sprite16c() 
: hidden(false) 
{
	u16 defaultSprite[4] = {
		ATTR0_NORMAL | ATTR0_COLOR_16 | ATTR0_SQUARE | OBJ_Y(192),
		ATTR1_SIZE_32 | OBJ_X(256),
		ATTR2_PRIORITY(2) | ATTR2_PALETTE(0), // prio1 = between bg3/2 and bg0/1
		0,
	}; 

	memcpy(& m_top, defaultSprite, sizeof(SpriteEntry));
	memcpy(& m_bottom, defaultSprite, sizeof(SpriteEntry)); 
} 
 
void Sprite16c::setAlpha(bool alpha) 
{ 
	if ( alpha ) { 
		m_top.objMode = (tObjMode) OBJMODE_BLENDED; 
		m_bottom.objMode = (tObjMode) OBJMODE_BLENDED; 
	} else { 
		m_top.objMode = (tObjMode) OBJMODE_NORMAL; 
		m_bottom.objMode = (tObjMode) OBJMODE_NORMAL; 
	} 
} 
 
void Sprite16c::setPosition(int x_top, int y_top, int x_bottom, int y_bottom) 
{
	if ( hidden )
		return;

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
} 
 
void Sprite16c::setSprite(int s) 
{ 
	// 960 = jump over the 15 64x64 sprites (64x64x2Bpp/128Bboundary=960) 
	m_top.tileIdx = m_bottom.tileIdx = 960 + s * 4; // a 32x32 16c sprite is 512 bytes, tile boundary is 128 
} 
 
void Sprite16c::setLayerPriority(int layer) 
{
	m_top.objPriority = (tObjPriority) layer;
	m_bottom.objPriority = (tObjPriority) layer; 
} 
 
bool Sprite16c::load(const char * img, const char * pal, int count) 
{ 
	FILE * f; 
	static u16 buffer[4096] ALIGN(32); 
	char filepath[64];
 	
	// Load sprites 
	strcpy(filepath, "/tepeyo/spr/");
	strcat(filepath, img); 
	f = fopen(filepath, "rb"); 
	if ( f == 0 ) 
		return false; 
	fread(buffer, 1, count * 32 * 32 / 2, f); 
	DC_FlushRange(buffer, count * 32 * 32 / 2); 
	dmaCopy(buffer, SPRITE_GFX + (15 * 64 * 64), count * 32 * 32 / 2); 
	dmaCopy(buffer, SPRITE_GFX_SUB + (15 * 64 * 64), count * 32 * 32 / 2); 
	fclose(f); 
 
	// Load 16 colors palette 
	strcpy(filepath, "/tepeyo/spr/");
	strcat(filepath, pal);
	f = fopen(filepath, "rb");
	if ( f == 0 ) 
		return false; 
	fread(buffer, 1, 16 * 2, f); 
	DC_FlushRange(buffer, 16 * 2); 
	dmaCopy(buffer, SPRITE_PALETTE, 16 * 2); 
	dmaCopy(buffer, SPRITE_PALETTE_SUB, 16 * 2); 
	fclose(f); 
 
	return true; 
}
