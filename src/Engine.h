/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * @Author: Daniel "MsK'" Borges <msk@lywenn.eu.org>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#ifndef _ENGINE_
#define _ENGINE_

#include <stdio.h>
#include <stdlib.h>
#include <nds.h>
#include <fat.h>
#include "Sprite.h"
#include "Background.h"
#include "Sound.h"
#include "BGVramManager.h"
#include "16cSprite.h"

#define MAX_SPRITES	15
#define MAX_BG		4
#define MAX_SOUNDS	10
#define MAX_SCREENS	2 // Yes ! We are ready for Nintendo QS and Nintendo OS (Quad & Octo screens)

namespace Tepeyo {
	/* Some useful stuff */ 
	inline int distance(int x, int y) {	return sqrt32(x*x + y*y); }
	inline int abs(int i) {	return (i < 0) ? -i: i;	}
	inline int sign(int i) { return (i < 0) ? -1: 1; }
	inline int max(int a, int b) { return (a < b) ? b: a; }
	inline int min(int a, int b) { return (a < b) ? a: b; }

	// fixed point maths
	typedef struct int12 {
	  int value;
	  int12(int value) : value(value) { }
	  int12() : value(0) {}
	} int12;
	
	inline int12 fp12(int a) { 
	  int12 res;
	  res.value = a << 12;
	  return res;
	}
	
	inline int val12(int12 a) { return a.value >> 12; }

	inline int12 add12(int12 a, int12 b) { 
	  int12 res; 
	  res.value = a.value + b.value;
	  return res;
	}
	
	inline int12 sub12(int12 a, int12 b) {
	  int12 res;
	  res.value = a.value - b.value;
	  return res;
	}

	inline int12 mul12(int12 a, int12 b) { 
	  int12 res; 
	  res.value = ((long long)a.value * b.value)>>12; 
	  return res;
	}

	inline int12 div12(int12 a, int12 b) { 
	  int12 res;
	  res.value = (((long long)a.value<<12)/b.value);
	  return res;
	}

	inline int round12(int12 a) {
	  int12 res;
	  res.value = a.value + (1<<11);
	  return val12(res);
	}

#ifdef _NDSBUILD_
	inline int sqrt12(int12 a) { return sqrt32(a.value)<<6; }
#else
	inline int sqrt12(int12 a) { return (int)sqrtf(a.value)<<6; }
#endif
	inline int12 distance12(int12 x, int12 y) { 
	  int12 res;
	  res.value = sqrt12(add12(mul12(x,x), mul12(y,y))); 
	  return res;
	}

	inline int12 max12(int12 a, int12 b) { return (a.value < b.value) ? b: a; }
	inline int12 min12(int12 a, int12 b) { return (a.value < b.value) ? a: b; }
};

/**
 * This is the main class. It's purpose is to manage all the resources the way a NDS would manage them and adapt to the system (PC or NDS).
 * On a PC, creates instances of SDL resources, on a NDS, creates instances of NDS resources.
 */
class Engine
{
 private:
#ifdef _DEBUG
	 static FILE * flog;
	 static const int LOGTIME = 10;
	 static int logtime;
	 static bool logpending;
#endif

  /** Can store up to 4 shadow sprites in a single 64x64 sprite */
  //Sprite * m_shadowSprites;
  Sprite * m_sprites[MAX_SPRITES];
  Sprite16c * m_sprites16c[MAX_SPRITES];
  bool     m_sprite_availability[MAX_SPRITES];
  bool     m_sprite16c_availability[MAX_SPRITES];
  Background * m_backs[2][MAX_BG];
  Sound * m_sounds[MAX_SOUNDS];
  BGVramManager * bgVramManagers[MAX_SCREENS];

  int targetBrightness[MAX_SCREENS];

  // used by qsort :
  // void qsort(const void *, size_t, size_t, int (*compar)(const void *, const void *));
  static int spriteComparator(const void * a, const void * b);

 public:
  //Constructors
  Engine();
  ~Engine();

 public:
  /** Waits for the screen to redisplay (VBlank signal on NDS)  */
  void waitForDisplay();

  /** Allows to write in a debug file for debugging purpose */
  static void log(const char* message, ...);
  static void logError(const char* message, ...);

  /** Returns NULL if no sprite is available */
  Sprite * getAvailableSprite();
  Sprite16c * getAvailableSprite16c();

  /** Make the sprite available for another actor */
  void releaseSprite(Sprite * pSprite);
  void releaseSprite16c(Sprite16c * pSprite);

  /** Returns NULL if priority is not between 0 and 3, and screen's not between 0 (UP) et 1 (DOWN) */
  Background * getBackground(int priority, int screen);

  /** Make the background available for another actor */
  void resetBackgrounds(int screen);

  /** Update the resources. (streams sprites, sounds, etc) */
  void update();

  void init();
  Sound * newSound();
  //double getTime();
  
  bool isScreenTouched() const;
  bool isScreenClicked() const;
  bool isScreenReleased() const;
  touchPosition getTouchPosition();
  bool isBlowing() const;

  static inline void topSpritesOn() {
	  DISPLAY_CR |= DISPLAY_SPR_ACTIVE;
  }
  static inline void topSpritesOff() {
	  DISPLAY_CR &= ~DISPLAY_SPR_ACTIVE;
  }

  /** Sets nds' master brightness register */
  void setBrightness(int screen, int power); // screen(0=main, 1=sub), power=0~32, 0=black, 16=normal, 32=white
  int getBrightness(int screen);
  // Fade screen to target brightness
  void fade(int screen, int brightness);
};


#endif

