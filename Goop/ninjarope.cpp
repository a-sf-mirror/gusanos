#include "ninjarope.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#include "part_type.h"
#include "sprite.h"
#include "base_animator.h"
#include "animators.h"
#include "vec.h"

#include <vector>

using namespace std;

NinjaRope::NinjaRope(PartType *type)
{
	justCreated = false;
	active = false;
	attached = false;
	m_type = type;
	
	m_angle = 0;
	m_angleSpeed = 0;
	m_animator = NULL;
	
	m_sprite = m_type->sprite;
	if ( m_sprite )
	{
		switch ( m_type->animType )
		{
			case PartType::ANIM_PINGPONG : 
				m_animator = new AnimPingPong(m_sprite,m_type->animDuration); break;
			
			case PartType::ANIM_LOOPRIGHT : 
				m_animator = new AnimLoopRight(m_sprite,m_type->animDuration); break;
		}
	}
	
	for ( vector< TimerEvent* >::iterator i = m_type->timer.begin(); i != m_type->timer.end(); i++)
	{
		timer.push_back( NRTimer(*i) );
	}
}

void NinjaRope::shoot(Vec _pos, Vec _spd)
{
	pos = _pos;
	spd = _spd;
	
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
};

void NinjaRope::think()
{
	if ( active )
	for ( int i = 0; i < m_type->repeat; ++i)
	{
		
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
	}
}

float NinjaRope::getAngle()
{
	return m_angle;
}

void NinjaRope::addAngleSpeed( float speed )
{
	m_angleSpeed += speed;
}

int NinjaRope::getColour()
{
	return m_type->colour;
}

void NinjaRope::draw(BITMAP* where,int xOff, int yOff)
{
	if (active)
	{
		if (!m_sprite)
			putpixel(where,(int)(pos.x)-xOff,(int)(pos.y)-yOff,m_type->colour);
		else
		{
			if ( m_angle < 180 )
			{
				m_sprite->drawAngled(where, m_animator->getFrame(), static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff), m_angle);
			}else
			{
				m_sprite->drawAngled(where, m_animator->getFrame(), static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff), 360-m_angle , true);
			}
		}
		if (m_type->distortion)
		{
			m_type->distortion->apply( where, static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff), m_type->distortMagnitud );
		}
	}
}