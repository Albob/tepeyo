/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * @Author: Daniel Borges <msk@lywenn.eu.org>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#include "Background.h"

#include "Engine.h"
#include <string.h>

#define BGx_CR(s,bg) (*(vuint16*)(0x04000008+(bg)*2+(s)*0x1000))
#define BGx_X0(s,bg) (*(vuint16*)(0x04000010+(bg)*4+(s)*0x1000))
#define BGx_Y0(s,bg) (*(vuint16*)(0x04000012+(bg)*4+(s)*0x1000))
#define DISPLAY_REG(s) (*(vuint32*)(0x04000000+(s)*0x1000))
#define DISPLAY_BGx_ACTIVE(bg)	(1 << ((bg)+8))
#undef BG_MAP_RAM
#define BG_MAP_RAM(s, p) ((u16 *) ((s)*0x200000 + (p)*0x800 + 0x06000000))

Background::Background(int screen, int priority, BGVramManager * manager)
	: manager(manager), screen(screen), priority(priority), visible(false),
	  request(NONE), maps(0)
{
	filename[0] = 0;
}

void Background::_load()
{
	char filepath[64];
	strcpy(filepath, "/tepeyo/bg/");
	strcat(filepath, filename);

	FILE * f = fopen(filepath, "rb");
	if ( f == 0 ) {
		Engine::log("[BG::_load] Could not open \"%s\"", filepath);
		return;
	}

	// Go to palette
	fseek(f, HEADERSIZE, SEEK_SET);

	// Load palette
	static u16 pal[PALSIZE/sizeof(u16)] ALIGN(32);
	fread(pal, sizeof(u16), PALSIZE/sizeof(u16), f);
#ifdef _NDSBUILD_
	DC_FlushRange(pal, PALSIZE);
	if ( screen == 0 ) { // MAIN
		if ( priority < 2 ) {
			// Unlock VRAM
			vramSetBankF(VRAM_F_LCD);
			// Load palette
			dmaCopy(pal, VRAM_F + 0x1000 * priority, 512);
			// Lock VRAM
			VRAM_F_CR = VRAM_ENABLE | VRAM_F_BG_EXT_PALETTE | VRAM_OFFSET(0);
		} else {
			// Unlock VRAM
			vramSetBankG(VRAM_G_LCD);
			// Load palette
			dmaCopy(pal, VRAM_G + 0x1000 * (priority-2), 512);
			// Lock VRAM
			VRAM_G_CR = VRAM_ENABLE | VRAM_G_BG_EXT_PALETTE | VRAM_OFFSET(1);
			// Copy color 0 to backdrop if lower layer
			if ( priority == 3 )
				BG_PALETTE[0] = pal[0];
		}
	} else { // SUB
		// Unlock VRAM
		vramSetBankH(VRAM_H_LCD);
		// Load palette
		dmaCopy(pal, VRAM_H + 0x1000 * priority, 512);
		// Lock VRAM
		vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
		// Copy color 0 to backdrop if lower layer
		if ( priority == 3 )
			BG_PALETTE_SUB[0] = pal[0];
	}
#else
	memcpy(palettes[screen][priority], pal, PALSIZE);
#endif

	// Jump over maps
	fseek(f, frames * (w/8) * (h/8) * sizeof(u16), SEEK_CUR);

	// Load tiles
	if ( request == LOAD )
		tiles = manager->allocate(tilecount * TILESIZE);

	u8 * tile = (u8 *) tiles;
	static u8 buffer[TILESIZE] ALIGN(32);
	for ( u32 i = 0 ; i < tilecount ; i++ ) {
		fread(buffer, sizeof(u8), TILESIZE, f);
#ifdef _NDSBUILD_
		DC_FlushRange(buffer, TILESIZE);
		dmaCopy(buffer, tile, TILESIZE);
#else
		memcpy(tile, buffer, TILESIZE);
#endif
		tile += TILESIZE;
	}

	fclose(f);

#ifdef _DEBUG
	Engine::log("%sBG%d Loaded : %s -> Size(%dx%d->%dx%d) Tiles(%d)%s",
			(screen==0)?"":"Sub", priority,
			filename, w, h, w/8, h/8, tilecount,
			(frames > 1)?" animated":"");
#endif

#ifdef _NDSBUILD_
	// Setup register
	BGx_CR(screen, priority) = BG_256_COLOR | BG_64x32
			| BG_MAP_BASE(priority*2) | BG_PRIORITY(priority)
			| BG_TILE_BASE(manager->computeTileBase(tiles));
#else
#endif

	request = NONE;
}

bool Background::load(const char * _filename, bool overload)
{
	char filepath[64];
	strcpy(filepath, "/tepeyo/bg/");
	strcat(filepath, _filename);

	FILE * f = fopen(filepath, "rb");
	if ( f == 0 ) {
		Engine::logError("Background::load(%s) Could not open file", filepath);
		return false;
	}

	strncpy(filename, _filename, FILENAME_MAXLEN);

	// Read header
	fread(& w, sizeof(u32), 1, f);
	fread(& h, sizeof(u32), 1, f);
	fread(& frames, sizeof(u32), 1, f);
	fread(& speed, sizeof(u32), 1, f);
	fread(& tilecount, sizeof(u32), 1, f);

	// Free old map
	if ( maps )
		delete maps;

	// Go to maps
	fseek(f, HEADERSIZE + PALSIZE, SEEK_SET);

	// Load maps
	maps = new u16[frames*(w/8)*(h/8)];
	u16 * ptr = maps;
	for ( u32 i = 0 ; i < frames ; i++ ) {
		fread(ptr, sizeof(u16), (w/8)*(h/8), f);
		ptr += (w/8)*(h/8);
	}

	fclose(f);

	request = overload ? OVERLOAD : LOAD;

	frame = 0;
	timer = 0;
	x = 0;
	y = 0;
	lastx = -256;
	lasty = -256;

	return true;
}

void Background::setPosition(int _x, int _y)
{
	// Adjust to valid positions
	if ( _x < 0 )
		_x = 0;
	else if ( (u32)_x > w-256 )
		_x = w-256;
	if ( _y < 0 )
		_y = 0;
	else if ( (u32)_y > h-192 )
		_y = h-192;

	x = _x;
	y = _y;
}

void Background::unload()
{
	visible = false;
	if ( maps ) {
		delete maps;
		maps = 0;
	}
}

void Background::update()
{
	if ( request != NONE )
		_load();

	if ( visible ) {
		// Enable BG
		DISPLAY_REG(screen) =
			DISPLAY_REG(screen) | DISPLAY_BGx_ACTIVE(priority);

		// Animation
		timer++;
		if ( timer == speed ) {
			timer = 0;
			frame = (frame + 1) % frames;
		}
		
		// NDS scrolling
		BGx_X0(screen, priority) = x % 8;
		BGx_Y0(screen, priority) = y % 8;

		// Global scrolling
		if ( x/8 != lastx/8 || y/8 != lasty/8 || timer == 0 ) {
			u16 * inmap = & maps[frame * (w/8) * (h/8) + (x/8) + (y/8) * (w/8)];
			u16 * outmap = BG_MAP_RAM(screen, priority*2);

			// TODO optimize
			// Fill first 32x32 map
			for ( int i = 0 ; i < 25 ; i++ )
				for ( int j = 0 ; j < 32 ; j++ )
					outmap[i*32+j] = inmap[i*(w/8)+j];
			// Fill second 32x32 map (just the first column)
			for ( int i = 0 ; i < 25 ; i++ )
				outmap[32*32+i*32] = inmap[i*(w/8)+32];
		}

		lastx = x;
		lasty = y;
	} else {
		// Disable BG
		DISPLAY_REG(screen) =
			DISPLAY_REG(screen) & ~DISPLAY_BGx_ACTIVE(priority);
	}
}

