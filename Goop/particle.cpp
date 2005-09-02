#include "particle.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#include "base_worm.h"
#include "base_player.h"
#include "part_type.h"
#include "gfx.h"
#include "sprite.h"
#include "sprite_set.h"
#include "base_animator.h"
#include "animators.h"
#include "glua.h"
#include "lua/bindings.h"
#include "detect_event.h"

#include <vector>
#define BOOST_NO_MT
#include <boost/pool/pool.hpp>

using namespace std;

boost::pool<> particlePool(sizeof(Particle));

void* Particle::operator new(size_t count)
{
	assert(count <= sizeof(Particle));
	return particlePool.malloc();
}

void Particle::operator delete(void* block)
{
	particlePool.free(block);
}

Particle::Particle(PartType *type, Vec pos_, Vec spd_, int dir, BasePlayer* owner, Angle angle)
: BaseObject(owner, dir, pos_, spd_), justCreated(true), m_type(type)
, m_health(type->health), m_angle(angle), m_angleSpeed(0)
, m_alpha(m_type->alpha), m_fadeSpeed(0), m_animator(0)
, m_alphaDest(255), m_sprite(m_type->sprite)
{
/*
	justCreated = true;
	m_type = type;
	
	m_health = type->health;
	pos = _pos;
	spd = _spd;
	m_angle = spd.getAngle();
	m_angleSpeed = 0;
	m_animator = NULL;
	
	m_alpha = m_type->alpha;
	m_fadeSpeed = 0;
	m_alphaDest = 255;
	
	m_sprite = m_type->sprite;*/
	
	if ( m_sprite )
	{
		// TODO: This is overheadish, use some kind of union?
		switch ( m_type->animType )
		{
			case PartType::ANIM_PINGPONG : 
				m_animator = new AnimPingPong(m_sprite,m_type->animDuration); break;
			
			case PartType::ANIM_LOOPRIGHT : 
				m_animator = new AnimLoopRight(m_sprite,m_type->animDuration); break;
				
			case PartType::ANIM_RIGHTONCE : 
				m_animator = new AnimRightOnce(m_sprite,m_type->animDuration); break;
		}
	}
	
	// TODO: This is also overheadish
	for ( vector< TimerEvent* >::iterator i = m_type->timer.begin(); i != m_type->timer.end(); i++)
	{
		timer.push_back( PartTimer(*i) );
	}
}

void Particle::think()
{
	for ( int i = 0; i < m_type->repeat; ++i)
	{
		if ( m_health <= 0 && m_type->death )
		{
			m_type->death->run(this);
			m_health = m_type->health;
			if ( deleteMe ) break;
		}
	
		spd.y += m_type->gravity;
		
		if ( m_type->acceleration )
		{
			// TODO: Cache the angle vector?
			if ( m_type->maxSpeed < 0 || spd.dotProduct(Vec(m_angle)) < m_type->maxSpeed )
				spd += Vec(m_angle, (double)m_type->acceleration); //angleVec(m_angle, m_type->acceleration);
		}
				
		spd *= m_type->damping;
		
		bool collision = false;
		if ( !game.level.getMaterial( (int)(pos.x + spd.x), (int)pos.y ).particle_pass)
		{
			spd.x *= -m_type->bounceFactor; // TODO: Precompute the negative of this
			spd.y *= m_type->groundFriction;
			collision = true;
		}
		if ( !game.level.getMaterial( (int)pos.x, (int)(pos.y + spd.y) ).particle_pass)
		{
			spd.y *= -m_type->bounceFactor; // TODO: Precompute the negative of this
			spd.x *= m_type->groundFriction;
			collision = true;
		}
		if( collision )
		{
			if ( m_type->groundCollision )
			{
				m_type->groundCollision->run(this);
				if ( deleteMe ) break;
			}
			if ( !m_type->animOnGround && m_animator )
				m_animator->freeze(5); //I GOT DEFEATED!
		}
		// "if ( deleteMe ) break;" moved from here because the flag can only be set if an event is ran
		
		for ( vector< DetectEvent* >::iterator t = m_type->detectRanges.begin(); t != m_type->detectRanges.end(); ++t )
		{
			(*t)->check(this);
		}
		if ( deleteMe ) break;
		
		for ( vector< PartTimer >::iterator t = timer.begin(); t != timer.end(); t++)
		{
			if ( t->tick() )
			{
				t->m_tEvent->event->run(this);
			}
			if ( deleteMe ) break;
		}
		//if ( deleteMe ) break; // Useless
		
		if ( justCreated && m_type->creation ) // TODO: 
		{
			m_type->creation->run(this);
			justCreated = false;
			if ( deleteMe ) break;
		}
		//if ( deleteMe ) break; // Useless

		
		if(m_type->angularFriction)
		{
			if ( abs(m_angleSpeed) < m_type->angularFriction ) m_angleSpeed = 0;
			else if ( m_angleSpeed < 0 ) m_angleSpeed += m_type->angularFriction;
			else m_angleSpeed -= m_type->angularFriction;
		}
		
		m_angle += m_angleSpeed;
		m_angle.clamp();
		
		//Position update
		pos = pos + spd;
		
		// Animation
		if ( m_animator ) m_animator->tick();
		
		// Alpha Fade
		if ( m_type->blender != NONE && m_fadeSpeed )
		{
			if ( fabs(m_alphaDest - m_alpha) < fabs(m_fadeSpeed) )
			{
				m_fadeSpeed = 0;
				m_alpha = m_alphaDest;
			}
			else
				m_alpha += m_fadeSpeed;
		}
	}
}

Angle Particle::getAngle()
{
	return m_angle;
}

void Particle::addAngleSpeed( AngleDiff speed )
{
	m_angleSpeed += speed;
}

void Particle::setAlphaFade(int frames, int dest)
{
	m_fadeSpeed = ( dest - m_alpha ) / frames;
	m_alphaDest = dest;
}

void Particle::customEvent( size_t index )
{
	if ( index < m_type->customEvents.size() && m_type->customEvents[index] )
	{
		m_type->customEvents[index]->run(this);
	}
}

void Particle::damage( float amount, BasePlayer* damager )
{
	m_health -= amount;
}

void Particle::draw(BITMAP* where, int xOff, int yOff)
{
	int x = static_cast<int>(pos.x - xOff);
	int y = static_cast<int>(pos.y - yOff);
	
	if (!m_sprite)
	{
		if ( m_type->blender != NONE )
		{
			gfx.setBlender( m_type->blender, (int)m_alpha ); // TODO: why isn't alpha int from the beginning
			putpixel(where, x, y, m_type->colour);
			solid_mode();
		}
		else putpixel(where, x, y, m_type->colour); //TODO: Use something faster than putpixel ?
	}
	else
	{
		if ( m_angle < Angle(180.0) )
		{
			if ( m_type->blender == NONE )
				m_sprite->getSprite(m_animator->getFrame(), m_angle)->draw(where, x, y);
			else
				m_sprite->getSprite(m_animator->getFrame(), m_angle)->drawBlended(where, x, y, (int)m_alpha, false, 0, m_type->blender);
		}
		else
		{
			if ( m_type->blender == NONE )
				m_sprite->getSprite(m_animator->getFrame(), -m_angle)->draw(where, x, y, true);
			else
				m_sprite->getSprite(m_animator->getFrame(), -m_angle)->drawBlended(where, x, y, (int)m_alpha, true, 0, m_type->blender);
		}
	}
	if (m_type->distortion)
	{
		m_type->distortion->apply( where, x, y, m_type->distortMagnitude );
	}
}

void Particle::pushLuaReference()
{
	lua.pushFullReference(*this, LuaBindings::particleMetaTable);
}
