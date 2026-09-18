/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#ifndef _GAME_
#define _GAME_

#include "Engine.h"
#include "Sprite.h"
#include "Level.h"
#include "Hero.h"
#include "Relic.h"
#include "Camera.h"

#define MAX_ENEMIES MAX_SPRITES - 2 // MAX sprites - 1 hero - 1 relic

class Hero;
class Relic;
class Level;

/** 
* This contains the logic of the game
*/
class Game
{
private:
	Engine * m_engine;
	Level  * m_level;
	Hero   * m_hero;
	Relic  * m_relic;
	Camera *m_topCamera, *m_bottomCamera;
	touchPosition touchPos;
	bool endlevel;

public:
	Game();
	
	//Getters
	inline Engine * getEngine() const { return m_engine; }
	inline Level  * getLevel()  const { return m_level; }
	inline Hero   * getHero()   const { return m_hero; }
	inline Relic  * getRelic()  const { return m_relic; }
	inline Camera * getTopCamera()	  const { return m_topCamera; }
	inline Camera * getBottomCamera() const { return m_bottomCamera; }
	inline int getStylusX() const { return touchPos.px; }
	inline int getStylusY() const { return touchPos.py; }

	/** Game Main loop */
	void run();
	/** Inits the game */
	void init(int level);
	/** Level stuff */
	void gameOver();
	void restartLevel();
	void endLevel();
	/** Displays the title screen and wait for the player to touch the screen
	 * Returns true if cheat code successes */
	bool titleScreen();
	void black();
	/** Converts coordinates from touchScreen to world */
	void touchScreen2world(touchPosition &tp);
	int world2camTopX(int i) { return i - round12(m_topCamera->getX()); }
	int world2camTopY(int i) { return i - round12(m_topCamera->getY()); }
	int world2camBottomX(int i) { return i - round12(m_bottomCamera->getX()); }
	int world2camBottomY(int i) { return i - round12(m_bottomCamera->getY()); }
};

#endif

