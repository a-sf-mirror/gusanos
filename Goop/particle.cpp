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

#include <vector>

using namespace std;

Particle::Particle(PartType *type, Vec _pos, Vec _spd, int dir, BasePlayer* owner) : BaseObject(owner,dir)
{
	justCreated = true;
	m_type = type;
	
	pos = _pos;
	spd = _spd;
	m_angle = spd.getAngle();
	m_angleSpeed = 0;
	m_animator = NULL;
	
	m_alpha = m_type->alpha;
	m_fadeSpeed = 0;
	m_alphaDest = 255;
	
	m_sprite = m_type->sprite;
	if ( m_sprite )
	{
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
		
		if ( m_type->acceleration )
		{
			if ( spd.dotProduct(angleVec(m_angle,1)) < m_type->maxSpeed || m_type->maxSpeed < 0)
				spd+= angleVec(m_angle,m_type->acceleration);
		}
		
		spd*=m_type->damping;
		
		bool collision = false;
		if ( !game.level.getMaterial( (int)(pos.x+spd.x), (int)pos.y ).particle_pass)
		{
			spd.x *= -m_type->bounceFactor;
			spd.y *= m_type->groundFriction;
			collision = true;
		}
		if ( !game.level.getMaterial( (int)pos.x, (int)(pos.y+spd.y) ).particle_pass)
		{
			spd.y*=-m_type->bounceFactor;
			spd.x*=m_type->groundFriction;
			collision = true;
		}
		if( collision )
		{
			if ( m_type->groundCollision != NULL )
					m_type->groundCollision->run(this);
			if ( !m_type->animOnGround && m_animator )
				m_animator->freeze(5); //I GOT DEFEATED!
		}
		if ( deleteMe ) break;
		
		for ( vector< WormDetectEvent* >::iterator t = m_type->detectRanges.begin(); t != m_type->detectRanges.end(); ++t )
		{
			ObjectsList::ColLayerIterator worm;
			for ( worm = game.objects.colLayerBegin(WORMS_COLLISION_LAYER); (bool)worm; ++worm)
			{
				if ( (*t)->m_detectOwner || (*worm)->getOwner() != m_owner )
				if ( (*worm)->isCollidingWith(pos, (*t)->m_range) )
				{
					(*t)->event->run( this,(*worm), dynamic_cast<BaseWorm*>( (*worm) ));
				}
			}
		}
		if ( deleteMe ) break;
		
		for ( vector< PartTimer >::iterator t = timer.begin(); t != timer.end(); t++)
		{
			if ( (*t).tick() )
			{
				(*t).m_tEvent->event->run(this);
			}
			if ( deleteMe ) break;
		}
		if ( deleteMe ) break;
		
		if ( justCreated && m_type->creation )
		{
			m_type->creation->run(this);
			justCreated = false;
		}
		if ( deleteMe ) break;

		
		if ( abs(m_angleSpeed) < m_type->angularFriction ) m_angleSpeed = 0;
		else if ( m_angleSpeed < 0 ) m_angleSpeed += m_type->angularFriction;
		else m_angleSpeed -= m_type->angularFriction;
		
		m_angle += m_angleSpeed;
		while ( m_angle > 360 ) m_angle -= 360;
		while ( m_angle < 0 ) m_angle += 360;
		
		//Position update
		pos = pos + spd;
		
		// Animation
		if ( m_animator ) m_animator->tick();
		
		// Alpha Fade
		if ( m_type->blender != NONE && m_fadeSpeed )
		{
			if ( fabs(m_alphaDest-m_alpha) < fabs(m_fadeSpeed) )
			{
				m_fadeSpeed = 0;
				m_alpha = m_alphaDest;
			}else
				m_alpha += m_fadeSpeed;
		}
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

void Particle::draw(BITMAP* where,int xOff, int yOff)
{
	if (!m_sprite)
		if ( m_type->blender != NONE )
		{
			gfx.setBlender( m_type->blender, (int)m_alpha );
			putpixel(where,(int)(pos.x)-xOff,(int)(pos.y)-yOff,m_type->colour);
			solid_mode();
		}
		else putpixel(where,(int)(pos.x)-xOff,(int)(pos.y)-yOff,m_type->colour);
	else
	{
		if ( m_angle < 180 )
		{
			if ( m_type->blender == NONE )
				m_sprite->getSprite(m_animator->getFrame(), m_angle)->draw(where, static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff));
			else
				m_sprite->getSprite(m_animator->getFrame(), m_angle)->drawBlended(where, static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff), (int)m_alpha, false, 0, m_type->blender);
		}else
		{
			if ( m_type->blender == NONE )
				m_sprite->getSprite(m_animator->getFrame(), 360-m_angle)->draw(where, (int)pos.x-xOff, (int)pos.y-yOff, true);
			else
				m_sprite->getSprite(m_animator->getFrame(), 360-m_angle)->drawBlended(where, static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff), (int)m_alpha, true, 0, m_type->blender);
		}
	}
	if (m_type->distortion)
	{
		m_type->distortion->apply( where, static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff), m_type->distortMagnitude );
	}
}
