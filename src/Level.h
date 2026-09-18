/*
* @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
* Tepeyollotl Engine
* Enjmin 2008
*/

#ifndef _LEVEL_
#define _LEVEL_

class Event;

#include "Engine.h"
#include "Background.h"
#include "Entity.h"
#include "Game.h"
#include "AltarsANDdoors.h"
#include "Event.h"

#define VOID	0
#define BRIDGE	1
#define ROCK	2
#define EARTH	3
#define WALL	4
#define WATER	5
#define ALTAR	6

#define MAX_ALTARS 3

/** 
* Class Level
*/
class Level
{
private:
	struct EventDeclaration {
		int x;
		int y;
		char * name;//[32];
	};

	static const int BELLSOUNDTIME = 480;

	Engine * m_engine;
	Game * m_game;

	Background * m_backs[MAX_SCREENS][MAX_BG];
	int m_parallaxX[MAX_BG];
	int m_parallaxY[MAX_BG];
	Sprite * m_altarSprite0;
	Sprite * m_altarSprite1;
	Sprite * m_hornSprite;
	Sprite * m_doorSprite0;
	Sprite * m_doorSprite1;
	Sprite * m_doorSprite2;
	Sprite * m_doorSprite3;

	int m_width, m_height;

	int m_playerStartX, m_playerStartY;

	int m_relicStartX, m_relicStartY;
	int m_relicPower;

	int m_altarCount;
	int m_altarsRemaining;
	int m_altarCurrentlyLoaded;

	int m_bellSoundTimer;
	int m_bellSoundChannel;
	int m_chargeSoundPlayed;

	Door * m_door;
	Altar * m_altars[MAX_ALTARS];
	Horn * m_horns[MAX_ALTARS];

	int m_eventCount;
	int m_currentEvent;
	EventDeclaration m_events[6];

	char * m_nextLevel;//[30];

	char m_bgName[MAX_BG][35];

	char m_colmap[450*450];   //we assume here that the levels won't exceed a surface of 450x450 tiles... cross your fingers!

	static const char * m_levelFiles[];
	static const char * m_altarSprites[][8];
	static const char * m_doorSprites[][2];

private:
	bool loadCollisionMap(const char * filename);
	void openTheDoor();

public:
	Level(Engine * engine, Game * game);
	
	void reset();

	bool load(const char * filename);
	bool load(int index);
	void update(bool showEvent = true);
	void activateHorn(int12 _x, int12 _y);

	void nextLevel();
	void resetAltarsAndDoor();
	void reloadTopBGs();

	void showElement(int12 x, int12 y, int time, bool direct = false);
	void presentLevel();
	void playFinal();

	//setters
	void setAltarState(int index, int state, bool forceload = false);

	//getters
	int findAltar(int12 _x, int12 _y);
	int findHorn(int12 _x, int12 _y);
	bool isAltarActive(int12 x, int12 y);
	bool isAltarWaiting(int index);
	bool isAltarSpecial(int index) const { return m_altars[index]->special; }
	int getAltarsRemaining() const { return m_altarsRemaining; }
	char getTileType(int x, int y) const;  // use world coordinates
	inline int getPlayerStartX() const { return m_playerStartX; }
	inline int getPlayerStartY() const { return m_playerStartY; }
	inline int getRelicStartX() const { return m_relicStartX; }
	inline int getRelicStartY() const { return m_relicStartY; }
	inline int getRelicPower() const { return m_relicPower * 60; }
	inline int getDoorX() const { return m_door->x; }
	inline int getDoorY() const { return m_door->y; }
	inline int getAltarX(int index) const { return m_altars[index]->x; }
	inline int getAltarY(int index) const { return m_altars[index]->y; }
	inline int getHornX(int index) const { return m_horns[index]->x; }
	inline int getHornY(int index) const { return m_horns[index]->y; }
	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }
	inline char* getNextLevel() { return m_nextLevel; }
};

#endif

