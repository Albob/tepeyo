/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#ifndef _RELIC_
#define _RELIC_

#include "Engine.h"
#include "Game.h"
#include "Actor.h"
#include "RelicStates.h"
#include "16cSprite.h"

using namespace Tepeyo;

class RelicState;
class RelicIdleState;
class RelicCarriedState;
class RelicChasingState;
class RelicFlyingState;

/** 
 * Class Relic
 */
class Relic: public Actor
{
 public:
	typedef enum { INERT = 0, MAGIC = 1, BERSERK = 2 } Form;

 private:
  Sprite* m_sprite;
  Sprite* m_flameSprite;
  Sprite16c * m_shadow;
  RelicState * m_currentState;
  RelicState * m_prevState;
  RelicIdleState * m_idleState;
  RelicCarriedState * m_carriedState;
  RelicChasingState * m_chasingState;
  RelicFlyingState * m_flyingState;
  int12 m_dirX, m_dirY;
  int12 m_z;
  int12 m_zspeed;
  int m_power;
  int m_lastpower;
  int m_maxpower;
  int m_pokora;
  int m_onAltar; // -1 if none
  Form m_currentForm;
  bool m_enabled;
  bool m_charging;
  bool m_computePower;

  static const char * m_flameSpriteFiles[];

 public:
  Relic(Engine * engine, Game * game);
  virtual ~Relic();

  //Setters
  void setCharging(bool value);
  void setState(RelicState * state);
  void setPosition(int12 x, int12 y);
  inline void setAltitude(int12 alt) { m_z = alt; };
  inline void setPower(int pow) { m_computePower = true; m_lastpower = m_power; m_power = pow; }       //remember to multiply by 60 your value
  inline void setMaxPower(int pow) { m_maxpower = pow; } //remember to multiply by 60 your value
  inline void setOnAltar(int altar) { m_onAltar = altar; }
  inline void enable(bool enabled) {
	  if ( m_currentForm == BERSERK )
		  enabled = true;
	  m_enabled = enabled;
	  if ( ! m_enabled )
		  m_sprite->setPosition(256, 192, 256, 192);
  }
  inline void setMagic() { m_currentForm = MAGIC; }
  inline void setBerserk() { m_currentForm = BERSERK; }

  //Getters
  inline RelicState* getPrevState()    const { return m_prevState; }
  inline RelicState* getCurrentState() const { return m_currentState; }
  inline RelicState* getIdleState()    const { return (RelicState*)m_idleState; }
  inline RelicState* getCarriedState() const { return (RelicState*)m_carriedState; }
  inline RelicState* getChasingState() const { return (RelicState*)m_chasingState; }
  inline RelicState* getFlyingState()  const { return (RelicState*)m_flyingState; }
  inline int12 getAltitude() const { return m_z; }
  inline int getPower()      const { return m_power; }
  inline int getLastPower()	 const { return m_lastpower; }
  inline int getMaxPower()   const { return m_maxpower; }
  inline bool isEnabled()    const { return m_enabled; }
  inline bool onAltar()      const { return m_onAltar != -1; }
  
  void update();
  void throwAt(int x, int y);

  friend class RelicState;
  friend class RelicIdleState;
  friend class RelicCarriedState;
  friend class RelicChasingState;
  friend class RelicFlyingState;
};

#endif

