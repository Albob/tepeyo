#ifndef _BG_VRAM_MANAGER_H_
#define _BG_VRAM_MANAGER_H_

#include <nds.h>

class BGVramManager
{
	static const int VRAM_SIZE = 128*1024; // 128KB
	static const int TILE_SLOT_BOUNDARY = 16*1024; // 16KB
	u16 * vram;
	u16 * free_slot;

	int allignToSlots(int size);

public:
	// Manage given 128K VRAM slot
	BGVramManager(u16 * vram);
	// Reset the manager
	void reset();
	// Allocate size bytes of managed VRAM
	u16 * allocate(int size);
	// Returns the tile base number computed from the vram address
	int computeTileBase(u16 * addr);
};

#endif // _BG_VRAM_MANAGER_H_

