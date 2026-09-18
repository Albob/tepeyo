/*
* @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
* Tepeyollotl Engine
* Enjmin 2008
*/

#include "Level.h"
#include "Camera.h"
#include <string.h>
#include <stdio.h>
#include "StreamedSound.h"
#include "../common/SoundSystem.h"
#include "BufferedSound.h"
#include "mt19937.h"
#include "cutscene.h"
#include "Vector2.h"

using namespace Tepeyo;

const char *
Level::m_levelFiles[] = {
  "Level0.lvl",
  "Level1.lvl",
  "Level2.lvl",
  "Level3.lvl",
  "Level4.lvl",
  "End.lvl",
};

const char *
Level::m_altarSprites[][8] = {
  {  
	"AltarFull1.spr",
	"AltarWaiting1.spr",
	"AltarCharging1.spr",
	"AltarEmpty1.spr",
	
	"FinalAltarFull1.spr",
	"FinalAltarRelicMerging1.spr",
	"FinalMerging1.spr",
	"",
  },
  {
	"AltarFull2.spr",
	"AltarWaiting2.spr",
	"AltarCharging2.spr",
	"AltarEmpty2.spr",
	
	"FinalAltarFull2.spr",
	"FinalAltarRelicMerging2.spr",
	"FinalMerging2.spr",
	"",
  },
};

const char * 
Level::m_doorSprites[][2] = {
	{	  
	  "Door1.spr",
	  "DoorOpening1.spr",
	},
	{	  
	  "Door2.spr",
	  "DoorOpening2.spr",
	},
	{	  
	  "Door3.spr",
	  "DoorOpening3.spr",
	},
	{	  
	  "Door4.spr",
	  "DoorOpening4.spr",
	},
	
};

Level::Level(Engine * engine, Game * game)
	: m_engine(engine), m_game(game)
{
	Engine::log("Created a level");

	m_altarSprite0 = m_engine->getAvailableSprite();
	m_altarSprite0->setAnimation(true);
	m_altarSprite1 = m_engine->getAvailableSprite();
	m_altarSprite1->setAnimation(true);
	m_hornSprite = m_engine->getAvailableSprite();
	m_doorSprite0 = m_engine->getAvailableSprite();
	m_doorSprite0->setPriority(384);
	m_doorSprite1 = m_engine->getAvailableSprite();
	m_doorSprite1->setPriority(384);
	m_doorSprite2 = m_engine->getAvailableSprite();
	m_doorSprite2->setPriority(384);
	m_doorSprite3 = m_engine->getAvailableSprite();
	m_doorSprite3->setPriority(384);
	m_door = new Door();
	for ( int i = 0 ; i < MAX_ALTARS ; i++ ) {
		m_altars[i] = new Altar();
		m_horns[i] = new Horn();
	}
	m_nextLevel = new char[30];
	memset(m_nextLevel, 0, 30);
	for ( int i = 0 ; i < 6 ; i++ ) {
		m_events[i].name = new char[35];
		memset(m_events[i].name, 0, 35);
	}
}

void Level::reset()
{
	Engine::log("Level::reset()");
	for (int j = 0; j < MAX_SCREENS; j++)
		for (int i = 0; i < MAX_BG; i++)
			m_backs[j][i] = m_engine->getBackground(i, j);

	m_width = 0;
	m_height = 0;
	for (int i = 0; i < MAX_ALTARS; i++) {
	  m_altars[i]->x = 0;
	  m_altars[i]->y = 0;
	  m_altars[i]->state = FULL;
	}
	m_playerStartX = 0;
	m_playerStartY = 0;
	m_relicStartX = 0;
	m_relicStartY = 0;

	m_altarCurrentlyLoaded = 0;

	m_bellSoundChannel = NOCHANNEL;
	m_bellSoundTimer = 0;

	getStreamedSound()->stop();

	if ( m_altarCount > 0 ) {
		m_altarSprite0->load(m_altarSprites[0][m_altars[0]->special * 4]);
		m_altarSprite1->load(m_altarSprites[1][m_altars[0]->special * 4]);
	} else {
		m_altarSprite0->load(m_altarSprites[0][0]);
		m_altarSprite1->load(m_altarSprites[1][0]);
	}
	m_doorSprite0->load(m_doorSprites[0][0]);
	m_doorSprite1->load(m_doorSprites[1][0]);
	m_doorSprite2->load(m_doorSprites[2][0]);
	m_doorSprite3->load(m_doorSprites[3][0]);
	m_hornSprite->load("Horn.spr");
}

bool
Level::load(const char * filename)
{
	char filepath[64];
	strcpy(filepath, "/tepeyo/lvl/");
	strcat(filepath, filename);
  
	Engine::log("Level::Load(%s)", filepath);

	if ( strcmp(filename, "outro") == 0 ) {
		playCutScene(m_engine, CUTSCENE_OUTRO, false);
		m_engine->resetBackgrounds(0);
		m_engine->resetBackgrounds(1);
		Background * thxTop = m_engine->getBackground(0, 0);
		Background * thxBottom = m_engine->getBackground(0, 1);
		if ( thxTop->load("pix/Credit5.bg") )
			thxTop->setVisibility(true);
		if ( thxBottom->load("pix/Credit6.bg") )
			thxBottom->setVisibility(true);
		m_engine->fade(0, 16);
		m_engine->fade(1, 16);
		while ( true ) {
			m_engine->waitForDisplay();
			m_engine->update();
		}
	}

	reset();
	
	FILE * file;
	int bufferSize = 30;
	char buffer[bufferSize];
	bool loaded;
	
	file = fopen(filepath, "rb");
	if ( !file )
	  {
		Engine::log("Level::load() : Couldn't open the file \"%s\"", filepath);
	  }

	//next level filename
	fread(m_nextLevel, 30, 1, file);
	Engine::log("m_nextLevel = %s", m_nextLevel);

	// music filename
	fread(& buffer, sizeof(char) * bufferSize, 1, file);
	char ambiantName[35];
	strcpy(ambiantName, "/tepeyo/snd/");
	strcat(ambiantName, buffer);
	getStreamedSound()->load(ambiantName);
	getStreamedSound()->play();
	getStreamedSound()->setVolume(0);
	getStreamedSound()->fadein();

	// collision filename
	fread(& buffer, sizeof(char) * bufferSize, 1, file);
	loadCollisionMap(buffer);

	// reset the backgrounds
	m_engine->resetBackgrounds(0);
	m_engine->resetBackgrounds(1);

	// bg0
	fread(& buffer, sizeof(char) * bufferSize, 1, file);
	loaded = m_backs[0][0]->load(buffer, false);
	m_backs[0][0]->setVisibility(loaded);
	loaded = m_backs[1][0]->load(buffer, false);
	m_backs[1][0]->setVisibility(loaded);
	fread(& m_parallaxX[0], 2, 1, file);  //parallax-x
	fread(& m_parallaxY[0], 2, 1, file);  //parallax-y
	strncpy(m_bgName[0], buffer, 35);

	// bg1
	fread(& buffer, sizeof(char) * bufferSize, 1, file);
	loaded = m_backs[0][1]->load(buffer, false);
	m_backs[0][1]->setVisibility(loaded);
	loaded = m_backs[1][1]->load(buffer, false);
	m_backs[1][1]->setVisibility(loaded);
	fread(& m_parallaxX[1], 2, 1, file);  //parallax-x
	fread(& m_parallaxY[1], 2, 1, file);  //parallax-y
	strncpy(m_bgName[1], buffer, 35);

	// bg2
	fread(& buffer, sizeof(char) * bufferSize, 1, file);
	loaded = m_backs[0][2]->load(buffer, false);
	m_backs[0][2]->setVisibility(loaded);
	loaded = m_backs[1][2]->load(buffer, false);
	m_backs[1][2]->setVisibility(loaded);
	fread(& m_parallaxX[2], 2, 1, file);  //parallax-x
	fread(& m_parallaxY[2], 2, 1, file);  //parallax-y
	strncpy(m_bgName[2], buffer, 35);

	// bg3
	fread(& buffer, sizeof(char) * bufferSize, 1, file);
	loaded = m_backs[0][3]->load(buffer, false);
	m_backs[0][3]->setVisibility(loaded);
	loaded = m_backs[1][3]->load(buffer, false);
	m_backs[1][3]->setVisibility(loaded);
	fread(& m_parallaxX[3], 2, 1, file);  //parallax-x
	fread(& m_parallaxY[3], 2, 1, file);  //parallax-y
	strncpy(m_bgName[3], buffer, 35);

	// hero
	fread(& m_playerStartX, 2, 1, file);
	fread(& m_playerStartY, 2, 1, file);

	// relic
	fread(& m_relicStartX, 2, 1, file);
	fread(& m_relicStartY, 2, 1, file);
	fread(& m_relicPower, 2, 1, file);  // relic charge

	// door
	fread(& m_door->x, 2, 1, file);
	fread(& m_door->y, 2, 1, file);
	fread(& buffer, sizeof(char) * bufferSize, 1, file);   // sound played when the door opens
	m_door->state = CLOSED;

	// altars
	fread(& m_altarCount, 1, 1, file);
	m_altarsRemaining = m_altarCount;
	if (m_altarCount == 0)
	  m_door->state = OPEN;
	for (int i = 0; i < m_altarCount; i++)
	{
		fread(& m_altars[i]->x, 2, 1, file);
		fread(& m_altars[i]->y, 2, 1, file);
		fread(& m_horns[i]->x, 2, 1, file);
		fread(& m_horns[i]->y, 2, 1, file);
		fread(& m_altars[i]->special, 1, 1, file);
		m_altars[i]->state = FULL;
	}
	if ( m_altarCount != 0 )
		setAltarState(0, FULL, true);

	// events
	m_currentEvent = 0;
	fread(& m_eventCount, 1, 1, file);
	for ( int i = 0 ; i < m_eventCount ; i++ ) {
		fread(& m_events[i].x, 2, 1, file);
		fread(& m_events[i].y, 2, 1, file);
		strcpy(m_events[i].name, "pix/");
		fread(m_events[i].name + 4, 1, 30, file);
	}

	fclose(file);

	m_chargeSoundPlayed = false;

	return true;
}

bool
Level::load(int index)
{
	return load(m_levelFiles[index]);
}

bool
Level::loadCollisionMap(const char * filename)
{
	char filepath[64];
	strcpy(filepath, "/tepeyo/col/");
	strcat(filepath, filename);
	
	Engine::log("Loading collision map \"%s\"", filepath);

	FILE * colFile;
	int w, h;
	colFile = fopen(filepath, "rb");

	if (!colFile)
	{
		Engine::log("Level::loadCollisionMap() Couldn't load \"%s\". Exiting.", filepath);
		return false;
	}

	fread(& w, sizeof(int), 1, colFile);
	fread(& h, sizeof(int), 1, colFile);
	m_width = w * 8; // tile size = 8
	m_height = h * 8;

	for (int i = 0; i < w * h; i++)
	{
		fread(& m_colmap[i], sizeof(char), 1, colFile);
	}

	fclose(colFile);

	return true;
}

void 
Level::update(bool showEvent)
{
	Hero * hero = m_game->getHero();
	Camera * topCam = m_game->getTopCamera();
	Camera * bottomCam = m_game->getBottomCamera();
	int dist;  //manhattan distance
	int temp;
	int index; //index of the nearest altar/horn

	//update top screen
	for (int i = 0; i < MAX_BG; i++)
	{
		// when parallax = 0, don't move the bg at all and display it on both (careful, bg height must be 192*2
		if (m_parallaxX[i] == 0 && m_parallaxY[i] == 0) {
			m_backs[0][i]->setPosition(0,0);
		}
		else {
		m_backs[0][i]->setPosition(
			round12(topCam->getX()) * m_parallaxX[i] / 256,
			round12(topCam->getY()) * m_parallaxY[i] / 256);
		}
	}
	
	//update bottom screen
	for (int i = 0; i < MAX_BG; i++)
	{
		// when parallax = 0, don't move the bg at all and display it on both (careful, bg height must be 192*2
		if (m_parallaxX[i] == 0 && m_parallaxY[i] == 0) {
			m_backs[1][i]->setPosition(0,192);
		}
		else {
		m_backs[1][i]->setPosition(
			round12(bottomCam->getX()) * m_parallaxX[i] / 256, 
			round12(bottomCam->getY()) * m_parallaxY[i] / 256);
		}
	}

	// update door
	int topdoorx = m_game->world2camTopX(m_door->x);
	int topdoory = m_game->world2camTopY(m_door->y);
	int bottomdoorx = m_game->world2camBottomX(m_door->x);
	int bottomdoory = m_game->world2camBottomY(m_door->y);
	if (m_door->state != OPEN) {
	  m_doorSprite0->setPosition(topdoorx, topdoory - 128, bottomdoorx, bottomdoory - 128);
	  m_doorSprite1->setPosition(topdoorx + 64, topdoory - 128, bottomdoorx + 64, bottomdoory - 128);
	  m_doorSprite2->setPosition(topdoorx, topdoory - 64, bottomdoorx, bottomdoory - 64);
	  m_doorSprite3->setPosition(topdoorx + 64, topdoory - 64, bottomdoorx + 64, bottomdoory - 64);
	}
	else {
	  m_doorSprite0->setPosition(256, 192, 256, 192);
	  m_doorSprite1->setPosition(256, 192, 256, 192);
	  m_doorSprite2->setPosition(256, 192, 256, 192);
	  m_doorSprite3->setPosition(256, 192, 256, 192);
	  
	  //is the hero near the exit door?
	  if (round12(hero->getX()) > m_door->x &&
		  round12(hero->getX()) < m_door->x + 128 &&
		  round12(hero->getY()) > m_door->y &&
		  round12(hero->getY()) < m_door->y + 48)
		{
		  m_game->endLevel();
		}
	}

	//update altars
	if ( m_altarCount == 0 ) {
	  m_altarSprite0->setPosition(256,192,256,192);
	  m_altarSprite1->setPosition(256,192,256,192);
	}
	else 
	  {
		index = 0;
		dist = Tepeyo::abs(round12(hero->getX()) - m_altars[index]->x) + Tepeyo::abs(round12(hero->getY()) - m_altars[index]->y);
		for (int i = 1; i < m_altarCount; i++)
		  {
			temp = Tepeyo::abs(round12(hero->getX()) - m_altars[i]->x) + Tepeyo::abs(round12(hero->getY()) - m_altars[i]->y);
			if ( temp < dist )
			  {
				dist = temp;
				index = i;
			  }
		  }
		if ( m_altars[index]->state == FULL ) {
			if ( m_bellSoundTimer == 0 ) {
				m_bellSoundTimer = BELLSOUNDTIME + genrand_int32()%16;
				m_bellSoundChannel = 
					BufferedSoundPlay(SND_ALTAR_CLOCHE0 + genrand_int32()%4);
			}
			m_bellSoundTimer--;
			if ( dist < 1024 ) {
				if ( m_bellSoundChannel != NOCHANNEL ) {
					SoundSetVolume(m_bellSoundChannel, 
							(((1024 - dist) * 127) / 1024) );
					int pan;
					pan = m_altars[index]->y - round12(hero->getX()) - 64;
					if ( pan < 0 )
						pan = 0;
					else if ( pan > 127 )
						pan = 127;
					SoundSetPanning(m_bellSoundChannel, pan);
				}
			} else {
				SoundSetVolume(m_bellSoundChannel, 0);
			}
#if 1 // met 1 une fois que t'as codé le système complet pour les autels
		} else if ( m_altars[index]->state == WAITING ) {
			if ( m_bellSoundChannel == NOCHANNEL )
				m_bellSoundChannel = BufferedSoundPlay(SND_ALTAR_ENERGY);
		} else if ( m_altars[index]->state == CHARGING ) {
			//BufferedSoundStop(m_bellSoundChannel);
			if ( ! m_chargeSoundPlayed ) {
				BufferedSoundPlay(SND_RELIC_CHARGE);
				m_chargeSoundPlayed = true;
			}
		}
#else
	  } else if ( m_altars[index]->state == WAITING ) {
		  if ( m_bellSoundChannel == NOCHANNEL )
			  m_bellSoundChannel = BufferedSoundPlay(SND_ALTAR_ENERGY);
	  } else if ( m_altars[index]->state == CHARGING ) {
		  BufferedSoundPlay(SND_RELIC_CHARGE);
	  }
#endif
		if (m_altarCurrentlyLoaded != index) {
		  m_altarCurrentlyLoaded = index;
		  m_altarSprite0->load(m_altarSprites[0][m_altars[index]->state + m_altars[index]->special * 4]);
		  m_altarSprite1->load(m_altarSprites[1][m_altars[index]->state + m_altars[index]->special * 4]);
		}
		m_altarSprite0->setPosition(
									m_game->world2camTopX(m_altars[index]->x), m_game->world2camTopY(m_altars[index]->y - 128),
									m_game->world2camBottomX(m_altars[index]->x), m_game->world2camBottomY(m_altars[index]->y - 128));
		m_altarSprite1->setPosition(
									m_game->world2camTopX(m_altars[index]->x), m_game->world2camTopY(m_altars[index]->y - 64),
									m_game->world2camBottomX(m_altars[index]->x), m_game->world2camBottomY(m_altars[index]->y - 64));
	  }

	//update horns
	if ( m_altarCount == 0 ) {
		m_hornSprite->setPosition(256,192,256,192);
	}
	else {
		index = 0;
		dist = Tepeyo::abs(round12(hero->getX()) - m_horns[index]->x) + Tepeyo::abs(round12(hero->getY()) - m_horns[index]->y);
		for (int i = 1; i < m_altarCount; i++)
		{
			temp = Tepeyo::abs(round12(hero->getX()) - m_horns[i]->x) + Tepeyo::abs(round12(hero->getY()) - m_horns[i]->y);
			if ( temp < dist )
			{
				dist = temp;
				index = i;
			}
		}
		m_hornSprite->setPosition(
			m_game->world2camTopX(m_horns[index]->x), m_game->world2camTopY(m_horns[index]->y - 64),
			m_game->world2camBottomX(m_horns[index]->x), m_game->world2camBottomY(m_horns[index]->y - 64));
	}

	// Events
	if ( showEvent ) {
		if ( m_currentEvent < m_eventCount ) {
			if ( playEvent(m_game, m_events[m_currentEvent].x, m_events[m_currentEvent].y,
						m_events[m_currentEvent].name) )
				m_currentEvent++;
		}
	}
}

char
Level::getTileType(int x, int y) const
{
	x /= 8;
	y /= 8;
	int index = y * m_width / 8 + x;

	if (index >= m_width / 8 * m_height / 8)
	{
		Engine::log("Error in getTileType");
		return ROCK;
	}

	return m_colmap[y * m_width / 8 + x];
}

int
Level::findAltar(int12 _x, int12 _y)
{
  int mdist; //manhattan distance
  int dx, dy;
  int index = -1;

  for (int i = 0; i < m_altarCount; i++)
	{
	  dx = Tepeyo::abs(round12(_x) - m_altars[i]->x);
	  dy = Tepeyo::abs(round12(_y) - m_altars[i]->y);
	  mdist = dx + dy;
	  if (mdist < 92)
		{
		  index = i;
		}
	}

  return index;
}

void
Level::setAltarState(int index, int state, bool forceload) 
{
  m_altars[index]->state = state;
 
  if ( m_altarCurrentlyLoaded == index || forceload )
	{
	  m_altarSprite0->load(m_altarSprites[0][m_altars[index]->state + m_altars[index]->special * 4]);
	  m_altarSprite1->load(m_altarSprites[1][m_altars[index]->state + m_altars[index]->special * 4]);
	}
}

bool
Level::isAltarActive(int12 _x, int12 _y)
{
  int index = findAltar(_x, _y);
  return (m_altars[index]->state != EMPTY);
}


int
Level::findHorn(int12 _x, int12 _y)
{
  int mdist; //manhattan distance
  int dx, dy;
  int index = -1;

  for (int i = 0; i < m_altarCount; i++)
	{
	  dx = Tepeyo::abs(round12(_x) - m_horns[i]->x);
	  dy = Tepeyo::abs(round12(_y) - m_horns[i]->y);
	  mdist = dx + dy;
	  if (mdist < 92)
		{
		  index = i;
		}
	}

  return index;
}

void
Level::activateHorn(int12 _x, int12 _y)
{
  Relic * relic = m_game->getRelic();
  int index = findHorn(_x, _y);
  Camera * cam = m_game->getTopCamera();
  
  if (isAltarWaiting(index))
	{
	  /* before the animation */
	  lcdSwap();
	  setAltarState(index, CHARGING);
	  cam->setPosition(sub12(relic->getX(), fp12(256 / 2)), sub12(relic->getY(), fp12(192 / 2)));
	  relic->enable(true);
	  relic->setCharging(true);
	  relic->setPosition(relic->getX(), relic->getY());
	  relic->update();

	  /* the animation */
	  for (int i = 0; i < 50; i++)
		{
		  this->update();
		  m_engine->waitForDisplay();
		  m_engine->update();
		}
	  
	  for (int i = 0; i < 7*4; i++)
		{
		  this->update();
		  m_engine->waitForDisplay();
		  m_engine->update();
		}

	  relic->setCharging(false);
	  relic->setPosition(relic->getX(), add12(relic->getY(), fp12(48)));
	  relic->update();
	  setAltarState(index, EMPTY);

	  for (int i = 0; i < 60; i++)
		{
		  this->update();
		  m_engine->waitForDisplay();
		  m_engine->update();
		}
 
	  /* after the animation */
	  lcdSwap();
	  relic->setPower(relic->getPower() + relic->getMaxPower() / 3 + 1);
	  cam->setPosition(cam->getOldX(), cam->getOldY());
	  m_chargeSoundPlayed = false;
	  m_altarsRemaining--;

	  if (m_altarsRemaining == 0) {
		openTheDoor();
	  }
	}
}

void
Level::openTheDoor()
{
  Camera * cam = m_game->getTopCamera();
  Hero * hero = m_game->getHero();
  Relic * relic  = m_game->getRelic();
  int countdown;


  cam->setPosition(
				   fp12(m_door->x - 64),
				   fp12(m_door->y - 160));
  hero->setPosition(
					hero->getX(),
					hero->getY());
  hero->setAnimation(false);
  relic->setPosition(
					 relic->getX(),
					 relic->getY());
  
  for (int i = 0; i < 60; i++)
	{
	  this->update();
	  m_engine->waitForDisplay();
	  m_engine->update();
	}

  BufferedSoundPlay(SND_DOOR);
  
  m_doorSprite0->load(m_doorSprites[0][OPENING]);
  m_doorSprite0->setAnimation(true);  
  m_doorSprite1->load(m_doorSprites[1][OPENING]);
  m_doorSprite1->setAnimation(true);  
  m_doorSprite2->load(m_doorSprites[2][OPENING]);
  m_doorSprite2->setAnimation(true);  
  m_doorSprite3->load(m_doorSprites[3][OPENING]);
  m_doorSprite3->setAnimation(true);

  countdown = m_doorSprite0->getFrameCount() * m_doorSprite0->getAnimationSpeed();

  while ( countdown > 0 - 60 )
	{  
	  this->update();
	  m_engine->waitForDisplay();
	  m_engine->update();
	  countdown--;
	  
	  if (countdown < 0)
		{
		  m_doorSprite0->setAnimation(false);  
		  m_doorSprite1->setAnimation(false);  
		  m_doorSprite2->setAnimation(false);  
		  m_doorSprite3->setAnimation(false);

		  m_doorSprite0->setPosition(256, 192, 256, 192);
		  m_doorSprite1->setPosition(256, 192, 256, 192);
		  m_doorSprite2->setPosition(256, 192, 256, 192);
		  m_doorSprite3->setPosition(256, 192, 256, 192);
		  
		  m_door->state = OPEN;		  
		  m_engine->waitForDisplay();
		  m_engine->update();
		}
	}

  cam->setPosition(cam->getOldX(), cam->getOldY());
  hero->setAnimation(true);
}

bool
Level::isAltarWaiting(int index)
{
  return m_altars[index]->state == WAITING;
}

void Level::nextLevel()
{
	load(m_nextLevel);
}

void Level::resetAltarsAndDoor()
{
	m_altarsRemaining = m_altarCount;
	if (m_altarCount == 0)
	  m_door->state = OPEN;
	for ( int i = 0 ; i < m_altarCount ; i++ )
		setAltarState(i, FULL);
}

void Level::reloadTopBGs()
{
	m_engine->resetBackgrounds(0);
	Engine::log("Reloading : %s\n%s\n%s\n%s",
			m_bgName[0],
			m_bgName[1],
			m_bgName[2],
			m_bgName[3]);
	if ( m_backs[0][0]->load(m_bgName[0]) )
		m_backs[0][0]->setVisibility(true);
	else
		m_backs[0][0]->setVisibility(false);
	if ( m_backs[0][1]->load(m_bgName[1]) )
		m_backs[0][1]->setVisibility(true);
	else
		m_backs[0][1]->setVisibility(false);
	if ( m_backs[0][2]->load(m_bgName[2]) )
		m_backs[0][2]->setVisibility(true);
	else
		m_backs[0][2]->setVisibility(false);
	if ( m_backs[0][3]->load(m_bgName[3]) )
		m_backs[0][3]->setVisibility(true);
	else
		m_backs[0][3]->setVisibility(false);
}

void Level::playFinal()
{
	int countdown;
	Hero * hero = m_game->getHero();
	Relic * relic = m_game->getRelic();

	m_door->state = OPEN;

	// Watch animation
	//BufferedSoundPlay(SND_TONATIUH_FFINALE1);
	m_game->getBottomCamera()->setPosition(fp12(m_altars[0]->x+32-128), fp12(m_altars[0]->y-96-32));
	hero->setState(hero->getIdleState());
	hero->setPosition(hero->getX(), hero->getY());
	relic->setPosition(relic->getX(), relic->getY());
	relic->enable(false);
	update();
	
	// Merge relic with altar
	BufferedSoundPlay(SND_TONATIUH_FUSIONFINALE);
	//BufferedSoundPlay(SND_TONATIUH_FFINALE3);
	countdown = m_altarSprite0->getFrameCount() * m_altarSprite0->getAnimationSpeed();
	while ( countdown > 0 ) {
		m_engine->waitForDisplay();
		m_engine->update();
		countdown--;
	}

	// Merge player with relic
	hero->setVisibility(false);
	hero->update();
	m_altarSprite0->load("FinalMerging1.spr");
	m_altarSprite0->setFrame(0);
	m_altarSprite1->load("FinalMerging2.spr");
	m_altarSprite1->setFrame(0);
	countdown = m_altarSprite0->getFrameCount() * m_altarSprite0->getAnimationSpeed();
	while ( countdown > 0 ) {
		m_engine->waitForDisplay();
		m_engine->update();
		countdown--;
	}

	// Let's ...die !?
	Engine::log("Getting ready to die");
	hero->setVisibility(true);
	hero->setBerserk();
	hero->setState(hero->getIdleMergedState());
	hero->setPosition(fp12(m_altars[0]->x + 32), fp12(m_altars[0]->y));
	hero->setMaxSpeed(int12(2500));
	relic->setBerserk();
	relic->setState(relic->getCarriedState());
	relic->setPosition(fp12(m_altars[0]->x + 32), fp12(m_altars[0]->y - 64));
	relic->enable(true);
	m_altarSprite0->setVisibility(false);
	m_altarSprite1->setVisibility(false);
	m_game->getBottomCamera()->lookAt(relic);
}

void Level::showElement(int12 x, int12 y, int time, bool direct)
{
	Vector2 move;
	int maxSpeedSquared;

	Engine::log("Showing (%d,%d) for %d seconds", round12(x), round12(y), time/60);

	Camera * topCam;
	Camera * bottomCam;

	topCam = m_game->getTopCamera();
	bottomCam = m_game->getBottomCamera();

	maxSpeedSquared = mul12(bottomCam->getMaxSpeed(), bottomCam->getMaxSpeed()).value;

	if ( round12(y) > m_height - 192 )
		y = fp12(m_height - 192);

	// Go to the target
	if ( direct ) {
		m_engine->waitForDisplay();
		m_engine->update();
		bottomCam->setPosition(x, y);
		topCam->update();
		update(false);
		m_engine->waitForDisplay();
		m_engine->update();
	} else {
		while ( bottomCam->getX().value != x.value || bottomCam->getY().value != y.value ) {
			m_engine->waitForDisplay();
			m_engine->update();

			move.x = sub12(x, bottomCam->getX());
			move.y = sub12(y, bottomCam->getY());
			if ( move.squaredNorm().value > maxSpeedSquared ) {
				move.normalize();
				move = move * bottomCam->getMaxSpeed();
			}
			bottomCam->setPosition(add12(bottomCam->getX(), move.x),
					add12(bottomCam->getY(), move.y));
			bottomCam->update();
			topCam->update();
			update(false);
			
			Engine::log("CameraPosition(%d,%d) move(%d,%d)", 
					round12(bottomCam->getX()), round12(bottomCam->getY()),
					round12(move.x), round12(move.y));
		}
	}
	Engine::log("OnTarget (%d,%d)", round12(bottomCam->getX()), round12(bottomCam->getY()));

	// Wait a moment on the target
	for ( int i = 0 ; i < time ; i++ ) {
		m_engine->waitForDisplay();
		m_engine->update();
	}
}

void Level::presentLevel()
{
	/*
	Camera * topCam;
	Camera * bottomCam;

	topCam = m_game->getTopCamera();
	bottomCam = m_game->getBottomCamera();

	topCam->lookAt(0);
	topCam->update();
	bottomCam->lookAt(0);

	showElement(fp12(m_door->x+64), fp12(m_door->y-64), 30, true);
	for ( int i = 0 ; i < m_altarCount ; i++ ) {
		showElement(fp12(m_altars[i]->x+32), fp12(m_altars[i]->y+32), 60);
		showElement(fp12(m_horns[i]->x-32), fp12(m_horns[i]->y-32), 60);
	}
	showElement(sub12(m_game->getHero()->getX(), fp12(128)), sub12(m_game->getHero()->getY(), fp12(96)), 0);
	*/

//	topCam->lookAt(m_game->getRelic());
//	bottomCam->lookAt(m_game->getHero());
}
