/*
* @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
* Tepeyollotl Engine
* Enjmin 2008
*/

#include "Actor.h"

Actor::Actor(Engine * engine, Game * game)
  :Entity(engine, game), m_speed(fp12(0)), m_maxSpeed(fp12(0)), m_minSpeed(fp12(0)),
   m_clipX(0), m_clipY(0), m_clipWidth(64), m_clipHeight(64),
   m_xOrigin(32), m_yOrigin(32),
   m_direction(0), m_oldDirection(0)
{
}

Actor::~Actor()
{
}

void
Actor::setClipRectangle(int x, int y, int width, int height)
{
	m_clipX = x;
	m_clipY = y;
	m_clipWidth = width;
	m_clipHeight = height;
}

bool
Actor::contains(int x, int y)
{
	int X = round12(m_x) + m_clipX - m_xOrigin;
	int Y = round12(m_y) + m_clipY - m_yOrigin;
	return ((x > X) && (x < X + m_clipWidth) && (y > Y) && (y < Y + m_clipHeight));
}

void
Actor::setDirection(int dx, int dy)
{
	if (dy > 2*dx) 	{
		if (dy > -dx/2) {
			if (dy > -2*dx)
				this->setDirection(0);
			else
				this->setDirection(7);
		}
		else { // dy < -0.5*dx 
			if (dy > dx/2)
				this->setDirection(6);
			else
				this->setDirection(5);
		}
	}
	else { // dy < 2*dx
		if (dy > -dx/2) {
			if (dy > dx/2)
				this->setDirection(1);
			else
				this->setDirection(2);
		}
		else // dy < -0.5*dx
		{
			if (dy > -2*dx)
				this->setDirection(3);
			else
				this->setDirection(4);
		}
	}
}


