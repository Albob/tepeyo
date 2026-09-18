/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * @Author: Daniel Borges <msk@lywenn.eu.org>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#ifndef _BACKGROUND_
#define _BACKGROUND_


#include <stdio.h>
#include <stdlib.h>
#include <nds.h>

#include "BGVramManager.h"

/** Stores, manipulates, and displays the background of the levels (ie: the levels) */
class Background
{
public:
	typedef enum { NONE, LOAD, OVERLOAD } request_t;

 private:
	static const int HEADERSIZE = 20;
	static const int PALSIZE = 512;
	static const int TILESIZE = 64;
	static const int FILENAME_MAXLEN = 30;

	BGVramManager * manager;

	int screen;
	int priority;
	bool visible;

	request_t request;
	char filename[FILENAME_MAXLEN]; // filename of the current managed bg (not the displayed one ! a newer can have been requested)

	// Header
	u32 w, h, frames, speed, tilecount;

	// Data
	u16 * maps;
	u16 * tiles;

	// Current position
	int x, y;
	int lastx, lasty;

	// Animation values
	u32 frame, timer;

	void _load(); // Requests a new vram slot and load to it

public:
	Background(int screen, int priority, BGVramManager * manager);

	// Load requests (will be perform at update)
	// if overload is set, the load will be performed upon 
	bool load(const char * filename, bool overload = false);
	void unload();

	//getters
	inline int getTileCount() const { return tilecount; }
	inline int getWidth()	  const { return w; }
	inline int getHeight()	  const { return h; }

	//setters
	void setPosition(int x, int y);
	inline void setVisibility(bool _visible) { visible = _visible; }

	// Call me at VBLANK please please ohhhh please !
	void update();
};

#endif

