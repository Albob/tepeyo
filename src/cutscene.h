#ifndef _CUTSCENE_H_
#define _CUTSCENE_H_

#include "Engine.h"

#define CUTSCENE_INTRO 0
#define CUTSCENE_OUTRO 1

void playCutScene(Engine * engine, int id, bool stopMusic = true);

#endif // _CUTSCENE_H_
