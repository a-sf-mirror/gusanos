#include "base_worm.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#include "base_player.h"
#include "player_options.h"
#include "base_animator.h"
#include "animators.h"
#include "sprite.h"
#include "weapon.h"
#include "ninjarope.h"

#include <math.h>

BaseWorm::BaseWorm()
{
	skin = spriteList.load("skin.png");
	m_animator = new AnimLoopRight(skin,35);
	
	do
	{
		pos.x = rnd()*game.level.width();
		pos.y = rnd()*game.level.height();
	} while ( !game.level.getMaterial((int) pos.x,(int) pos.y).particle_pass );
	
	dir = 1;
	aimAngle = 90;
	aimSpeed = 0;
	aimRecoilSpeed = 0;	
	
	currentWeapon = 0;
	m_weapons.push_back(new Weapon(game.weaponList[0], this));
	
	m_owner = NULL;
	m_ninjaRope = new NinjaRope(game.NRPartType);
	movingLeft = false;
	movingRight = false;
	jumping = false;
}

BaseWorm::~BaseWorm()
{
	if (m_animator) delete m_animator;
	for ( size_t i = 0; i < m_weapons.size(); i++)
	{
		delete m_weapons[i];
	}
}

void BaseWorm::assignOwner( BasePlayer* owner)
{
	m_owner = owner;
}

NinjaRope* BaseWorm::getNinjaRopeObj()
{
	return m_ninjaRope;
}

void BaseWorm::think()
{
	
	spd.y+=game.options.worm_gravity;
	
	if ( m_ninjaRope->attached && (m_ninjaRope->getPos() - pos).length() > currentRopeLength)
	{
		spd += (m_ninjaRope->getPos() - pos).normal() * game.options.ninja_rope_pullForce;
	}

	if ( movingRight ) 
	{
		if ( spd.x < game.options.worm_maxSpeed )
		{
			if (game.level.getMaterial( (int)pos.x, (int)(pos.y + spd.y + 1) ).particle_pass)
				spd.x += game.options.worm_acceleration * game.options.worm_airAccelerationFactor;
			else
				spd.x += game.options.worm_acceleration;
		}
		dir = 1;
	}
	
	if ( movingLeft ) 
	{
		if ( -spd.x < game.options.worm_maxSpeed )
		{
			if (game.level.getMaterial( (int)pos.x, (int)(pos.y + spd.y + 1) ).particle_pass)
				spd.x -= game.options.worm_acceleration * game.options.worm_airAccelerationFactor;
			else
				spd.x -= game.options.worm_acceleration;
		}
		dir = -1;
	}
	
	
	// Bottom collision
	Material g = game.level.getMaterial( (int)pos.x, (int)(pos.y + spd.y) );
	if (!g.particle_pass && spd.y > 0)
	{
		// Floor friction;
		if ( fabs(spd.x) < game.options.worm_friction ) spd.x=0;
		if ( spd.x < 0 ) spd.x += game.options.worm_friction;
		if ( spd.x > 0 ) spd.x += -game.options.worm_friction;
			
		if ( spd.y < game.options.worm_bounceLimit ) spd.y=0;
		else
		{
			spd.y*=-game.options.worm_bounceQuotient;
		}
	}
	
	// Top collision
	g = game.level.getMaterial( (int)pos.x, (int)(pos.y + spd.y - game.options.worm_height) );
	if (!g.particle_pass && spd.y < 0)
	{
		// Roof friction;
		if ( fabs(spd.x) < game.options.worm_friction ) spd.x=0;
		if ( spd.x < 0 ) spd.x += game.options.worm_friction;
		if ( spd.x > 0 ) spd.x += -game.options.worm_friction;
			
		if ( -spd.y < game.options.worm_bounceLimit ) spd.y=0;
		else
		{
			spd.y*=-game.options.worm_bounceQuotient;
		}
	}
	
	//Side collisions and climbing
	//int o = 0;
	int upper = -1;
	int lower = -1;
	for ( int i = 0; i < game.options.worm_height; i++ )
	{
		if (!game.level.getMaterial( (int)(pos.x + spd.x), (int)(pos.y - i) ).particle_pass) lower = i;
	}
	for ( int i = 0; i <= game.options.worm_height; i++ )
	{
		if (!game.level.getMaterial( (int)(pos.x + spd.x), (int)(pos.y - game.options.worm_height + i) ).particle_pass) upper = i;
	}
	
	// Floor climb
	if (lower <= game.options.worm_maxClimb && lower != -1 && !spd.x == 0)
	{
		pos.y -= 1;
	}
	
	// Roof climb
	if (upper <= game.options.worm_maxClimb && upper != -1 && !spd.x == 0)
	{
		pos.y += 1;
	}
	
	if ( lower >= game.options.worm_maxClimb / 2 )
	{
		if ( fabs( spd.x ) > game.options.worm_bounceLimit )
		{
			spd.x *= -game.options.worm_bounceQuotient;
		}
		else spd.x = 0;
	}
	
	if ( upper == -1 && lower == -1 )
		pos.x += spd.x;
	pos.y += spd.y;
	
	if ( m_owner )
	{
		if ( fabs(aimSpeed) < m_owner->getOptions()->aimFriction ) aimSpeed = 0;
		else if ( aimSpeed > 0 ) aimSpeed -= m_owner->getOptions()->aimFriction;
		else if ( aimSpeed < 0 ) aimSpeed += m_owner->getOptions()->aimFriction;
	}
	aimAngle += aimSpeed;

	if( aimAngle < 0 )
	{
		aimAngle = 0;
		aimSpeed = 0;
	}
	if( aimAngle > 180 )
	{
		aimAngle = 180;
		aimSpeed = 0;
	}
		
	if ( movingLeft || movingRight ) m_animator->tick();
		
	// Make weapons think
	for ( size_t i = 0; i < m_weapons.size(); ++i )
	{
		m_weapons[i]->think();
	}
	
}

Vec BaseWorm::getPos()
{
	return pos - Vec(0,game.options.worm_weaponHeight+0.5);
}

Vec BaseWorm::getRenderPos()
{
	return renderPos - Vec(0,game.options.worm_weaponHeight+0.5);
}

float BaseWorm::getAngle()
{
	return aimAngle*dir;
}

char BaseWorm::getDir()
{
	return dir;
}

void BaseWorm::draw(BITMAP* where,int xOff, int yOff)
{
	int y = (int)renderPos.y - game.options.worm_weaponHeight; // The weapon is the center of the worm render
	bool flipped = false;
	if ( dir < 0 ) flipped = true;
	for(int i = 0; i< 10; i++)
	{
		Vec crosshair = angleVec(aimAngle*dir,rnd()*10+30) + renderPos - Vec(xOff,yOff + game.options.worm_weaponHeight);
		putpixel(where, static_cast<int>( crosshair.x ), static_cast<int>(crosshair.y), makecol(255,0,0));
	}
	if (m_ninjaRope->active) line(where, static_cast<int>(renderPos.x-xOff), static_cast<int>(y-yOff), static_cast<int>(m_ninjaRope->getPos().x-xOff), static_cast<int>(m_ninjaRope->getPos().y-yOff), m_ninjaRope->getColour());
	skin->drawAngled(where, m_animator->getFrame(), (int)renderPos.x-xOff, y-yOff,aimAngle, flipped);
}

void BaseWorm::addAimSpeed( float speed )
{
	if ( m_owner )
	if ( fabs( aimSpeed ) < m_owner->getOptions()->aimMaxSpeed )
	aimSpeed += speed;
}

void BaseWorm::addRopeLength( float distance )
{
	currentRopeLength += distance;
	if ( currentRopeLength < 0 ) currentRopeLength = 0;
}

void BaseWorm::actionStart( Actions action)
{
	switch ( action )
	{
		case MOVELEFT:
			movingLeft = true;
		break;
		
		case MOVERIGHT:
			movingRight = true;
		break;
		
		case FIRE:
			m_weapons[0]->actionStart( Weapon::PRIMARY_TRIGGER );
		break;
		
		case JUMP:
			if ( !game.level.getMaterial( (int)pos.x, (int)(pos.y + spd.y) + 1 ).particle_pass)
			{
				spd.y -= game.options.worm_jumpForce;
			}
			jumping = true;
		break;
			
		case NINJAROPE:
			m_ninjaRope->shoot(pos-Vec(0,game.options.worm_weaponHeight+0.5), angleVec(aimAngle*dir,game.options.ninja_rope_shootSpeed));
			currentRopeLength = game.options.ninja_rope_startDistance;
		break;
		
	}
}

void BaseWorm::actionStop( Actions action)
{
	switch ( action )
	{
		case MOVELEFT:
			movingLeft = false;
		break;
		
		case MOVERIGHT:
			movingRight = false;
		break;
		
		case FIRE:
			m_weapons[0]->actionStop( Weapon::PRIMARY_TRIGGER );
		break;
		
		case JUMP:
			jumping = false;
		break;
		
		case NINJAROPE:
			m_ninjaRope->remove();
		break;
	}
}


