/*
 * @Author: Daniel Borges <msk@lywenn.eu.org>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#include "BGVramManager.h"
#include "Engine.h"

BGVramManager::BGVramManager(u16 * _vram)
: vram(_vram)
{
	reset();
}

void BGVramManager::reset()
{
	// We do not use first tile slot as it's used for maps
	free_slot = vram + TILE_SLOT_BOUNDARY/sizeof(u16);
}

int BGVramManager::allignToSlots(int size)
{
	if ( size % TILE_SLOT_BOUNDARY == 0 )
		return size;
	else
		return size + (TILE_SLOT_BOUNDARY - size % TILE_SLOT_BOUNDARY);
}

u16 * BGVramManager::allocate(int size)
{
	u16 * memory = free_slot;

	free_slot += allignToSlots(size)/sizeof(u16);

#ifdef _DEBUG
	if ( (int)free_slot > (int)vram + VRAM_SIZE ) {
		Engine::log("[BGVramManager] WARNING : NOT ENOUGH MEMORY !\n"
					":: Allocating %p size(%d -> %d)",
					memory, size, allignToSlots(size));
	}
#endif

	return memory;
}

int BGVramManager::computeTileBase(u16 * addr)
{
	return ((int)addr - (int)vram)/TILE_SLOT_BOUNDARY;
}

