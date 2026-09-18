/*
 * @Author: Alexis Bauchu <alexis.bauchu@gmail.com>
 * Tepeyollotl Engine
 * Enjmin 2008
 */

#include "Relic.h"
#include "Hero.h"
#include "BufferedSound.h"

using namespace Tepeyo;

const char * Relic::m_flameSpriteFiles[] = {
	"Flame1.spr",
	"Flame2.spr",
	"Flame3.spr",
};

Relic::Relic(Engine * engine, Game * game)
  :Actor(engine, game), m_currentState(0), m_prevState(0), m_z(fp12(0)),
   m_zspeed(0), m_onAltar(-1), m_enabled(true), m_charging(false), m_computePower(true)
{
	m_sprite = m_engine->getAvailableSprite();
	m_sprite->setAnimation(true);
	m_flameSprite = m_engine->getAvailableSprite();
	m_flameSprite->setAnimation(true);
	m_flameSprite->setPriority(64);
	m_flameSprite->setVisibility(false);
	m_shadow = m_engine->getAvailableSprite16c();
	m_shadow->setAlpha(true);
	m_shadow->setSprite(5);
	m_idleState = new RelicIdleState(this);
	m_carriedState = new RelicCarriedState(this);
	m_chasingState = new RelicChasingState(this);
	m_flyingState = new RelicFlyingState(this);
	setOrigin(64 / 2, 64 - 10);
	setState(getCarriedState());
	m_currentForm = INERT;
	m_engine->log("Relic created");
}

Relic::~Relic()
{
	if (m_sprite)
		m_engine->releaseSprite(m_sprite);
	if (m_idleState)
		delete m_idleState;
	if (m_carriedState)
		delete m_carriedState;
	if (m_chasingState)
		delete m_chasingState;
	if (m_flyingState)
		delete m_flyingState;
}

void
Relic::setState(RelicState *state)
{
	if (m_currentState)
		m_currentState->leave();
	m_prevState = m_currentState;
	m_currentState = state;
	m_currentState->enter();
}

void
Relic::setPosition(int12 x, int12 y)
{
	Level * level = m_game->getLevel();
	if (round12(x) > 0 && round12(x) < level->getWidth() &&
		round12(y) > 0 && round12(y) < level->getHeight())
	{
		Entity::setPosition(x,y);
		int xt, xb, yt, yb;
		xt = m_game->world2camTopX(round12(m_x)) - m_xOrigin;
		xb = m_game->world2camBottomX(round12(m_x)) - m_xOrigin;
		yt = m_game->world2camTopY(round12(m_y)) - m_yOrigin - round12(m_z);
		yb = m_game->world2camBottomY(round12(m_y)) - m_yOrigin - round12(m_z);
		if (m_enabled) {
			m_sprite->setPosition(xt, yt, xb, yb);
			if ( m_onAltar == -1 )
				m_flameSprite->setPosition(xt, yt, xb, yb);
			else
				m_flameSprite->setPosition(256, 192, 256, 192);
			char tile = m_game->getLevel()->getTileType(round12(m_x), round12(m_y));
			if ( tile == VOID || tile == WATER ) {
				m_shadow->hide();
			} else {
				m_shadow->show();
				m_shadow->setPosition(m_game->world2camTopX(round12(m_x)) - m_xOrigin + 16,
						m_game->world2camTopY(round12(m_y)) - m_yOrigin + 40,
					m_game->world2camBottomX(round12(m_x)) - m_xOrigin + 16,
					m_game->world2camBottomY(round12(m_y)) - m_yOrigin + 40);
				int shadow_size; // 0 = smallest, 5 = normal
				shadow_size = (50 - round12(m_z))/10;
				if ( shadow_size < 0 )
					shadow_size = 0;
				m_shadow->setSprite(shadow_size);
			}
		} else {
			m_sprite->setPosition(256, 192, 256, 192); // put the sprite off screen
			m_flameSprite->setPosition(xt, yt+5, xb, yb+5);
			m_shadow->hide();
		}
	}
}

void
Relic::throwAt(int x, int y)
{
	Hero* hero = m_game->getHero();
	int12 diffx, diffy, dist;
	
	diffx = sub12(fp12(x), hero->getX());
	diffy = sub12(fp12(y), hero->getY());
	dist = distance12(diffx, diffy);
	m_dirX = div12(diffx, dist);
	m_dirY = div12(diffy, dist);

	if (round12(dist) < 24)
	  {
		m_speed = getMinSpeed();
		m_zspeed = fp12(1);
		setState(getFlyingState());		
	  }
	else
	  {
		dist = sub12(dist, fp12(16));  //these values are arbitrary
		dist = min12(dist, fp12(96));  //find your own to get what you want
		m_speed = div12(mul12(dist, getMaxSpeed()), fp12(96));
		m_zspeed = fp12(6);
		setState(getFlyingState());
	  }
}

void
Relic::update()
{
	m_currentState->update();
	this->setPosition(m_x, m_y); //refresh sprites
	if ( m_currentForm == MAGIC ) {
		if ( m_power == 0 && m_maxpower != 0 ) {
			m_flameSprite->setVisibility(false);
			m_flameSprite->setPosition(256, 192, 256, 192);
			m_game->gameOver();
		} else {
			if ( m_power % (m_maxpower/3) == 0 || m_power >= m_lastpower || m_computePower ) { // change flame sprite
				int flame = m_power / (m_maxpower/3) - 1;
				if ( m_computePower )
					flame++;
				m_computePower = false;
				if ( flame > 2 || flame < 0 || m_maxpower == 0 )
					flame = 2;
				m_flameSprite->load(m_flameSpriteFiles[flame]);
				m_flameSprite->setVisibility(true);
				if ( m_power != m_maxpower && m_power < m_lastpower )
					BufferedSoundPlay(SND_RELIC_CHARGE);
			}
		}
		m_lastpower = m_power--;
	}
}

void
Relic::setCharging(bool value)
{
  m_charging = value;

  if (value) {
	  //m_sprite->load("RelicAltarCharging.spr");
	m_sprite->setFrame(0);
  }
}

