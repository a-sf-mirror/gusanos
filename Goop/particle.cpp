#include "particle.h"

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

Particle::Particle(PartType *type, Vec _pos, Vec _spd)
{
	justCreated = true;
	m_type = type;
	
	pos = _pos;
	spd = _spd;
	m_angle = spd.getAngle();
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
		timer.push_back( PartTimer(*i) );
	}
}

void Particle::think()
{
	for ( int i = 0; i < m_type->repeat; ++i)
	{
		spd.y+=m_type->gravity;
		
		bool collision = false;
		if ( !game.level.getMaterial( (int)(pos+spd).x, (int)pos.y ).particle_pass)
		{
			spd.x*=-m_type->bounceFactor;
			collision = true;
		}
		if ( !game.level.getMaterial( (int)pos.x, (int)(pos+spd).y ).particle_pass)
		{
			spd.y*=-m_type->bounceFactor;
			collision = true;
		}
		if( collision )
		{
			if ( m_type->groundCollision != NULL )
					m_type->groundCollision->run(this);
		}
		
		for ( vector< PartTimer >::iterator t = timer.begin(); t != timer.end(); t++)
		{
			(*t).count--;
			if ( (*t).count < 0 )
			{
				(*t).m_tEvent->event->run(this);
				(*t).reset();
			}
		}
		
		if ( justCreated && m_type->creation )
		{
			m_type->creation->run(this);
			justCreated = false;
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

float Particle::getAngle()
{
	return m_angle;
}

void Particle::addAngleSpeed( float speed )
{
	m_angleSpeed += speed;
}

void Particle::draw(BITMAP* where,int xOff, int yOff)
{
	if (!m_sprite)
		putpixel(where,(int)(pos.x)-xOff,(int)(pos.y)-yOff,m_type->colour);
	else
	{
		m_sprite->draw(where, m_animator->getFrame(), (int)pos.x-xOff, pos.y-yOff);
	}
	if (m_type->distortion)
	{
		m_type->distortion->apply( where, pos.x-xOff, pos.y-yOff, m_type->distortMagnitud );
	}
}
