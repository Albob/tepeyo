/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * Tepeyollotl Engine
 * Enjmin 2008
 */ 
 
#include "RelicStates.h" 
#include "Hero.h"
#include "HeroStates.h"
#include "BufferedSound.h"
#include "../common/SoundSystem.h"
#include "Level.h"
#include "AltarsANDdoors.h"

using namespace Tepeyo;
 
RelicState::RelicState(Relic * pRelic) 
{ 
	m_parent = pRelic; 
} 
 
RelicState::~RelicState(void) 
{ 
} 
 
 
//------------------------------------------------------------------------------------------------------------- 
 
const char * RelicIdleState::m_filenames[] = {
		"RelicIdleN.spr",
		"RelicIdleNE.spr",
		"RelicIdleE.spr",
		"RelicIdleSE.spr",
		"RelicIdleS.spr",
		"RelicIdleSW.spr",
		"RelicIdleW.spr",
		"RelicIdleNW.spr"
}; 

const char * RelicIdleState::m_waitingFilename = {
  "RelicAltar.spr"
};

const char * RelicIdleState::m_chargingFilename = {
  "RelicAltarCharging.spr"
};
 
RelicIdleState::RelicIdleState(Relic* pRelic) 
:RelicState(pRelic) 
{
}

RelicIdleState::~RelicIdleState() 
{
}

void 
RelicIdleState::enter()
{
	Engine::log("Entering the Relic Idle state"); 
	if (m_parent->m_onAltar != -1) {
	  m_parent->m_sprite->load(m_waitingFilename);
	} 
	else if (m_parent->m_charging) {
	  m_parent->m_sprite->load(m_chargingFilename);
	} 
	else {
	  m_parent->m_sprite->load(m_filenames[m_parent->m_direction]); 
	}
	m_parent->setAltitude(fp12(0));
}

void 
RelicIdleState::update() 
{
	Hero * hero = m_parent->m_game->getHero();
	int12 dx = sub12(hero->getX(), m_parent->getX());
	int12 dy = sub12(hero->getY(), m_parent->getY());
	int12 dist = Tepeyo::distance12(dx, dy);

	/* What direction is facing the relic? */
	if (m_parent->m_onAltar != -1) {
	  m_parent->m_sprite->load(m_waitingFilename);
	} 
	else if (m_parent->m_charging) {
	  m_parent->m_sprite->load(m_chargingFilename);
	} 
	else {
	  m_parent->setDirection(round12(dx), -round12(dy));
	  if (m_parent->getOldDirection() != m_parent->getDirection()) 
		{
		  m_parent->m_sprite->load(m_filenames[m_parent->getDirection()]);
		  
		  int volume = round12(sub12(fp12(127), div12(dist, fp12(4))));
		  if ( volume < 0 )
			volume = 0;
		  int panning = round12(dx) - 64;
		  if ( panning < 0 )
			panning = 0;
		  if ( panning > 127 )
			panning = 127;
		  panning = 127 - panning;
		  int snd = BufferedSoundPlay(SND_RELIC_ROTATION, false, volume, panning);
		  if ( snd != NOCHANNEL )
			SoundSetVolume(snd, 48);
		}
	}
	
	if ( m_parent->m_onAltar != -1 ) {
		if ( ! m_parent->m_game->getLevel()->isAltarWaiting(m_parent->m_onAltar) ) {
			m_parent->m_onAltar = -1;
			m_parent->m_sprite->setPriority(128);
			m_parent->m_flameSprite->setVisibility(true);
			if ( m_parent->m_currentForm == Relic::INERT ) {
				m_parent->m_currentForm = Relic::MAGIC;
				m_parent->enable(false);
			}
		} else {
			m_parent->m_x = fp12(m_parent->m_game->getLevel()->getAltarX(m_parent->m_onAltar) + 32);
			m_parent->m_y = fp12(m_parent->m_game->getLevel()->getAltarY(m_parent->m_onAltar) - 32);
			m_parent->m_sprite->setPriority(64);
		}
	}

	HeroState* state = hero->getCurrentState();
	if (round12(dist) > 178 &&
		m_parent->m_currentForm == Relic::MAGIC &&
		state != hero->getDieState() && 
		state != hero->getCaughtState() &&
		state != hero->getMergingState() && 
		state != hero->getRTBState() &&
		state != hero->getBlowingState())
	  m_parent->setState(m_parent->getChasingState());
}


//------------------------------------------------------------------------------------------------------------- 

const char * RelicCarriedState::m_filenames[] = {
		"RelicIdleN.spr",
		"RelicIdleNE.spr",
		"RelicIdleE.spr",
		"RelicIdleSE.spr",
		"RelicIdleS.spr",
		"RelicIdleSW.spr",
		"RelicIdleW.spr",
		"RelicIdleNW.spr"
}; 
 
RelicCarriedState::RelicCarriedState(Relic* pRelic) 
:RelicState(pRelic) 
{ 
} 
 
RelicCarriedState::~RelicCarriedState() 
{ 
} 
 
void 
RelicCarriedState::enter() 
{ 
	Engine::log("Entering the Relic Carried state"); 
	if ( m_parent->m_currentForm == Relic::BERSERK )
		m_parent->m_sprite->load("FinalMergedIdle1.spr");
	else
		m_parent->m_sprite->load(m_filenames[m_parent->m_direction]); 
	m_parent->m_sprite->setPriority(64);
	m_lastState = 0;
} 
 
void 
RelicCarriedState::update() 
{	
	Hero * hero = m_parent->m_game->getHero();

	//todo : setPriorita(FOREGROUNDO);
	if ( m_parent->m_currentForm == Relic::BERSERK ) {
		m_parent->setPosition(hero->getX(), sub12(hero->getY(), fp12(64)));
		m_parent->setAltitude(fp12(0));
		if ( hero->getCurrentState() != m_lastState ) {
			if ( hero->getCurrentState() == hero->getIdleMergedState() )
				m_parent->m_sprite->load("FinalMergedIdle1.spr");
			else
				m_parent->m_sprite->load("FinalMergedMoving1.spr");
			m_lastState = hero->getCurrentState();
		}
	} else {
		m_parent->setPosition(hero->getX(), hero->getY());
		m_parent->setAltitude(fp12(30));
		m_parent->setDirection(hero->getDirection());
		if (m_parent->getOldDirection() != m_parent->getDirection()) 
		{
			m_parent->m_sprite->load(m_filenames[m_parent->getDirection()]); 
		}
	}
}

void
RelicCarriedState::leave()
{
	Engine::log("Leaving the Relic Carried state"); 
	m_parent->m_sprite->setPriority(128);
}

//------------------------------------------------------------------------------------------------------------- 

const char * RelicChasingState::m_filenames[] = {
		"RelicIdleN.spr",
		"RelicIdleNE.spr",
		"RelicIdleE.spr",
		"RelicIdleSE.spr",
		"RelicIdleS.spr",
		"RelicIdleSW.spr",
		"RelicIdleW.spr",
		"RelicIdleNW.spr"
}; 

const char * RelicChasingState::m_handfilenames[] = {
		"RelicHandN.spr",
		"RelicHandE.spr",
		"RelicHandS.spr",
		"RelicHandW.spr"
};

const char * RelicChasingState::m_chargingFilename = {
  "RelicAltar.spr"
};
 
RelicChasingState::RelicChasingState(Relic* pRelic) 
:RelicState(pRelic) 
{
	m_handSprite = m_parent->m_engine->getAvailableSprite();
	m_handSprite->setAnimation(true);
	m_handX = fp12(256);   // set the sprite
	m_handY = fp12(192);   // off screen
	m_handXOrigin = 32;
	m_handYOrigin = 32;
	m_handDirection = m_handOldDirection = 0;
	m_handSpeed.value = 9216;  // 2.25 << 12
} 
 
RelicChasingState::~RelicChasingState() 
{
	m_parent->m_engine->releaseSprite(m_handSprite);
	m_handSprite = 0;
}
 
void 
RelicChasingState::enter() 
{ 
	Engine::log("Entering the Relic Chasing state");
	m_handX = m_parent->getX();
	m_handY = add12(m_parent->getY(), fp12(1));
	m_handOldDirection = m_handDirection;
	m_handDirection = m_parent->getDirection() / 2;
	m_handSprite->load(m_handfilenames[m_handDirection]); // careful, the hand only has 4 directions!
	if (m_parent->m_onAltar != -1) {
	  m_parent->m_sprite->load(m_chargingFilename);
	} else {
	  m_parent->m_sprite->load(m_filenames[m_parent->m_direction]);
	}

	chasingSound = BufferedSoundPlay(SND_RELIC_HAND, SND_LOOP);
} 
 
void 
RelicChasingState::update()
{
	Game * game = m_parent->m_game;
	Hero * hero = game->getHero();
	int12 dx = sub12(hero->getX(), m_parent->getX());
	int12 dy = sub12(hero->getY(), m_parent->getY());
	int12 dist;
	int diffx, diffy;

	if (hero->getCurrentState() == hero->getRTBState())
	  {
		m_parent->setState(m_parent->getIdleState());
		return;
	  }

	/* What direction is facing the relic? */
	m_parent->setDirection(round12(dx), -round12(dy));

	if ( m_parent->m_onAltar == -1 ) {
		if (m_parent->getOldDirection() != m_parent->getDirection()) 
			m_parent->m_sprite->load(m_filenames[m_parent->getDirection()]); 
	} else {
	  m_parent->m_sprite->load(m_chargingFilename);
	}

	/* about the hand... */
	dx = sub12(hero->getX(), m_handX);
	dy = sub12(hero->getY(), m_handY);
	dist = Tepeyo::distance12(dx, dy);

	/* where to display the hand */
	m_handX = add12(m_handX, div12(mul12(dx, m_handSpeed), dist));
	m_handY = add12(m_handY, div12(mul12(dy, m_handSpeed), dist));
	m_handSprite->setPosition(
		game->world2camTopX(round12(m_handX)) - m_handXOrigin, game->world2camTopY(round12(m_handY)) - m_handYOrigin,
		game->world2camBottomX(round12(m_handX)) - m_handXOrigin, game->world2camBottomY(round12(m_handY)) - m_handYOrigin);

	/* what direction is "facing" the hand */
	m_handOldDirection = m_handDirection;	
	diffx = round12(dx);
	diffy = -round12(dy);
	if (diffx < diffy)
	{
		if (diffx < -diffy)
			m_handDirection = 3;
		else 
			m_handDirection = 0;
	}
	else
	{
		if (diffx < -diffy)
			m_handDirection = 2;
		else
			m_handDirection = 1;
	}

	if (m_handOldDirection != m_handDirection)
		m_handSprite->load(m_handfilenames[m_handDirection]);

	if (round12(dist) < hero->getClipWidth())
	{
		m_parent->setState(m_parent->getIdleState());
		hero->setState(hero->getCaughtState());
	}

	diffx = -diffx + 64;
	if ( diffx < 0 )
		diffx = 0;
	if ( diffx > 127 )
		diffx = 127;
	SoundSetPanning(chasingSound, diffx);
	int volume = round12(sub12(fp12(127), div12(dist, fp12(4))));
	if ( volume < 0 )
		volume = 0;
	SoundSetVolume(chasingSound, volume);
}

void
RelicChasingState::leave()
{
	Engine::log("Leaving Relic State \"Chasing\"");
	m_handSprite->setPosition(256, 192, 256, 192); //set the sprite offscreen

	BufferedSoundStop(chasingSound);
}
 
 
//------------------------------------------------------------------------------------------------------------- 

const char * RelicFlyingState::m_filenames[] = {
		"RelicIdleN.spr",
		"RelicIdleNE.spr",
		"RelicIdleE.spr",
		"RelicIdleSE.spr",
		"RelicIdleS.spr",
		"RelicIdleSW.spr",
		"RelicIdleW.spr",
		"RelicIdleNW.spr"
}; 

const int12 RelicFlyingState::gravity  = int12 (2048);  // 0.5 px/frame
const int12 RelicFlyingState::friction = int12(1229);   // 0.3 px/frame

 
RelicFlyingState::RelicFlyingState(Relic* pRelic) 
:RelicState(pRelic) 
{
}

RelicFlyingState::~RelicFlyingState() 
{
}

void 
RelicFlyingState::enter()
{
	Engine::log("Entering the Relic Flying state"); 
	m_parent->m_sprite->load(m_filenames[m_parent->m_direction]);
	m_parent->setAltitude(fp12(30));
	touchGround = false;
	autochase = false;
	slideSound = NOCHANNEL;
	BufferedSoundPlay(SND_RELIC_JETTE);
}

void 
RelicFlyingState::update() 
{
	Game * game = m_parent->m_game;
	Hero * hero = game->getHero();
	Level * level = game->getLevel();
	int12 dx = sub12(hero->getX(), m_parent->getX());
	int12 dy = sub12(hero->getY(), m_parent->getY());
	int12 tempx, tempy;
	int tileType;

	/* What direction is facing the relic? */
	m_parent->setDirection(round12(dx), -round12(dy));

	if (m_parent->getOldDirection() != m_parent->getDirection()) 
	{
		m_parent->m_sprite->load(m_filenames[m_parent->getDirection()]); 
	}

	m_parent->m_zspeed = sub12(m_parent->m_zspeed, gravity);
	tempx = add12(m_parent->getX(), mul12(m_parent->m_dirX, m_parent->getSpeed()));
	tempy = add12(m_parent->getY(), mul12(m_parent->m_dirY, m_parent->getSpeed()));

	/* collisions when in the air */
	tileType = game->getLevel()->getTileType(round12(tempx), round12(tempy));
	switch (tileType)
	  {
	  case WALL:
		m_parent->setSpeed(fp12(0));
		break;

	  case ALTAR:
		m_parent->setSpeed(fp12(0));
		if (level->isAltarActive(tempx, tempy)) {
		  m_parent->m_onAltar = level->findAltar(tempx, tempy);
		  level->setAltarState(m_parent->m_onAltar, WAITING);
		  m_parent->setState(m_parent->getIdleState());
		  BufferedSoundPlay(SND_RELIC_POSEAUTEL);
		  if ( level->isAltarSpecial(m_parent->m_onAltar) )
			  level->playFinal();
		  return;
		}
		break;

	  default:
		m_parent->setPosition(tempx, tempy);	
	}

	if (tileType == VOID || tileType == WATER) {
	  autochase = true;
	}
	
	m_parent->setAltitude(add12(m_parent->getAltitude(), m_parent->m_zspeed));
	
	/* collisions when on the ground */
	if (round12(m_parent->getAltitude()) <= 0)
	{
		m_parent->setAltitude(fp12(0));
		switch (tileType)
		  {
		  case VOID:
		  case WATER:
			m_parent->setPosition(hero->getX(), hero->getY());
			hero->setState(hero->getMergingState());
			return;

		  default:
			m_parent->setSpeed(sub12(m_parent->getSpeed(), friction));
		  }

		if ( ! touchGround ) {
			slideSound = BufferedSoundPlay(SND_RELIC_GLISSE);
			BufferedSoundPlay(SND_RELIC_ATTERIT);
			touchGround = true;
		}

		if (round12(m_parent->getSpeed()) <= 0)
		{
		  if (autochase) {
			m_parent->setState(m_parent->getChasingState());
		  }
		  else if (m_parent->contains(round12(hero->getX()), round12(hero->getY()))) {
			hero->setState(hero->getMergingState());
		  }
		  else {
			m_parent->setState(m_parent->getIdleState());
		  }
		}
	}
}

void
RelicFlyingState::leave()
{
	Engine::log("Leaving the Relic Flying state");
	BufferedSoundStop(slideSound);
}

//------------------------------------------------------------------------------------------------------------- 
 

