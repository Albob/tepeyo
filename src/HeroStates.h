/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#ifndef _HEROSTATES_
#define _HEROSTATES_

#include "Hero.h"
class Hero;


//-------------------------------------------------------------------------------------------------------------

/** 
 * This is a an implementation of the pattern State, suited for the Hero.
 * @see <a href="../../actor%20states/hero_states.png">Hero State Machine</a>
 */
class HeroState
{
protected:
	Hero* m_parent;

public:
	HeroState(Hero* parent);
	virtual ~HeroState();
	virtual void clickOnHero() {}
	virtual void clickOnRelic() {}
	virtual void clickOnGround(int x, int y) {}
	virtual void targetReached() {}
	virtual void stylusReleased() {}
	virtual void blow() {}
	virtual void enter() {}
	virtual void update() {}
	virtual void leave() {}
	virtual void fall() {}
	virtual const char* getFilename(int direction) { return ""; }
};


//-------------------------------------------------------------------------------------------------------------

/** Implementation of the idle state
*	@see <a href="../../actor%20states/hero_states.png">Hero State Machine</a>
*/
class HeroStateIdle: public HeroState
{
	static const int FIRST_BREATH = 120; // two second
	static const int BREATH = 60*5; // every five second
	int breathTime;
	/** In Idle state, the hero can be drawn in 8 directions */
	static const char * m_filenames[];

public:
	HeroStateIdle(Hero* parent);
	virtual ~HeroStateIdle();
	void enter();
	void update();
	void leave();
	void clickOnGround(int x, int y);
	void clickOnRelic();
	inline const char * getFilename(int direction) { return m_filenames[direction];}
};



//-------------------------------------------------------------------------------------------------------------

/** Implementation of the moving state
*	@see <a href="../../actor%20states/hero_states.png">Hero State Machine</a>
*/
class HeroStateMoving: public HeroState
{
	static const int STEPTIME = 10; // time to wait (in frames) between two step sounds
	int step;

public:
	/** In Moving state, the hero can be drawn in 8 directions */
	static const char * m_filenames[];
	
public:
	HeroStateMoving(Hero* parent);
	virtual ~HeroStateMoving();
	void enter();
	void update();
	void leave();
	void clickOnGround(int x, int y);
	void stylusReleased();
	void targetReached();
	void fall();
	inline const char * getFilename(int direction) { return m_filenames[direction];}
};


//-------------------------------------------------------------------------------------------------------------

/** Implementation of the merged state
*	@see <a href="../../actor%20states/hero_states.png">Hero State Machine</a>
*/
class HeroStateIdleMerged: public HeroState
{
public:
	/** In Idle state, the hero can be drawn in 8 directions */
	static const char * m_filenames[3][8];

public:
	HeroStateIdleMerged(Hero* parent);
	virtual ~HeroStateIdleMerged();
	void enter();
	void update();
	void leave();
	void clickOnHero();
	void clickOnGround(int x, int y);
	inline const char * getFilename(int direction);
};



//-------------------------------------------------------------------------------------------------------------

/** Implementation of the moving merged state
*	@see <a href="../../actor%20states/hero_states.png">Hero State Machine</a>
*/
class HeroStateMovingMerged: public HeroState
{
	static const int STEPTIME = 10; // time to wait (in frames) between two step sounds
	int step;

public:
	/** In Moving merged state, the hero can be drawn in 8 directions */
	static const char * m_filenames[3][8];
	
public:
	HeroStateMovingMerged(Hero* parent);
	virtual ~HeroStateMovingMerged();
	void enter();
	void update();
	void leave();
	void clickOnGround(int x, int y);
	void targetReached();
	void fall();
	const char * getFilename(int direction);
};



//-------------------------------------------------------------------------------------------------------------

/** When the Relic and Tonatiuh merge */
class HeroStateMerging: public HeroState
{
public:
	static const char * m_filenames[];
	int countdown;

public:
	HeroStateMerging(Hero* parent);
	virtual ~HeroStateMerging();
	void enter();
	void update();
	void leave();
	inline const char * getFilename(int direction) { return m_filenames[0];}
};



//-------------------------------------------------------------------------------------------------------------

/** When Tonatiuh and the Relic separate from each other */
class HeroStateUnmerging: public HeroState
{
private:
	static const char * m_filenames[];
	int m_countdown;
	int m_totalFrames;
	bool m_playbackward;

public:
	HeroStateUnmerging(Hero* parent);
	virtual ~HeroStateUnmerging();

	void enter();
	void update();
	void leave();
	void stylusReleased();
	inline const char * getFilename(int direction) { return m_filenames[0];}
};

//-------------------------------------------------------------------------------------------------------------

/** Ready to throw: Tonatiuh is holding the relic above his head */
class HeroStateRTT: public HeroState
{
 private:
  static const char * m_filenames[];
  int m_stylusX;
  int m_stylusY;

 public:
  HeroStateRTT(Hero* parent);
  virtual ~HeroStateRTT();
  
  void enter();
  void update();
  void leave(); 
  void stylusReleased();
  inline const char * getFilename(int direction) { return m_filenames[direction];}
};


//-------------------------------------------------------------------------------------------------------------

class HeroStateFalling: public HeroState
{
 private:
  static const char * m_filenames[];
  int m_countdown;

 public:
  HeroStateFalling(Hero* parent);
  virtual ~HeroStateFalling();

  void enter();
  void update();
  void leave();
  inline const char * getFilename(int direction) { return m_filenames[direction / 2]; }
};

//-------------------------------------------------------------------------------------------------------------

class HeroStateFallingMerged: public HeroState
{
 private:
  static const char * m_filenames[];
  int m_countdown;

 public:
  HeroStateFallingMerged(Hero* parent);
  virtual ~HeroStateFallingMerged();

  void enter();
  void update();
  void leave();
  inline const char * getFilename(int direction) { return m_filenames[direction / 2]; }
};

//-------------------------------------------------------------------------------------------------------------

class HeroStateTeleporting: public HeroState
{
 private:
  static const char * m_filenames[];
  int m_countdown;

 public:
  HeroStateTeleporting(Hero* parent);
  virtual ~HeroStateTeleporting();

  void enter();
  void update();
  void leave();
  inline const char * getFilename(int direction) { return m_filenames[direction / 2]; }
};

//-------------------------------------------------------------------------------------------------------------

class HeroStateCaught: public HeroState
{
 private:
  static const char * m_filenames[];
  int m_countdown;

 public:
  HeroStateCaught(Hero* parent);
  virtual ~HeroStateCaught();

  void enter();
  void update();
  void leave();
  inline const char * getFilename(int direction) { return m_filenames[0]; }
};

//-------------------------------------------------------------------------------------------------------------


/** Ready to blow */
class HeroStateRTB: public HeroState
{
 private:
  static const char * m_filenames[];
  int m_countdown;

 public:
  HeroStateRTB(Hero* parent);
  virtual ~HeroStateRTB();

  void enter();
  void update();
  void leave();
  void blow();
  inline const char * getFilename(int direction) { return m_filenames[0]; }
};

//-------------------------------------------------------------------------------------------------------------


/** Blowing in a horn */
class HeroStateBlowing: public HeroState
{
 private:
  static const char * m_filenames[];
  int m_countdown;
  int m_countdown2;
  bool m_done;

 public:
  HeroStateBlowing(Hero* parent);
  virtual ~HeroStateBlowing();

  void enter();
  void update();
  void leave();
  inline const char * getFilename(int direction) { return m_filenames[0]; }
};

//------------------------------------------------------------------------------------------------------------

/** Dying state */
class HeroStateDie : public HeroState
{
private:
	int m_countdown;
	bool m_meteor;
	int m_meteorX, m_meteorY;
	Sprite * m_meteorSprite;

public:
	HeroStateDie(Hero * parent);
	virtual ~HeroStateDie();

	void enter();
	void update();
	void leave();
	const char * getFilename(int direction);
};

#endif

