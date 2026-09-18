/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#include "Game.h"
#include "StreamedSound.h"
#include "16cSprite.h"
#include "cutscene.h"
#include "Event.h"

Game::Game()
	: endlevel(false)
{
	touchPos.x = touchPos.y = touchPos.px = touchPos.py = touchPos.z1 = touchPos.z2 = 0;
	m_engine = new Engine();
	m_level = new Level(m_engine, this);
	m_hero = new Hero(m_engine, this);
	m_relic = new Relic(m_engine, this);
	m_topCamera = new Camera(m_engine, this);
	m_bottomCamera = new Camera(m_engine, this);
}

void
Game::init(int level)
{
#if 1
//#ifndef _DEBUG
	if ( level == 0 )
		playCutScene(m_engine, CUTSCENE_INTRO);
	black();
//#endif
#endif

	Sprite16c::load("shadow.bin", "shadow.pal", 7);

	m_level->load(level);
	m_engine->fade(0, 16);
	m_engine->fade(1, 16);

	m_hero->setPosition(fp12(m_level->getPlayerStartX()), fp12(m_level->getPlayerStartY()));
	m_hero->setClipRectangle(18, 20, 26, 44);
	m_hero->setMinSpeed(fp12(1));
	//m_hero->setMaxSpeed(int12(10240)); // 2.5px per frame
	m_hero->setMaxSpeed(int12(5120));
	m_hero->setState(m_hero->getIdleMergedState());

	m_relic->enable(false);
	m_relic->setPosition(fp12(m_level->getRelicStartX()), fp12(m_level->getRelicStartY()));
	m_relic->setClipRectangle(20,28,24,33);
	m_relic->setMinSpeed(fp12(1));
	m_relic->setMaxSpeed(fp12(8));
	m_relic->setPower(m_level->getRelicPower());
	m_relic->setMaxPower(m_level->getRelicPower());
	m_relic->setState(m_relic->getCarriedState());
	
	m_topCamera->lookAt(m_relic);
	m_topCamera->setPosition(fp12(0),fp12(0));
	m_topCamera->setMaxSpeed(int12(7373)); //1.8px perf frame  
	m_topCamera->setSpeed(m_topCamera->getMaxSpeed());
	m_topCamera->setMinSpeed(fp12(1));
	m_bottomCamera->lookAt(m_hero);
	m_bottomCamera->setPosition(fp12(0),fp12(0));
 	m_bottomCamera->setMaxSpeed(int12(7373));  //1.8px per frame
	m_bottomCamera->setSpeed(m_bottomCamera->getMaxSpeed());
	m_bottomCamera->setMinSpeed(fp12(1));

	if ( level != 0 ) {
		m_relic->setMagic();
		m_relic->setState(m_relic->getIdleState());
		m_relic->enable(true);
		m_hero->setMerged();
		m_hero->setState(m_hero->getIdleState());
		m_hero->setMaxSpeed(int12(10240));
	}

	m_level->presentLevel();
}

bool
Game::titleScreen()
{
	bool cheatSuccess = false;

#if 1
//#ifndef _DEBUG
	Engine::log("enters title screen");

	static int cheatcode[] = { KEY_UP, KEY_UP, KEY_DOWN, KEY_DOWN,
								KEY_LEFT, KEY_RIGHT, KEY_LEFT, KEY_RIGHT,
								KEY_A, KEY_B, KEY_START, 0 };
	int * cheat = cheatcode;

	// Prepare screens
	m_hero->setPosition(fp12(300), fp12(200));
	m_relic->setPosition(fp12(300), fp12(200));
	m_topCamera->setPosition(fp12(0), fp12(0));
	m_bottomCamera->setPosition(fp12(0), fp12(192));

	getStreamedSound()->load("/tepeyo/snd/ambiance02.snd");
	getStreamedSound()->setVolume(0);
	getStreamedSound()->fadein();
	getStreamedSound()->play();

	Background * bgTop = m_engine->getBackground(0, 0);
	Background * bgBottom = m_engine->getBackground(0, 1);

	m_engine->setBrightness(0, 0);
	m_engine->setBrightness(1, 0);

	// --- Draw ENJMIN logo ---
	if ( bgTop->load("pix/EnjminCouleur.bg") )
		bgTop->setVisibility(true);

	// Fade in
	m_engine->fade(0, 16);
	while ( m_engine->getBrightness(0) != 16 ) {
		m_engine->waitForDisplay();
		m_engine->update();
	}
	// Wait 3 seconds or a touch
	for ( int i = 0 ; i < 180 && ! m_engine->isScreenTouched() ; i++ ) {
		m_engine->waitForDisplay();
		m_engine->update();
	}
	// Fade out
	m_engine->fade(0, 0);
	while ( m_engine->getBrightness(0) != 0 ) {
		m_engine->waitForDisplay();
		m_engine->update();
	}

	// --- Draw tepeyollotl logo ---
	bgTop->load("pix/EcranTitreAltTop.bg", true);
	if ( bgBottom->load("pix/EcranTitreAltBottom.bg") )
		bgBottom->setVisibility(true);

	// Fade in
	m_engine->fade(0, 16);
	while ( m_engine->getBrightness(0) != 16 ) {
		m_engine->waitForDisplay();
		m_engine->update();
	}
	// Wait for a screen touch
	while ( ! m_engine->isScreenTouched() && ! cheatSuccess )
	{
		m_engine->waitForDisplay();
		m_engine->update();

		if ( keysDown() & (*cheat) )
			cheat++;
		if ( *cheat == 0 )
			cheatSuccess = true;
	}
	// Fade out
	m_engine->fade(0, 0);
	m_engine->fade(1, 0);
	getStreamedSound()->fadeout();
	while ( m_engine->getBrightness(0) != 0 ) {
		m_engine->waitForDisplay();
		m_engine->update();
	}

	// Wait a second
	black();

	Engine::log("exits title screen");
//#endif
#endif

	return cheatSuccess;
}

void
Game::run()
{
  	bool cheat = titleScreen();
	init(cheat ? 4 : 0);
	Engine::log("\n**Game Beginning!!");

	while(true)
	{
		//Process Inputs
		touchPosition tmp = m_engine->getTouchPosition();
		if ( tmp.x != 0 && tmp.y != 0 ) {
			touchPos = tmp;
			touchScreen2world(touchPos);
		}
		if (m_engine->isScreenClicked())
		{
		  if (m_relic->contains(touchPos.px, touchPos.py)) {
			m_hero->clickOnRelic();
		  } 
		  if (m_hero->contains(touchPos.px, touchPos.py)) {
			m_hero->clickOnHero();
		  }
		}
		else if (m_engine->isScreenTouched())
		{
			m_hero->clickOnGround(touchPos.px, touchPos.py);
		}
		else if (m_engine->isScreenReleased())
		{
			m_hero->stylusReleased();
		}
		else if (m_engine->isBlowing())
		{
		  m_hero->blow();
		}
		m_hero->update();
		m_relic->update();

		//update cameras
		if (!m_relic->isEnabled() || m_bottomCamera->contains(round12(m_relic->getX()), round12(m_relic->getY())))
			m_topCamera->lookAt(0);
		else
			m_topCamera->lookAt(m_relic);
		m_topCamera->update();
		m_bottomCamera->update();

		// If last level is finished and faded out, go to next level
		if ( endlevel && m_engine->getBrightness(0) == 0 ) {
			black();
			m_level->nextLevel();
			m_engine->fade(0, 16);
			m_engine->fade(1, 16);
			endlevel = false;

			m_hero->setPosition(
								fp12(m_level->getPlayerStartX()), 
								fp12(m_level->getPlayerStartY()));
			m_relic->setPosition(
								 fp12(m_level->getRelicStartX()), 
								 fp12(m_level->getRelicStartY()));
			m_relic->setPower(m_level->getRelicPower());
			m_relic->setMaxPower(m_level->getRelicPower());
		}

		//update level ( update backgrounds )
		m_level->update();

		//wait for vblank
		m_engine->waitForDisplay();

		//stream sprites, BGs & sounds, render on PC, etc
		m_engine->update();
	}
}

void
Game::touchScreen2world(touchPosition &tp)
{
  tp.px = tp.px + round12(m_bottomCamera->getX());
  tp.py = tp.py + round12(m_bottomCamera->getY());
}

void
Game::endLevel()
{
  m_engine->fade(0, 0);
  m_engine->fade(1, 0);
  getStreamedSound()->fadeout();
  endlevel = true;
}

void
Game::restartLevel()
{
	// Fade to black
	m_engine->fade(0, 0);
	m_engine->fade(1, 0);
	//getStreamedSound()->fadeout();
	while ( m_engine->getBrightness(0) != 0 ) {
		m_engine->waitForDisplay();
		m_engine->update();
	}

	// Reset level
	m_engine->fade(0, 16);
	m_engine->fade(1, 16);
	//m_level->restartLevel();
	m_hero->setPosition(
						fp12(m_level->getPlayerStartX()), 
						fp12(m_level->getPlayerStartY()));
	m_hero->setState(m_hero->getIdleState());
	m_relic->enable(true);
	m_relic->setPosition(
						 fp12(m_level->getRelicStartX()), 
						 fp12(m_level->getRelicStartY()));
	m_relic->setPower(m_level->getRelicPower());
	m_relic->setMaxPower(m_level->getRelicPower());
	m_relic->setState(m_relic->getIdleState());
	m_level->resetAltarsAndDoor();
	m_level->presentLevel();
}

void
Game::gameOver()
{
	m_relic->setState(m_relic->getIdleState());
	if ( m_hero->getCurrentState() == m_hero->getMergingState()
			|| m_hero->getCurrentState() == m_hero->getMovingMergedState()
			|| m_hero->getCurrentState() == m_hero->getIdleMergedState()
			|| m_hero->getCurrentState() == m_hero->getFallingMergedState() )
		m_hero->setState(m_hero->getIdleMergedState());
	else
		m_hero->setState(m_hero->getIdleState());
	m_hero->setState(m_hero->getDieState());
}

void
Game::black()
{
	m_engine->setBrightness(0, 0);
	m_engine->setBrightness(1, 0);
	for ( int i = 0 ; i < 60 ; i++ )
		m_engine->waitForDisplay();
}
