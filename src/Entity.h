/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#ifndef _ENTITY_
#define _ENTITY_

#include "Entity.h"
#include "Engine.h"

class Game;

using namespace Tepeyo;

/** 
 * This is the hero: location in the world, behavior, display...
 */
class Entity
{
 protected:
	 Engine * m_engine;
	 Game * m_game;
	 int12 m_x, m_y,
		   m_oldX, m_oldY;
	 int m_width, m_height;

 public:
  Entity(Engine * pEngine, Game * pGame);
  virtual ~Entity();

 public:
  //getters
  inline Engine * getEngine() const { return m_engine; }
  inline int12 getX() const { return m_x; }
  inline int12 getY() const { return m_y; }
  inline int12 getOldX() const { return m_oldX; }
  inline int12 getOldY() const { return m_oldX; }
  inline int getWidth()  const { return m_width; }
  inline int getHeight() const { return m_height; }
  
  //setters
  virtual void setPosition(int12 x, int12 y);
  virtual void setSize(int width, int height);
  bool positionChanged();
};

#endif

