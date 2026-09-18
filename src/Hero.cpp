/*
* @author: Alexis Bauchu <alexis.bauchu@gmail.com>
* Tepeyollotl Engine
* Enjmin 2008
*/

#include "Entity.h"
#include "Hero.h"
#include "BufferedSound.h"

using namespace Tepeyo;

Hero::Hero(Engine * pEngine, Game * pGame)
:Actor(pEngine, pGame), m_target(0), m_currentState(0), m_prevState(0), m_visible(true), m_gogettherelic(0)
{
	m_sprite = pEngine->getAvailableSprite();
	m_sprite->setAnimation(true);
	m_shadow = pEngine->getAvailableSprite16c();
	m_shadow->setAlpha(true);
	m_shadow->setSprite(4);
	m_destination = new Entity(pEngine, pGame);
	m_idleState = new HeroStateIdle(this);
	m_movingState = new HeroStateMoving(this);
	m_idleMergedState = new HeroStateIdleMerged(this);
	m_movingMergedState = new HeroStateMovingMerged(this);
	m_mergingState = new HeroStateMerging(this);
	m_unmergingState = new HeroStateUnmerging(this);
	m_RTTState = new HeroStateRTT(this);
	m_fallingState = new HeroStateFalling(this);
	m_fallingMergedState = new HeroStateFallingMerged(this);
	m_teleportingState = new HeroStateTeleporting(this);
	m_caughtState = new HeroStateCaught(this);
	m_RTBState = new HeroStateRTB(this);
	m_blowingState = new HeroStateBlowing(this);
	m_dieState = new HeroStateDie(this);
	setState(m_idleMergedState);
	m_currentForm = HUMAN;
	m_xOrigin = 32;
	m_yOrigin = 64 - 10;
	Engine::log("Hero and HeroStates created");
}

/** WTF?? On NDS?? geez...*/
Hero::~Hero()
{
	if (m_target)
		delete m_target;
	if (m_idleState)
		delete m_idleState;
	if (m_movingState)
		delete m_movingState;
	if (m_idleMergedState)
		delete m_idleMergedState;
	if (m_movingMergedState)
		delete m_movingMergedState;
	if (m_mergingState)
		delete m_mergingState;
	if (m_unmergingState)
		delete m_unmergingState;
	if (m_RTTState)
		delete m_RTTState;
	if (m_fallingState)
		delete m_fallingState;
	if (m_fallingMergedState)
		delete m_fallingMergedState;
	if (m_teleportingState)
		delete m_teleportingState;
	if (m_caughtState)
		delete m_caughtState;
	if (m_dieState)
	  delete m_dieState;
}

void
Hero::clickOnHero()
{
	Engine::log("clickOnHero");
	m_currentState->clickOnHero();
}

void
Hero::clickOnRelic()
{
	Engine::log("clickOnRelic");
	m_currentState->clickOnRelic();
}

void
Hero::clickOnGround(int x, int y)
{
	//Engine::log("clickOnGround(%d,%d)", x, y);
	m_currentState->clickOnGround(x, y);
}

void
Hero::targetReached()
{
	Engine::log("targetReached");
	m_currentState->targetReached();
}

void
Hero::stylusReleased()
{
	Engine::log("stylusReleased");
	m_currentState->stylusReleased();
}

void
Hero::blow()
{
  //Engine::log("blow");
	m_currentState->blow();
}

void
Hero::fall()
{
  Engine::log("Hero::fall");
  m_currentState->fall();
}

void
Hero::setState(HeroState *state)
{
	if (m_currentState)
		m_currentState->leave();
	m_prevState = m_currentState;
	m_currentState = state;
	m_currentState->enter();
}

void
Hero::setTarget(int x, int y)
{
	m_destination->setPosition(fp12(x), fp12(y));
	m_target = m_destination;
}

void
Hero::setTarget(Entity* target)
{
	m_target = target;
}

void
Hero::setPosition(int12 x, int12 y)
{
  Level * level = m_game->getLevel();
  if ( m_visible ) {
	  if (round12(x) > 0 && round12(x) < level->getWidth() &&
		  round12(y) > 0 && round12(y) < level->getHeight())
		{
		  Entity::setPosition(x, y);
		  m_sprite->setPosition(
								m_game->world2camTopX(round12(m_x)) - m_xOrigin,    m_game->world2camTopY(round12(m_y)) - m_yOrigin,
								m_game->world2camBottomX(round12(m_x)) - m_xOrigin, m_game->world2camBottomY(round12(m_y)) - m_yOrigin);
		  m_shadow->show();
		  m_shadow->setPosition(
			m_game->world2camTopX(round12(m_x)) - m_xOrigin + 16,    m_game->world2camTopY(round12(m_y)) - m_yOrigin + 40,
			m_game->world2camBottomX(round12(m_x)) - m_xOrigin + 16, m_game->world2camBottomY(round12(m_y)) - m_yOrigin + 40);
		}
  } else {
	  m_sprite->setPosition(256, 192, 256, 192);
	  m_shadow->hide();
  }
}

void
Hero::update()
{
	m_currentState->update();
	this->setPosition(m_x, m_y);
}


void
Hero::move()
{
	int12 tempx, tempy;
	int12 dx = sub12(getTarget()->getX(), getX());
	int12 dy = sub12(getTarget()->getY(), getY());
	int12 dist;

	dist = Tepeyo::distance12(dx, dy);
	dist = min12(dist, fp12(88));
	if (round12(dist) < round12(getSpeed()))
	{
		targetReached();
		return;
	}
	setSpeed(div12(mul12(dist, getMaxSpeed()), fp12(88)));
	tempx = add12(getX(), div12(mul12(dx, getSpeed()), dist));
	tempy = add12(getY(), div12(mul12(dy, getSpeed()), dist));

	computeCollisions(tempx, tempy);
	setPosition(tempx, tempy);
	
	/* What direction is facing Tonatiuh? */
	dy = mul12(dy, fp12(-1));
	setDirection(round12(dx), round12(dy));

	if (m_direction != m_oldDirection)
	{
		this->m_sprite->load(m_currentState->getFilename(m_direction));
	}
}

void
Hero::computeCollisions(int12 &x, int12 &y)
{
	Level * level = this->m_game->getLevel();
	Relic * relic = this->m_game->getRelic();
	char tileType;
	int hornId, altarID;

	if (relic->isEnabled() && relic->contains(round12(x), round12(y)))
	{
	  if (m_gogettherelic)
		setState(getMergingState());
	  else {
		x = m_x;
		y = m_y;
	  }
	}
	else {
		tileType = level->getTileType(round12(x), round12(y));
		switch(tileType)
		{
		case WATER:
		  x = m_x;
		  y = m_y;
		  break;
		  
		case WALL:
		case ALTAR:
		  x = m_x;
		  y = m_y;
		  hornId = level->findHorn(x, y);
		  if (hornId != -1 && level->isAltarWaiting(hornId))
			{
			  x = fp12(level->getHornX(hornId) - 16);
			  y = fp12(level->getHornY(hornId) - 8);
			  setState(getRTBState());
			}
		  altarID = level->findAltar(x, y);
		  if (altarID != -1 && level->isAltarActive(x, y)  && 
			  getCurrentState() == getMovingMergedState())
			{
			  relic->enable(true);
			  relic->setOnAltar(altarID);
			  relic->setState(relic->getIdleState());
			  relic->setPosition(fp12(level->getAltarX(0) + 32), fp12(level->getAltarY(0)));
			  level->setAltarState(altarID, WAITING);
			  BufferedSoundPlay(SND_RELIC_POSEAUTEL);
			  if ( level->isAltarSpecial(altarID) )
				level->playFinal();
			  this->setState(this->getIdleState());
			}
		  break;

		case VOID:
			fall();
			break;
		}
	}
}

