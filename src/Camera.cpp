/*
* @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
* Tepeyollotl Engine
* Enjmin 2008
*/

#include "Camera.h"
#include "Level.h"
#include "Game.h"

Camera::Camera(Engine * engine, Game* game)
:Actor(engine, game)
{
	setClipRectangle(0,0,256,192);  //screen size of the NDS
	setOrigin(0,0);
	Engine::log("created Camera");
}

Camera::~Camera()
{
}

void
Camera::lookAt(Actor *target)
{
	m_target = target;
}

void
Camera::update()
{
	Level * lvl = m_game->getLevel();
	HeroState * currentState = ((Hero*)m_target)->getCurrentState();
	int12 tempx, tempy;
	int12 stylusx, stylusy;
	int12 dx, dy;

	if (m_target) {
	  if (this == m_game->getBottomCamera() && m_engine->isScreenTouched() &&
		  currentState == ((Hero*)m_target)->getRTTState())
		{
		  stylusx = sub12(fp12(m_game->getStylusX()), getX());
		  stylusy = sub12(fp12(m_game->getStylusY()), getY());
		  dx = sub12(stylusx, fp12(256 / 2));
		  dy = sub12(stylusy, fp12(192 / 2));

		  //setSpeed(getMaxSpeed());
		  tempx = add12(m_target->getX(), dx);
		  tempy = add12(m_target->getY(), dy);
		}
	  else {
		//setSpeed(fp12(-1));
		tempx = m_target->getX();
		tempy = m_target->getY();
	  }
	  moveTo(tempx, tempy);
	}
	else {
	  setPosition(
				  m_game->getBottomCamera()->getX(),
				  sub12(m_game->getBottomCamera()->getY(), fp12(192 + 50)));
	}
	
	//keep the camera in the level
	if (round12(m_x) < 0)
		m_x = fp12(0);
	if (round12(m_x) > lvl->getWidth() - 256)
		m_x = fp12(lvl->getWidth() - 256);
	if (round12(m_y) < 0)
		m_y = fp12(0);
	if (round12(m_y) > lvl->getHeight() - 192)
		m_y = fp12(lvl->getHeight() - 192);
}

void
Camera::moveTo(int12 x, int12 y)
{
  //int12 posx, posy;

  /*if (val12(getSpeed()) == -1) {
	posx = x;
	posy = y;
  }
  else {
	posx = x;
	posy = y;
  }*/
  
  //int shift = (m_target)? m_target->getClipHeight() / 2 : 0;
  int shift = 8;
  setPosition(
			  sub12(x, fp12(256 / 2)),
			  sub12(y, fp12(shift + 192 / 2)));
}
