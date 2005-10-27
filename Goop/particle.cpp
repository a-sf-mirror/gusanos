#include "particle.h"

#include "game.h"
#include "base_object.h"
#include "base_worm.h"
#include "base_player.h"
#include "viewport.h"
#include "part_type.h"
#ifndef DEDSERV
#include "gfx.h"
#include "sprite.h"
#include "sprite_set.h"
#include "base_animator.h"
#include "blitters/blitters.h"
#endif
#include "glua.h"
#include "lua/bindings-objects.h"
#include "detect_event.h"
#include "noise_line.h"
#include "omfgutil_macros.h"
#include "omfgutil_math.h"



#include <vector>
#include <iostream>
#define BOOST_NO_MT
#include <boost/pool/pool.hpp>

using namespace std;

static boost::pool<> particlePool(sizeof(Particle));

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
: BaseObject(owner, pos_, spd_), m_dir(dir), m_type(type)
, m_health(type->health), m_angle(angle), m_angleSpeed(0)
#ifndef DEDSERV
, m_alpha(m_type->alpha), m_fadeSpeed(0), m_animator(0)
, m_alphaDest(255), m_sprite(m_type->sprite)
#endif
, m_origin(pos_)
{
	m_angle.clamp();
	
#ifndef DEDSERV
	if ( m_sprite )
	{
		m_animator = m_type->allocateAnimator();
	}
#endif
	// If it's deleted, we must allocate animator so that we can render it,
	// but we don't need any timers.
	
	if ( m_type->creation )
	{
		m_type->creation->run(this);
		if(deleteMe)
			return;
	}
	
	//for ( vector< TimerEvent* >::iterator i = m_type->timer.begin(); i != m_type->timer.end(); i++)
	foreach(i, m_type->timer)
	{
		timer.push_back( (*i)->createState() );
	}
	
	
}

Particle::~Particle()
{
#ifndef DEDSERV
	delete m_animator;
#endif
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
		// TODO: Remove this TODO
		if ( m_type->acceleration )
		{
			Vec dir(m_angle);
			if ( m_type->maxSpeed < 0 || spd.dotProduct(dir) < m_type->maxSpeed )
				spd += dir * m_type->acceleration;
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
#ifndef DEDSERV
			if ( !m_type->animOnGround && m_animator )
				m_animator->freeze(5); //I GOT DEFEATED!
#endif
		}

		//for ( vector< DetectEvent* >::iterator t = m_type->detectRanges.begin(); t != m_type->detectRanges.end(); ++t )
		foreach(t, m_type->detectRanges)
		{
			(*t)->check(this);
		}
		if ( deleteMe ) break;
		
		//for ( vector< TimerEvent::State* >::iterator t = timer.begin(); t != timer.end(); t++)
		foreach(t, timer)
		{
			if ( t->tick() )
			{
				t->event->run(this);
			}
			if ( deleteMe ) break;
		}
		
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
		
#ifndef DEDSERV
		// Animation
		if ( m_animator ) m_animator->tick();
		
		// Alpha Fade
		if ( ( m_type->blender || m_type->lightHax ) && m_fadeSpeed )
		{
			if ( fabs(m_alphaDest - m_alpha) < fabs(m_fadeSpeed) )
			{
				m_fadeSpeed = 0;
				m_alpha = m_alphaDest;
			}
			else
				m_alpha += m_fadeSpeed;
		}
#endif
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

#ifndef DEDSERV
void Particle::setAlphaFade(int frames, int dest)
{
	m_fadeSpeed = ( dest - m_alpha ) / frames;
	m_alphaDest = dest;
}
#endif

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

#ifndef DEDSERV

void Particle::drawLine2Origin( Viewport* viewport, BlitterContext const& blitter)
{
	if(m_type->wupixels)
	{
		Vec rPos = viewport->convertCoordsPrec( pos );
		Vec rOPos = viewport->convertCoordsPrec( pos );
		blitter.linewu(viewport->dest, rOPos.x,rOPos.y,rPos.x,rPos.y, m_type->colour);
	}
	else
	{	
		IVec rPos = viewport->convertCoords( IVec(pos) );
		IVec rOPos = viewport->convertCoords( IVec(m_origin) );
		line(viewport->dest, rOPos.x,rOPos.y,rPos.x,rPos.y,m_type->colour); //TODO: Change to use blitter
		//mooo.createPath( 7, 7);
		//mooo.render(where, x,y,x2,y2, m_type->colour);
		//mooo.createPath( 5, 10);
		//mooo.render(where, x,y,x2,y2, m_type->colour);
	}
}

void Particle::draw(Viewport* viewport)
{

	IVec rPos = viewport->convertCoords( IVec(pos) );
	Vec rPosPrec = viewport->convertCoordsPrec( pos );
	BITMAP* where = viewport->dest;
	int x = rPos.x;
	int y = rPos.y;
	
	BlitterContext blitter(m_type->blender, (int)m_alpha);
	
	if (!m_sprite)
	{
		if(!m_type->invisible)
		{
			if(m_type->wupixels)
				blitter.putpixelwu(where, rPosPrec.x, rPosPrec.y, m_type->colour);
			else
				blitter.putpixel(where, x, y, m_type->colour);
		}
				
		if ( m_type->line2Origin )
			drawLine2Origin( viewport, blitter );
	}
	else
	{
		if ( !m_type->culled )
			m_sprite->getSprite(m_animator->getFrame(), m_angle)->draw(where, x, y, blitter);
		else
		{
			Sprite* renderSprite = m_sprite->getSprite(m_animator->getFrame(), m_angle);
			game.level.culledDrawSprite(renderSprite, viewport, IVec(pos), (int)m_alpha );
		}
	}
	if (m_type->distortion)
	{
		m_type->distortion->apply( where, x, y, m_type->distortMagnitude );
	}
	if ( m_type->lightHax )
	{
		game.level.culledDrawLight( m_type->lightHax, viewport, IVec(pos), (int)m_alpha );
	}
}
#endif

void Particle::pushLuaReference()
{
	lua.pushFullReference(*this, LuaBindings::particleMetaTable);
}
