/**
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * @author: Daniel "MsK`" Borges <msk@lywenn.eu.org>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#ifndef _SPRITE_
#define _SPRITE_

#include <stdio.h>
#include <stdlib.h>
#include <nds.h>

/**
 * Class Sprite
 * @author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * @author: Daniel "MsK'" Borges <msk@lywenn.eu.org>
 */
class Sprite
{
private:
	int y;
	/** is animation running ? */
	bool animate;
	bool m_playbackward;
	/** Number of the currently loaded frame */
	int m_loadedFrame;
	/** Index of the current frame */
	int m_currentFrame;
	/** Number of frames in the animation */
	int m_frameCount;
	/** Number of "screen frames" between two "Sprite frames" */
	int m_animationSpeed;
	/** Number of "screen frames" left before changing to new "Sprite frame" */
	int m_timer;
	/** Priority against other sprites (not related to priority against BGs */
	int priority;
	/** Loaded animation file */
	char m_filename[100];
	FILE * m_file;
	/** pointer to pixels in VRAM */
	u16 * m_vramSlot;
	/** NDS Sprites (for OAM) */
	SpriteEntry m_top;
	SpriteEntry m_bottom;
	bool m_visible;

	// load given frame from file to VRAM (streaming rulezz)
	void loadFrame(int frame);

public:
	/** Constructor */
	Sprite(u16* vramSlot);

	//Setters
	/** move the sprite to a new location */
	void setPosition(int x_top, int y_top, int x_bottom, int y_bottom);
	/** Sets the current frame to the specified index */
	inline void setFrame(int index)	{ m_timer = m_animationSpeed; m_currentFrame = index; }
	/** Toggles automatic animation on/off */
	inline void setAnimation(bool on) { animate = on; }
	/** Set priority */
	inline void setPriority(int layer) { priority = layer; }
	inline void setPlayBackward(bool on) { m_playbackward = on; }
	void setVisibility(bool yes);

	//Getters
	/** Returns the number of frames in the current sprite */
	inline int getFrameCount() const { return m_frameCount; }
	inline int getAnimationSpeed() const { return m_animationSpeed; }
	/** Returns the index of the current Frame */
	inline int getCurrentFrame() const { return m_currentFrame; }
	inline SpriteEntry getTopSpriteEntry() const { return m_top; }
	inline SpriteEntry getBottomSpriteEntry() const { return m_bottom; }
	inline int getPriority() const { return priority; }
	inline int getY() const { return y; }

	//Operations
	/** Set the current frame to the next frame and loads the right piece of sprite in vram */
	void update();
	/** Force the sprite to load a new animation */
	void load(const char* filename);
	/** Reset sprite, ready to be loaded */
	void reset();
	/** Advance to next frame */
	inline void nextFrame()
		{ m_currentFrame++; if ( m_currentFrame == m_frameCount ) m_currentFrame = 0; }
};


#endif

