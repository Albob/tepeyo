/*
* @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
* Tepeyollotl Engine
* Enjmin 2008
*/

#ifndef _ACTOR_
#define _ACTOR_

#include "Engine.h"
#include "Entity.h"
#include "Sprite.h"

using namespace Tepeyo;

/** 
* Class Actor
*/
class Actor: public Entity
{
protected:
	int12 m_speed;
	int12 m_maxSpeed;
	int12 m_minSpeed;
	/** The clip rectangle defines the collision zone of the actor */
	int m_clipX, m_clipY, m_clipWidth, m_clipHeight;
	int m_xOrigin, m_yOrigin;
	int m_direction; //the direction the actor is facing
	int m_oldDirection;

public:
	Actor(Engine * engine, Game * game);
	virtual ~Actor();

	//getters
	int12 getSpeed()    { return m_speed; }
	int12 getMaxSpeed() { return m_maxSpeed; }
	int12 getMinSpeed() { return m_minSpeed; }
	int getClipX()      { return m_clipX; }
	int getClipY()      { return m_clipY; }
	int getClipWidth()  { return m_clipWidth; }
	int getClipHeight() { return m_clipHeight; }
	inline int getDirection()     const { return m_direction; }
	inline int getOldDirection()  const { return m_oldDirection; }
	
	//setters
	void setSpeed(int12 speed) { m_speed = speed; }
	void setMinSpeed(int12 speed) { m_minSpeed = speed; }
	void setMaxSpeed(int12 speed) { m_maxSpeed = speed; }
	void setClipRectangle(int x, int y, int width, int height);
	inline void setOrigin(int x, int y) { m_xOrigin = x; m_yOrigin = y; }
	inline void setDirection(int dir)   { m_oldDirection = m_direction; m_direction = dir; }
	/** Set the direction according to the position of what the actor is looking at. Note that (x,y) are relative to the actor */
	void setDirection(int x, int y);
	
	/** Return true if the (x,y) point is in the actor clip */
	bool contains(int x, int y);
	virtual void update() {}
	virtual void draw() {}
};

#endif

