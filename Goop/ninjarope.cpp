#include "ninjarope.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#include "part_type.h"
#include "sprite_set.h"
#include "sprite.h"
#include "base_animator.h"
#include "animators.h"
#include "vec.h"
#include "part_type.h"

#include <vector>
#include <boost/variant/apply_visitor.hpp>

using namespace std;

NRTimer::NRTimer( TimerEvent* tEvent)
{
	m_tEvent = tEvent;
	count = m_tEvent->delay + (int)(rnd() * m_tEvent->delayVariation);
}

void NRTimer::reset()
{
	count = m_tEvent->delay + (int)(rnd() * m_tEvent->delayVariation);
}

NinjaRope::NinjaRope(PartType *type, BaseObject* worm)
: m_worm(worm)
{
	justCreated = false;
	active = false;
	attached = false;
	m_type = type;
	
	m_angle = 0;
	m_angleSpeed = 0;
	//m_animator = NULL;
	
	m_sprite = m_type->sprite;
	/*
	if ( m_sprite )
	{
		switch ( m_type->animType )
		{
			case PartType::ANIM_PINGPONG : 
				m_animator = new AnimPingPong(m_sprite,m_type->animDuration); break;
			
			case PartType::ANIM_LOOPRIGHT : 
				m_animator = new AnimLoopRight(m_sprite,m_type->animDuration); break;
		}
	}*/
	m_animator = m_type->allocateAnimator();
		
	// Why this?? :OO // Re: Modders may want to make the rope leave trails or sth :o
	for ( vector< TimerEvent* >::iterator i = m_type->timer.begin(); i != m_type->timer.end(); i++)
	{
		timer.push_back( NRTimer(*i) );
	}
}

void NinjaRope::shoot(Vec _pos, Vec _spd)
{
	pos = _pos;
	spd = _spd;
	m_length = game.options.ninja_rope_startDistance;
	
	justCreated = true;
	active = true;
	attached = false;
	
	m_angle = spd.getAngle();
	m_angleSpeed = 0;
	
	for ( vector< NRTimer >::iterator t = timer.begin(); t != timer.end(); t++)
	{
		(*t).reset();
	}
}

void NinjaRope::remove()
{
	active = false;
	justCreated = false;
	attached = false;
}

void NinjaRope::think()
{
	if (!active)
		return;
		
	if ( justCreated && m_type->creation )
	{
		m_type->creation->run(this);
		justCreated = false;
	}
	
	for ( int i = 0; !deleteMe && i < m_type->repeat; ++i)
	{
		pos += spd;
		
		BaseVec<long> ipos(pos);
		
		// TODO: Try to attach to worms/objects
				
		Vec diff(m_worm->pos, pos);
		float curLen = diff.length();
		Vec force(diff * game.options.ninja_rope_pullForce);
		
		/*
		if(<attached to object>)
		{
			//Apply force to object
		}
		else
		*/
		if(!game.level.getMaterial( ipos.x, ipos.y ).particle_pass)
		{
			if(!attached)
			{
				m_length = 450.f / 16.f - 1.0f;
				attached = true;
				spd.zero();
				if ( m_type->groundCollision != NULL )
					m_type->groundCollision->run(this);
			}
		}
		else
			attached = false;
			
		if(attached)
		{
			if(curLen > m_length)
			{
				m_worm->addSpeed(force / curLen);
			}
		}
		else
		{
			spd.y += m_type->gravity;
			
			if(curLen > m_length)
			{
				spd -= force / curLen;
			}
		}
		
/*
		if ( m_animator )
			m_animator->tick();
		*/
		m_animator->tick();
		
		/* OLD CODE
		if ( justCreated && m_type->creation )
		{
			m_type->creation->run(this);
			justCreated = false;
		}
		
		if( !game.level.getMaterial( (int)(pos+spd).x, (int)(pos+spd).y ).particle_pass )
		{
			if (!attached)
			{
				attached = true;
				pos = pos + spd;
				spd *= 0;
				if ( m_type->groundCollision != NULL )
						m_type->groundCollision->run(this);
			}
		}else attached = false;
		
		if (!attached)
		{
			spd.y+=m_type->gravity;
			
			for ( vector< NRTimer >::iterator t = timer.begin(); t != timer.end(); t++)
			{
				(*t).count--;
				if ( (*t).count < 0 )
				{
					(*t).m_tEvent->event->run(this);
					(*t).reset();
				}
			}
			
			if ( m_type->acceleration )
			{
				if ( spd.dotProduct(angleVec(m_angle,1)) < m_type->maxSpeed || m_type->maxSpeed < 0)
				spd+= angleVec(m_angle,m_type->acceleration);
			}
			
			spd*=m_type->damping;
			
			if ( abs(m_angleSpeed) < m_type->angularFriction ) m_angleSpeed = 0;
			else if ( m_angleSpeed < 0 ) m_angleSpeed += m_type->angularFriction;
			else m_angleSpeed -= m_type->angularFriction;
			
			m_angle += m_angleSpeed;
			while ( m_angle > 360 ) m_angle -= 360;
			while ( m_angle < 0 ) m_angle += 360;
			
			
			if ( !deleteMe ) pos = pos + spd;
			else break;
			if ( m_animator ) m_animator->tick();
		}
		*/
	}
}

Angle NinjaRope::getAngle()
{
	return m_angle;
}

void NinjaRope::addAngleSpeed( AngleDiff speed )
{
	m_angleSpeed += speed;
}

int NinjaRope::getColour()
{
	return m_type->colour;
}

Vec& NinjaRope::getPosReference()
{
	return pos;
}

void NinjaRope::draw(BITMAP* where,int xOff, int yOff)
{
	if (active)
	{
		if (!m_sprite)
			putpixel(where,(int)(pos.x)-xOff,(int)(pos.y)-yOff,m_type->colour);
		else
		{
			if ( m_angle < Angle(180.0) )
			{
				m_sprite->getSprite(m_animator->getFrame(), m_angle)->draw(where,static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff));
			}
			else
			{
				m_sprite->getSprite(m_animator->getFrame(), -m_angle)->draw(where, static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff), true);
			}
		}
		if (m_type->distortion)
		{
			m_type->distortion->apply( where, static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff), m_type->distortMagnitude );
		}
	}
}
