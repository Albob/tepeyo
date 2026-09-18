/*
* @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
* Tepeyollotl Engine
* Enjmin 2008
*/

#ifndef _CAMERA_
#define _CAMERA_

#include "Engine.h"
#include "Actor.h"

/** 
* Class Camera. Aims at an Entity and follows it
*/
class Camera: public Actor
{
private:
	Actor* m_target;

public:
	Camera(Engine * engine, Game* game);
	virtual ~Camera();

	void update();
	void moveTo(int12 x, int12 y);
	void lookAt(Actor* target);
	Actor * getLookingAt() const { return m_target; }
};

#endif

