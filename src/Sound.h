/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#ifndef _SOUND_
#define _SOUND_

#include <stdio.h>

/** 
 * A sound, according to the engine (musics, fx, voices...)
 */
class Sound
{
 private:

 public:
  /** Constructor(s) */
  Sound();
  Sound(const char* f);
  
  /** Operations */
  void play();
  void pause();
  void stop();
};

#endif

