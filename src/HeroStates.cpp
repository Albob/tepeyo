/*
* @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
* Tepeyollotl Engine
* Enjmin 2008
*/

#include "Entity.h"
#include "HeroStates.h"
#include "Level.h"
#include "Engine.h"
#include <nds.h>
#include "BufferedSound.h"
#include "../common/SoundSystem.h"

using namespace Tepeyo;

//-------------------------------------------------------------------------------------------------------------

/** Abstract State of the Hero */
HeroState::HeroState(Hero *parent)
{
	m_parent = parent;
}

HeroState::~HeroState()
{
}

//-------------------------------------------------------------------------------------------------------------

const char * HeroStateIdle::m_filenames[] = {
		"TonatiuhIdleN.spr",
		"TonatiuhIdleNE.spr",
		"TonatiuhIdleE.spr",
		"TonatiuhIdleSE.spr",
		"TonatiuhIdleS.spr",
		"TonatiuhIdleSW.spr",
		"TonatiuhIdleW.spr",
		"TonatiuhIdleNW.spr"
};

/** Concrete Idle State of the Hero */
HeroStateIdle::HeroStateIdle(Hero* parent)
:HeroState(parent)
{
	Engine::log("Created HeroStateIdle");
}

HeroStateIdle::~HeroStateIdle()
{
}

void
HeroStateIdle::enter()
{
	Engine::log("Hero Entering state \"Idle\"");
	m_parent->m_sprite->load(m_filenames[m_parent->m_direction]);

	breathTime = FIRST_BREATH;
	
	if (m_parent->m_gogettherelic)
	  clickOnRelic();
}

void
HeroStateIdle::update()
{
	m_parent->m_sprite->setAnimation(true);

	breathTime--;
	if ( breathTime == 0 ) {
		breathTime = BREATH;
		// Don't play this sound while relic is chasing tonatiuh
		if ( m_parent->m_game->getRelic()->getCurrentState() != m_parent->m_game->getRelic()->getChasingState() ) {
			int snd = BufferedSoundPlay(SND_TONATIUH_RESPIRATION);
			SoundSetVolume(snd, 32);
		}
	}
}

void
HeroStateIdle::leave()
{
	Engine::log("Hero Leaving state \"Idle\"");
}

void
HeroStateIdle::clickOnGround(int x, int y)
{
	m_parent->setTarget(x, y);
	m_parent->setState(m_parent->getMovingState());
}

void
HeroStateIdle::clickOnRelic()
{
	Relic * relic = m_parent->m_game->getRelic();
	int dx = Tepeyo::abs(round12(sub12(m_parent->getX(), relic->getX())));
	int dy = Tepeyo::abs(round12(sub12(m_parent->getY(), relic->getY())));

	// if hero is close enough of the relic, merge with it
	if (relic->onAltar())
	  return;
	
	if (dx < relic->getClipWidth() && dy < relic->getClipHeight())
	  {
		m_parent->m_gogettherelic = false;
		m_parent->setState(m_parent->getMergingState());
	  }
	else
	  {
		m_parent->m_gogettherelic = true;
		clickOnGround(round12(relic->getX()), round12(relic->getY()));
	  }
}


//-------------------------------------------------------------------------------------------------------------

const char * HeroStateMoving::m_filenames[] = {
		"TonatiuhMovingN.spr",
		"TonatiuhMovingNE.spr",
		"TonatiuhMovingE.spr",
		"TonatiuhMovingSE.spr",
		"TonatiuhMovingS.spr",
		"TonatiuhMovingSW.spr",
		"TonatiuhMovingW.spr",
		"TonatiuhMovingNW.spr"
};

/** Concrete Moving State of the Hero */
HeroStateMoving::HeroStateMoving(Hero* parent)
:HeroState(parent)
{
	m_parent->getEngine()->log("Created HeroStateMoving");
}

HeroStateMoving::~HeroStateMoving()
{
  m_parent = NULL;
}

void
HeroStateMoving::enter()
{
	Engine::log("Hero Entering state \"Moving\"");
	m_parent->m_sprite->load(m_filenames[m_parent->m_direction]);  //Sprite oriented to the South
	step = 0;
}

void
HeroStateMoving::update()
{
	m_parent->move();

	step++;
	if ( step > STEPTIME ) {
		step = 0;
		Level * level = m_parent->m_game->getLevel();
		char tile = level->getTileType(round12(m_parent->getX()), round12(m_parent->getY()));
		switch ( tile ) {
			case EARTH:
				BufferedSoundPlay(SND_TONATIUH_TERRE, false, 64);
				break;
			case ROCK:
				BufferedSoundPlay(SND_TONATIUH_CAILLOUX, false, 64);
				break;
			case BRIDGE:
				BufferedSoundPlay(SND_TONATIUH_BOIS);
				break;
		}
	}
}

void
HeroStateMoving::leave()
{
	Engine::log("Hero Leaving state \"Moving\"");
}

void
HeroStateMoving::targetReached()
{
	m_parent->setState(m_parent->getIdleState());
}

void
HeroStateMoving::clickOnGround(int x, int y)
{
  Relic * relic = m_parent->m_game->getRelic();
  
  m_parent->setTarget(x, y);
  if (!relic->contains(x, y))
	m_parent->m_gogettherelic = false;
}

void
HeroStateMoving::stylusReleased()
{
  if (!m_parent->m_gogettherelic)
	m_parent->setState(m_parent->getIdleState());
}

void
HeroStateMoving::fall()
{
  m_parent->setState(m_parent->getFallingState());
}

//-------------------------------------------------------------------------------------------------------------

const char * HeroStateIdleMerged::m_filenames[3][8] = {
	{
		"TonatiuhIdleCarryingN.spr",
		"TonatiuhIdleCarryingN.spr",
		"TonatiuhIdleCarryingN.spr",
		"TonatiuhIdleCarryingS.spr",
		"TonatiuhIdleCarryingS.spr",
		"TonatiuhIdleCarryingS.spr",
		"TonatiuhIdleCarryingS.spr",
		"TonatiuhIdleCarryingN.spr",
	},
	{
		"MergedIdleN.spr",
		"MergedIdleNE.spr",
		"MergedIdleE.spr",
		"MergedIdleSE.spr",
		"MergedIdleS.spr",
		"MergedIdleSW.spr",
		"MergedIdleW.spr",
		"MergedIdleNW.spr"
	},
	{
		"FinalMergedIdle2.spr",
		"FinalMergedIdle2.spr",
		"FinalMergedIdle2.spr",
		"FinalMergedIdle2.spr",
		"FinalMergedIdle2.spr",
		"FinalMergedIdle2.spr",
		"FinalMergedIdle2.spr",
		"FinalMergedIdle2.spr",
	},
};

HeroStateIdleMerged::HeroStateIdleMerged(Hero *parent)
:HeroState(parent)
{
}

HeroStateIdleMerged::~HeroStateIdleMerged()
{
	m_parent = 0;
}

void
HeroStateIdleMerged::enter()
{
	Relic * relic = m_parent->m_game->getRelic();

	Engine::log("Hero entering State \"IdleMerged\"");
	m_parent->m_sprite->load(m_filenames[m_parent->m_currentForm][m_parent->m_direction]);
	if ( m_parent->m_currentForm == Hero::BERSERK )
		m_parent->m_shadow->hide();
	relic->setState(relic->getCarriedState());
	relic->enable(false);
}

void
HeroStateIdleMerged::update()
{
	//compute direction
}

void
HeroStateIdleMerged::leave()
{
	Engine::log("Hero Leaving state \"IdleMerged\"");
}

void
HeroStateIdleMerged::clickOnHero()
{
	//se préparer à jeter la relique
	if ( m_parent->m_currentForm == Hero::MERGED )
		m_parent->setState(m_parent->getUnmergingState());
}

void
HeroStateIdleMerged::clickOnGround(int x, int y)
{
	m_parent->setTarget(x, y);
	m_parent->setState(m_parent->getMovingMergedState());
}

const char * HeroStateIdleMerged::getFilename(int direction) 
{
	return m_filenames[m_parent->m_currentForm][direction];
}

//-------------------------------------------------------------------------------------------------------------

const char * HeroStateMovingMerged::m_filenames[3][8] = {
	{
		"TonatiuhMovingCarryingN.spr",
		"TonatiuhMovingCarryingN.spr",
		"TonatiuhMovingCarryingN.spr",
		"TonatiuhMovingCarryingS.spr",
		"TonatiuhMovingCarryingS.spr",
		"TonatiuhMovingCarryingS.spr",
		"TonatiuhMovingCarryingS.spr",
		"TonatiuhMovingCarryingN.spr",
	},
	{
		"MergedMovingN.spr",
		"MergedMovingNE.spr",
		"MergedMovingE.spr",
		"MergedMovingSE.spr",
		"MergedMovingS.spr",
		"MergedMovingSW.spr",
		"MergedMovingW.spr",
		"MergedMovingNW.spr"
	},
	{
		"FinalMergedMoving2.spr",
		"FinalMergedMoving2.spr",
		"FinalMergedMoving2.spr",
		"FinalMergedMoving2.spr",
		"FinalMergedMoving2.spr",
		"FinalMergedMoving2.spr",
		"FinalMergedMoving2.spr",
		"FinalMergedMoving2.spr",
	},
};

HeroStateMovingMerged::HeroStateMovingMerged(Hero *parent)
:HeroState(parent)
{
}

HeroStateMovingMerged::~HeroStateMovingMerged()
{
	m_parent = 0;
}

void
HeroStateMovingMerged::enter()
{
	Engine::log("Hero entering State \"MovingMerged\"");
	m_parent->m_sprite->load(m_filenames[m_parent->m_currentForm][m_parent->m_direction]);
	if ( m_parent->m_currentForm != Hero::BERSERK ) {
		m_parent->m_game->getRelic()->enable(false);
		step = STEPTIME * 3;
	} else {
		step = STEPTIME;
	}
}

void
HeroStateMovingMerged::update()
{
	if (!m_parent->m_engine->isScreenTouched())
		m_parent->setState(m_parent->getIdleMergedState());

	if ( m_parent->m_currentForm == Hero::BERSERK ) {
		int12 tempy;
		int12 dy = sub12(m_parent->getTarget()->getY(), m_parent->getY());
		int12 dist;

		dist = Tepeyo::distance12(fp12(0), dy);
		dist = min12(dist, fp12(88));
		if (round12(dist) < round12(m_parent->getSpeed())) {
			m_parent->targetReached();
		} else {
			m_parent->setSpeed(div12(mul12(dist, m_parent->getMaxSpeed()), fp12(88)));
			tempy = add12(m_parent->getY(), div12(mul12(dy, m_parent->getSpeed()), dist));
			if ( tempy.value < m_parent->getY().value ) {
				m_parent->setPosition(m_parent->getX(), tempy);

				step--;
				if ( step == 0 ) {
					step = STEPTIME*3;
					BufferedSoundPlay(SND_BERSERK_PAS);
				}
			}
		}
	} else {
		m_parent->move();

		if ( m_parent->m_currentForm == Hero::HUMAN
				&& m_parent->m_game->getLevel()->isAltarActive(m_parent->getX(),
					add12(m_parent->getY(), 0)) ) {
			int altar = m_parent->m_game->getLevel()->findAltar(m_parent->getX(),
					add12(m_parent->getY(), 0));
			if ( altar != -1 ) {
				m_parent->m_game->getLevel()->setAltarState(altar, WAITING);
				m_parent->m_game->getRelic()->setOnAltar(altar);
				m_parent->m_game->getRelic()->setState(m_parent->m_game->getRelic()->getIdleState());
				m_parent->m_game->getRelic()->enable(true);
				m_parent->setState(m_parent->getIdleState());
				m_parent->m_currentForm = Hero::MERGED;
				m_parent->setMaxSpeed(int12(10240));
				BufferedSoundPlay(SND_RELIC_POSEAUTEL);
				Engine::log("MovingMerged::m_currentForm = MERGED [altar %d]", altar);
			}
		}

		step--;
		if ( step == 0 ) {
			step = STEPTIME;
			Level * level = m_parent->m_game->getLevel();
			char tile = level->getTileType(round12(m_parent->getX()), round12(m_parent->getY()));
			switch ( tile ) {
				case EARTH:
					BufferedSoundPlay(SND_TONATIUHFUSION_TERRE, false, 64);
					break;
				case ROCK:
					BufferedSoundPlay(SND_TONATIUHFUSION_CAILLOUX, false, 64);
					break;
				case BRIDGE:
					BufferedSoundPlay(SND_TONATIUHFUSION_BOIS);
					break;
			}
		}
	}
}

void
HeroStateMovingMerged::leave()
{
	Engine::log("Hero Leaving state \"MovingMerged\"");
}

void
HeroStateMovingMerged::clickOnGround(int x, int y)
{
	m_parent->setTarget(x, y);
}

void
HeroStateMovingMerged::targetReached()
{
	m_parent->setState(m_parent->getIdleMergedState());
}

void
HeroStateMovingMerged::fall()
{
  m_parent->setState(m_parent->getFallingMergedState());
}

const char * HeroStateMovingMerged::getFilename(int direction)
{ 
	return m_filenames[m_parent->m_currentForm][direction];
}

//-------------------------------------------------------------------------------------------------------------

const char * HeroStateMerging::m_filenames[] = {
		"TonatiuhMerging.spr"
};

HeroStateMerging::HeroStateMerging(Hero *parent)
:HeroState(parent)
{
}

HeroStateMerging::~HeroStateMerging()
{
	m_parent = 0;
}

void
HeroStateMerging::enter()
{
	Sprite * sprite = m_parent->m_sprite;
	Relic * relic = m_parent->m_game->getRelic();

	Engine::log("Hero Entering State \"Merging\"");
	sprite->load(m_filenames[0]);
	sprite->setFrame(0);
	countdown = sprite->getFrameCount() * sprite->getAnimationSpeed();
	relic->setState(relic->getIdleState());
	relic->enable(false);

	BufferedSoundPlay(SND_TONATIUH_FUSION);
}

void
HeroStateMerging::update()
{
	if (countdown == 0)
		m_parent->setState(m_parent->getIdleMergedState());
	else
		countdown--;
}

void
HeroStateMerging::leave()
{
	Engine::log("Hero Leaving State \"Merging\"");
	m_parent->setDirection(4);
}


//-------------------------------------------------------------------------------------------------------------

const char * HeroStateUnmerging::m_filenames[] = {
		"TonatiuhUnmerging.spr"
};

HeroStateUnmerging::HeroStateUnmerging(Hero *parent)
:HeroState(parent)
{
}

HeroStateUnmerging::~HeroStateUnmerging()
{
	m_parent = 0;
}

void
HeroStateUnmerging::enter()
{
	Sprite * sprite = m_parent->m_sprite;
	Engine::log("Hero Entering State \"Unmerging\"");
	sprite->load(m_filenames[0]);
	sprite->setFrame(0);
	m_totalFrames = sprite->getFrameCount() * sprite->getAnimationSpeed();
	m_countdown = m_totalFrames;
	m_playbackward = false;
	
	BufferedSoundPlay(SND_TONATIUH_DEFUSION);
}

void
HeroStateUnmerging::update()
{
  if (m_countdown == 0)
	{
	  if (!m_playbackward) {
		m_parent->setState(m_parent->getRTTState());
	  }
	  else {
		m_parent->m_sprite->setPlayBackward(false);
		m_parent->setState(m_parent->getIdleMergedState());
	  }
	}
  else
	{
	  m_countdown--;
	}
}

void
HeroStateUnmerging::leave()
{
	Engine::log("Hero Leaving State \"Unmerging\"");
	m_parent->setDirection(4);
	m_parent->m_game->getRelic()->enable(true);
}

void
HeroStateUnmerging::stylusReleased()
{
  m_parent->m_sprite->setPlayBackward(true);
  m_playbackward = true;
  m_countdown = m_totalFrames - m_countdown;
}


//----------------------------------------------------------------------

const char * HeroStateRTT::m_filenames[] = {
		"TonatiuhRTTN.spr",
		"TonatiuhRTTNE.spr",
		"TonatiuhRTTE.spr",
		"TonatiuhRTTSE.spr",
		"TonatiuhRTTS.spr",
		"TonatiuhRTTSW.spr",
		"TonatiuhRTTW.spr",
		"TonatiuhRTTNW.spr"
};

HeroStateRTT::HeroStateRTT(Hero *parent)
:HeroState(parent)
{
}

HeroStateRTT::~HeroStateRTT()
{
	m_parent = 0;
}

void
HeroStateRTT::enter()
{
  Relic* relic = m_parent->m_game->getRelic();
  Engine::log("Entering Hero State \"Ready to throw\"");
  
  m_parent->m_sprite->load(m_filenames[m_parent->getDirection()]);
  relic->setState(relic->getCarriedState());
  relic->enable(true);
}

void
HeroStateRTT::update()
{
  Game * game = m_parent->m_game;
 
  m_stylusX = game->getStylusX();
  m_stylusY = game->getStylusY();
  int dx = m_stylusX - round12(m_parent->getX());
  int dy = m_stylusY - round12(m_parent->getY());

  dy *= -1;
  m_parent->setDirection(dx, dy);
  
  if (m_parent->getOldDirection() != m_parent->getDirection())
    m_parent->m_sprite->load(m_filenames[m_parent->getDirection()]);
    
}

void
HeroStateRTT::leave()
{
  Engine::log("Leaving Hero State \"Ready to throw\"");
}

void
HeroStateRTT::stylusReleased()
{
   Game* game = m_parent->m_game;
   Relic* relic = game->getRelic();

   if (m_parent->contains(m_stylusX, m_stylusY))
	 {
	   m_parent->setState(m_parent->getMergingState());
	 }
   else 
	 {
	   relic->throwAt(m_stylusX, m_stylusY);
	   m_parent->setState(m_parent->getIdleState());
	 }
}

//-------------------------------------------------------------------------------------------------------------

const char * HeroStateFalling::m_filenames[] = {
  "TonatiuhFallingN.spr",
  "TonatiuhFallingE.spr",
  "TonatiuhFallingS.spr",
  "TonatiuhFallingW.spr"
};

HeroStateFalling::HeroStateFalling(Hero *parent)
:HeroState(parent)
{
}

HeroStateFalling::~HeroStateFalling()
{
	m_parent = 0;
}

void
HeroStateFalling::enter()
{
	Sprite * sprite = m_parent->m_sprite;
	Engine::log("Hero Entering State \"Falling\"");
	sprite->load(m_filenames[m_parent->m_direction / 2]);
	sprite->setFrame(0);
	m_countdown = sprite->getFrameCount() * sprite->getAnimationSpeed();

	BufferedSoundPlay(SND_TONATIUH_CHUTE);
}

void
HeroStateFalling::update()
{
  if (m_countdown == 0)
    {
      m_parent->setState(m_parent->getTeleportingState());
    }
  else
    m_countdown--;
}

void
HeroStateFalling::leave()
{
	Engine::log("Hero Leaving State \"Falling\"");
	m_parent->setDirection(4); //face south
}

//-------------------------------------------------------------------------------------------------------------

const char * HeroStateFallingMerged::m_filenames[] = {
  "MergedFallingN.spr",
  "MergedFallingE.spr",
  "MergedFallingS.spr",
  "MergedFallingW.spr"
};

HeroStateFallingMerged::HeroStateFallingMerged(Hero *parent)
:HeroState(parent)
{
}

HeroStateFallingMerged::~HeroStateFallingMerged()
{
	m_parent = 0;
}

void
HeroStateFallingMerged::enter()
{
	Relic * relic = m_parent->m_game->getRelic();
	Sprite * sprite = m_parent->m_sprite;
	int dx = 0;
	int dy = 0;

	Engine::log("Hero Entering State \"Falling Merged\"");
	
	//he drops the relic
	switch(m_parent->getDirection())
	  {
	  case 0: dx = 0; dy = 1; break;
	  case 1: dx = -1; dy = 1; break;
	  case 2: dx = -1; dy = 0; break;
	  case 3: dx = -1; dy = -1; break;
	  case 4: dx = 0; dy = -1; break;
	  case 5: dx = 1; dy = -1; break;
	  case 6: dx = 1; dy = 0; break;
	  case 7: dx = 1; dy = 1; break;
	  }
	relic->setState(relic->getIdleState());
	relic->enable(true);
	relic->setPosition(
		add12(m_parent->m_oldX, fp12(dx * relic->getClipWidth() / 2)),
		add12(m_parent->m_oldY, fp12(dy * relic->getClipWidth() / 2)));

	sprite->load(m_filenames[m_parent->m_direction / 2]);
	sprite->setFrame(0);
	m_countdown = sprite->getFrameCount() * sprite->getAnimationSpeed();

	BufferedSoundPlay(SND_TONATIUH_CHUTE);
}

void
HeroStateFallingMerged::update()
{
  if (m_countdown == 0)
    m_parent->setState(m_parent->getTeleportingState());
  else
    m_countdown--;
}

void
HeroStateFallingMerged::leave()
{
	Engine::log("Hero Leaving State \"Falling Merged\"");
	m_parent->setDirection(4); //face south
}

//-------------------------------------------------------------------------------------------------------------

const char * HeroStateTeleporting::m_filenames[] = {
  "TonatiuhTeleport.spr"
};

HeroStateTeleporting::HeroStateTeleporting(Hero *parent)
:HeroState(parent)
{
}

HeroStateTeleporting::~HeroStateTeleporting()
{
	m_parent = 0;
}

void
HeroStateTeleporting::enter()
{
	Sprite * sprite = m_parent->m_sprite;
	Relic * relic = m_parent->m_game->getRelic();

	Engine::log("Hero Entering State \"Teleporting\"");
	sprite->load(m_filenames[0]);
	sprite->setFrame(0);
	m_countdown = sprite->getFrameCount() * sprite->getAnimationSpeed();
	if ( ! relic->onAltar() ) {
		relic->setPosition(relic->getX(), sub12(relic->getY(), fp12(10)));
		m_parent->setPosition(relic->getX(), add12(relic->getY(), fp12(10)));
	} else {
		m_parent->setPosition(relic->getX(), add12(relic->getY(), fp12(48)));
	}
	relic->setState(relic->getIdleState());
		
	// Sounds good
	BufferedSoundPlay(SND_RELIC_ROTATION);
}
 
void
HeroStateTeleporting::update()
{
  Relic * relic = m_parent->m_game->getRelic();
  if (m_countdown == 0) {
	if ( relic->onAltar() )
	  m_parent->setState(m_parent->getIdleState());
	else
	  m_parent->setState(m_parent->getMergingState());
  } else
    m_countdown--;
}

void
HeroStateTeleporting::leave()
{
	Engine::log("Hero Leaving State \"Falling Merged\"");
	m_parent->setDirection(4); //face south
}

//-----------------------------------------------------------------------------------


const char * HeroStateCaught::m_filenames[] = {
		"RelicHandCatching.spr"
};

HeroStateCaught::HeroStateCaught(Hero *parent)
:HeroState(parent)
{
}

HeroStateCaught::~HeroStateCaught()
{
	m_parent = 0;
}

void
HeroStateCaught::enter()
{
	Sprite * sprite = m_parent->m_sprite;
	Engine::log("Hero Entering State \"Caught\"");
	sprite->load(m_filenames[0]);
	sprite->setFrame(0);
	m_countdown = sprite->getFrameCount() * sprite->getAnimationSpeed();

	BufferedSoundPlay(SND_TONATIUH_TELEPORT);
}

void
HeroStateCaught::update()
{
  if (m_countdown == 0)
	m_parent->setState(m_parent->getTeleportingState());
  else
	m_countdown--;
}

void
HeroStateCaught::leave()
{
	Engine::log("Hero Leaving State \"Caught\"");
	m_parent->setDirection(4);
}

//-----------------------------------------------------------------------------------


const char * HeroStateRTB::m_filenames[] = {
  "TonatiuhRTB.spr",
};

HeroStateRTB::HeroStateRTB(Hero *parent)
:HeroState(parent)
{
}

HeroStateRTB::~HeroStateRTB()
{
	m_parent = 0;
}

void
HeroStateRTB::enter()
{
	Sprite * sprite = m_parent->m_sprite;
	Engine::log("Hero Entering State \"RTB\"");
	sprite->load(m_filenames[0]);
	sprite->setFrame(0);
	m_countdown = sprite->getFrameCount() * sprite->getAnimationSpeed();
}

void
HeroStateRTB::update()
{
  if (m_countdown == 0) {
	m_parent->m_sprite->setAnimation(false);
  }
  else {
	m_countdown--;
  }
}

void
HeroStateRTB::leave()
{
	Engine::log("Hero Leaving State \"RTB\"");
	m_parent->setDirection(4);
}

void
HeroStateRTB::blow()
{
	static int count = 0;
	count++;
	if ( count >= 30 ) {
	  m_parent->setState(m_parent->getBlowingState());
	  count = 0;
	}
}


//-----------------------------------------------------------------------------------


const char * HeroStateBlowing::m_filenames[] = {
		"TonatiuhBlowing.spr"
};

HeroStateBlowing::HeroStateBlowing(Hero *parent)
:HeroState(parent)
{
}

HeroStateBlowing::~HeroStateBlowing()
{
	m_parent = 0;
}

void
HeroStateBlowing::enter()
{
	Sprite * sprite = m_parent->m_sprite;
	Engine::log("Hero Entering State \"Blowing\"");
	sprite->load(m_filenames[0]);
	sprite->setFrame(0);
	sprite->setAnimation(true);
	m_countdown = sprite->getFrameCount() * sprite->getAnimationSpeed();
	m_countdown2 = 7 * 4;
	m_done = false;
}

void
HeroStateBlowing::update()
{
  Level * level = m_parent->m_game->getLevel();

  if (m_countdown == 0) {
	level->activateHorn(m_parent->getX(), m_parent->getY());
	m_parent->setState(m_parent->getTeleportingState());
  }
  else {
	m_countdown--;
  }
}

void
HeroStateBlowing::leave()
{
	Engine::log("Hero Leaving State \"Blowing\"");
	m_parent->setDirection(4);
}

//-----------------------------------------------------------------------------------

HeroStateDie::HeroStateDie(Hero * parent)
	: HeroState(parent)
{
}

HeroStateDie::~HeroStateDie()
{
}

void
HeroStateDie::enter()
{
	Engine::log("Hero Entering State \"DIIIIIIIIIIIIIIIIIEEEEEEEEEEEEEEEEEEEE\"");
	m_meteor = true;
	m_meteorSprite = m_parent->m_engine->getAvailableSprite();
	m_meteorSprite->load("Meteorit.spr");
	m_meteorX = m_parent->m_game->world2camBottomX(round12(m_parent->m_x)) - 32;
	m_meteorY = -64;
	m_meteorSprite->setPosition(m_meteorX, m_meteorY, m_meteorX, m_meteorY-192);
	m_meteorSprite->setAnimation(true);
	BufferedSoundPlay(SND_TONATIUH_METEORITE);
}

void
HeroStateDie::update()
{
	if ( m_meteor ) {
		m_meteorY += 5;
		m_meteorSprite->setPosition(m_meteorX, m_meteorY, m_meteorX, m_meteorY-192);
		if ( m_meteorY > m_parent->m_game->world2camBottomY(round12(m_parent->m_y))+144 ) {
			m_meteorSprite->setPosition(256, 192, 256, 192);
			m_parent->m_sprite->load("Explosion.spr");
			m_parent->m_shadow->hide();
			m_countdown = m_meteorSprite->getFrameCount() * m_meteorSprite->getAnimationSpeed();
			m_meteor = false;
			BufferedSoundPlay(SND_CHOC_METEORIT);
		}
	} else {
		m_countdown--;
		if ( m_countdown == 0 )
			m_parent->m_game->restartLevel();
	}
}

void
HeroStateDie::leave()
{
	Engine::log("Hero Leaving State \"DIIIIIIIIIIIIIIIIIEEEEEEEEEEEEEEEEEEEE\"");
	m_parent->m_engine->releaseSprite(m_meteorSprite);
}

const char *
HeroStateDie::getFilename(int direction)
{
	if ( m_meteor ) {
		return m_parent->m_prevState->getFilename(direction);
	} else
		return "Explosion.spr";
}
