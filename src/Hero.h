/*
* @author: Alexis Bauchu <alexis.bauchu@gmail.com>
* Tepeyollotl Engine
* Enjmin 2008
*/

#ifndef _HERO_
#define _HERO_

#include "Engine.h"
#include "Game.h"
#include "Actor.h"
#include "HeroStates.h"
#include "16cSprite.h"

class HeroState;
class HeroStateIdle;
class HeroStateMoving;
class HeroStateIdleMerged;
class HeroStateMovingMerged;
class HeroStateMerging;
class HeroStateUnmerging;
class HeroStateRTT;
class HeroStateFalling;
class HeroStateFallingMerged;
class HeroStateTeleporting;
class HeroStateCaught;
class HeroStateRTB;
class HeroStateBlowing;
class HeroStateDie;

using namespace Tepeyo;

/** 
* This is the hero: location in the world, behavior, display...
*/
class Hero: public Actor
{
public:
	typedef enum { HUMAN = 0, MERGED = 1, BERSERK = 2 } Form;

private:

	Entity* m_target;
	Entity* m_destination;
	Sprite* m_sprite;
	Sprite16c * m_shadow;
	HeroState * m_currentState;
	HeroState * m_prevState;
	HeroStateIdle * m_idleState;
	HeroStateMoving * m_movingState;
	HeroStateIdleMerged * m_idleMergedState;
	HeroStateMovingMerged * m_movingMergedState;
	HeroStateMerging * m_mergingState;
	HeroStateUnmerging * m_unmergingState;
	HeroStateRTT * m_RTTState;
	HeroStateFalling * m_fallingState;
	HeroStateFallingMerged * m_fallingMergedState;
	HeroStateTeleporting * m_teleportingState;
	HeroStateCaught * m_caughtState;
	HeroStateRTB * m_RTBState;
	HeroStateBlowing * m_blowingState;
	HeroStateDie * m_dieState;
	Form m_currentForm;
	bool m_visible;
	bool m_gogettherelic;

public:
	Hero(Engine* pEngine, Game* pGame);
	virtual ~Hero();

	//Setters
	void setTarget(Entity* target);
	void setTarget(int x, int y);
	void setState(HeroState* state);
	void setPosition(int12 x, int12 y);
	inline void setMerged() { m_currentForm = MERGED; }
	inline void setBerserk() { m_currentForm = BERSERK; m_shadow->hide(); }
	inline void setVisibility(bool visible) { m_visible = visible; }

	//Getters
	bool hasTheRelic() const;
	Entity* getTarget() { return m_target; }
	Form getForm() const { return m_currentForm; }
	inline HeroState* getPrevState()    const { return m_prevState; }
	inline HeroState* getCurrentState() const { return m_currentState; }
	inline HeroState* getIdleState()    const { return (HeroState*) m_idleState; }
	inline HeroState* getMovingState()  const { return (HeroState*) m_movingState; }
	inline HeroState* getRTTState()	    const { return (HeroState*) m_RTTState; }
	inline HeroState* getIdleMergedState()    const { return (HeroState*) m_idleMergedState; }
	inline HeroState* getMovingMergedState()  const { return (HeroState*) m_movingMergedState; }
	inline HeroState* getMergingState()       const { return (HeroState*) m_mergingState; }
	inline HeroState* getUnmergingState()     const { return (HeroState*) m_unmergingState; }
	inline HeroState* getFallingState()       const { return (HeroState*) m_fallingState; }
	inline HeroState* getFallingMergedState() const { return (HeroState*) m_fallingMergedState; }
	inline HeroState* getTeleportingState()   const { return (HeroState*) m_teleportingState; }
	inline HeroState* getCaughtState()        const { return (HeroState*) m_caughtState; }
	inline HeroState* getRTBState()           const { return (HeroState*) m_RTBState; }
	inline HeroState* getBlowingState()       const { return (HeroState*) m_blowingState; }
	inline HeroState* getDieState()			  const { return (HeroState*) m_dieState; }


	//States related methods
	void clickOnHero();
	void clickOnRelic();
	void clickOnGround(int x, int y);
	void targetReached();
	void stylusReleased();
	void blow();
	void update();
	void fall();

	void computeCollisions(int12 &x, int12 &y);
	void move();
	inline void setAnimation(bool value) { m_sprite->setAnimation(value);  }

	friend class HeroState;
	friend class HeroStateIdle;
	friend class HeroStateMoving;
	friend class HeroStateIdleMerged;
	friend class HeroStateMovingMerged;
	friend class HeroStateMerging;
	friend class HeroStateUnmerging;
	friend class HeroStateRTT;
	friend class HeroStateFalling;
	friend class HeroStateFallingMerged;
	friend class HeroStateTeleporting;
	friend class HeroStateCaught;
	friend class HeroStateRTB;
	friend class HeroStateBlowing;
	friend class HeroStateDie;
};

#endif

