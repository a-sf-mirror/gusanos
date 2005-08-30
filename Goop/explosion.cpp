#include "explosion.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#include "base_worm.h"
#include "base_player.h"
#include "exp_type.h"
#include "events.h"
#include "distortion.h"
#include "gfx.h"
#include "sprite.h"
#include "sprite_set.h"
#include "base_animator.h"
#include "animators.h"

#include <vector>

using namespace std;

Explosion::Explosion(ExpType *type, const Vec& _pos, BasePlayer* owner) : BaseObject(owner,1)
{
	justCreated = true;
	m_type = type;
	
	pos = _pos;
	
	m_time = 0;
	
	m_alpha = m_type->alpha;
	
	m_timeout = m_type->timeout + (int)( rnd() * (m_type->timeoutVariation+1) ); 
	// I add +1 or variation will always be 0 if the value of timeout variation is 1
	
	if ( m_type->destAlpha >= 0 )
	{
		m_fadeSpeed = ( m_type->destAlpha - m_alpha ) / m_timeout;
	}else m_fadeSpeed = 0;
	
	
	if ( m_sprite = m_type->sprite )
	{
		m_animator = new AnimLoopRight( m_sprite, m_timeout+1);
	}
	else m_animator = NULL;
	
}

void Explosion::think()
{
	if ( justCreated )
	{	
		justCreated = false;
		
		if ( m_type->creation )
		{
			m_type->creation->run(this);
		}
	
		for ( vector< DetectEvent* >::iterator t = m_type->detectRanges.begin(); t != m_type->detectRanges.end(); ++t )
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
	}
	
	// Animation
	if ( m_animator ) m_animator->tick();
	
	// Alpha Fade
	if ( m_type->blender != NONE && m_fadeSpeed )
	{
		if ( fabs( m_type->destAlpha - m_alpha ) < fabs(m_fadeSpeed) )
		{
			m_fadeSpeed = 0;
			m_alpha = m_type->destAlpha;
		}else
			m_alpha += m_fadeSpeed;
	}
	
	++m_time;
	if ( m_time > m_timeout )
	{
		deleteMe = true;
	}
}

void Explosion::draw(BITMAP* where,int xOff, int yOff)
{
	if (!m_sprite)
	{
		if ( m_type->blender != NONE )
		{
			gfx.setBlender( m_type->blender, (int)m_alpha );
			putpixel(where,(int)(pos.x)-xOff,(int)(pos.y)-yOff,m_type->colour);
			solid_mode();
		}
		else putpixel(where,(int)(pos.x)-xOff,(int)(pos.y)-yOff,m_type->colour);
	}
	else
	{	
		if ( false )
		{
			if ( m_type->blender == NONE )
				m_sprite->getSprite(m_animator->getFrame(), 0)->draw(where, static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff));
			else
				m_sprite->getSprite(m_animator->getFrame(), 0)->drawBlended(where, static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff), (int)m_alpha, false, 0, m_type->blender);
		}
		else
		{
			if ( m_type->blender != NONE )
				gfx.setBlender( m_type->blender, (int)m_alpha );
			game.level.specialDrawSprite( m_sprite->getSprite(m_animator->getFrame(), 0), where, pos - Vec(xOff, yOff), pos );
			solid_mode();
		}
	}
	if (m_type->distortion)
	{
		m_type->distortion->apply( where, static_cast<int>(pos.x-xOff), static_cast<int>(pos.y-yOff), m_type->distortMagnitude );
	}
}
