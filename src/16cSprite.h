#ifndef _16CSPRITE_H_
#define _16CSPRITE_H_

#include <nds.h>

class Sprite16c {
	SpriteEntry m_top;
	SpriteEntry m_bottom;
	bool hidden;

public:
	Sprite16c();

	void setAlpha(bool alpha);
	void setPosition(int x_top, int y_top, int x_bottom, int y_bottom);
	void setSprite(int s);
	void setLayerPriority(int layer);

	SpriteEntry getTopSpriteEntry() const { return m_top; }
	SpriteEntry getBottomSpriteEntry() const { return m_bottom; }

	// Load a 4bpp raw image and given palette
	// count is the number of sprites in the image
	static bool load(const char * img, const char * pal, int count);

	// While hidden, a sprite16c can't move
	void show() { hidden = false; }
	void hide() { setPosition(256, 192, 256, 192); hidden = true; }
};


#endif // _16CSPRITE_H_

