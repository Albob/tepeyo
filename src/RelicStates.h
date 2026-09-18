/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

//-------------------------------------------------------------------------------------------------------------

#ifndef _RELICSTATES_
#define _RELICSTATES_
 
#include "Relic.h" 

class HeroState;

using namespace Tepeyo;
 
class Relic; 
 
/** 
 * Pattern of a Relic State  
 */ 
class RelicState 
{ 
public: 
	Relic * m_parent; 
 
public: 
	RelicState(Relic* pRelic); 
	virtual ~RelicState(void); 
 
	virtual void enter() {} 
	virtual void update() {} 
	virtual void leave() {} 
}; 
 
 
//------------------------------------------------------------------------------------------------------------- 
 
/** Behavior of the relic when in Idle State */ 
class RelicIdleState : public RelicState 
{ 
public: 
	static const char * m_filenames[];
	static const char * m_waitingFilename;
	static const char * m_chargingFilename;
 
public: 
	RelicIdleState(Relic* pRelic); 
	virtual ~RelicIdleState(); 
 
	void enter(); 
	void update(); 
}; 
 
 
//------------------------------------------------------------------------------------------------------------- 
 
/** Behavior of the relic when in Idle State */ 
class RelicCarriedState : public RelicState 
{ 
public: 
	static const char * m_filenames[]; 
	HeroState * m_lastState;
 
public: 
	RelicCarriedState(Relic* pRelic); 
	virtual ~RelicCarriedState(); 
 
	void enter(); 
	void update(); 
	void leave();
}; 
 
 
//------------------------------------------------------------------------------------------------------------- 
 
/** Behavior of the relic when in Chasing State */ 
class RelicChasingState : public RelicState 
{ 
private:
	Sprite * m_handSprite;
	
	int12 m_handX, m_handY;
	int m_handXOrigin, m_handYOrigin;
	int m_handDirection, m_handOldDirection;
	int12 m_handSpeed;

	int chasingSound;

private: 
	static const char * m_filenames[]; 
	static const char * m_handfilenames[];
	static const char * m_chargingFilename;

public: 
	RelicChasingState(Relic* pRelic); 
	virtual ~RelicChasingState(); 
 
	void enter(); 
	void update(); 
	void leave();
}; 
 
 
//------------------------------------------------------------------------------------------------------------- 
 
/** Behavior of the relic when in "flying" State! */ 
class RelicFlyingState : public RelicState 
{
private: 
	static const char * m_filenames[];
	bool touchGround;
	bool autochase;
	int slideSound;

public:
	static const int12 gravity;
	static const int12 friction;

public: 
	RelicFlyingState(Relic* pRelic); 
	virtual ~RelicFlyingState(); 
 
	void enter(); 
	void update(); 
	void leave();
}; 
 
//------------------------------------------------------------------------------------------------------------- 
 
#endif 
 

