/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#include "Entity.h"

Entity::Entity(Engine * pEngine, Game * pGame)
{
  m_engine = pEngine;
  m_game = pGame;
  m_x = fp12(0);
  m_y = fp12(0);
  m_oldY = fp12(0);
  m_oldX = fp12(0);
  m_height = m_width = 0;
}

Entity::~Entity()
{
}

void
Entity::setPosition(int12 x, int12 y)
{
	m_oldX = m_x;
	m_oldY = m_y;
	m_x = x;
	m_y = y;
}

void
Entity::setSize(int width, int height)
{
	m_width = width;
	m_height = height;
}

bool
Entity::positionChanged()
{
  return ( round12(m_oldX) != round12(m_x) && round12(m_oldY) != round12(m_y));
}

