#include "weapon.h"
#include "weapon_type.h"
#include "events.h"
#include "base_worm.h"
#include "game.h"

#include "network.h"

class BaseObject;

Weapon::Weapon(WeaponType* type, BaseWorm* owner)
{
	m_type = type;
	m_owner = owner;
	primaryShooting = false;
	ammo = type->ammo;
	inactiveTime = 0;
	reloading = false;
	reloadTime = 0;
}

Weapon::~Weapon()
{
}

void Weapon::reset()
{
	primaryShooting = false;
	ammo = m_type->ammo;
	inactiveTime = 0;
	reloading = false;
	reloadTime = 0;
}

void Weapon::think( bool isFocused, size_t index )
{
	if (inactiveTime > 0) inactiveTime--;
	else if ( isFocused )
	{
		if ( primaryShooting && ammo > 0)
		{
			if (m_type->primaryShoot) m_type->primaryShoot->run((BaseObject*)m_owner, NULL, NULL, this );
			ammo--;
		}
	}
	if ( isFocused )
	{
		if ( ammo <= 0 && !reloading && !network.isClient())
		{
			outOfAmmo();
			
			if ( network.isHost() )
			{
				ZCom_BitStream* data = new ZCom_BitStream;
				data->addInt( OUTOFAMMO , 8);
				m_owner->sendWeaponMessage( index, data );
				delete data;
			}
		}
		if ( reloading )
		{
			if ( reloadTime > 0 ) --reloadTime;
			else if ( !network.isClient() )
			{
				reload();
				
				if ( network.isHost() )
				{
					ZCom_BitStream* data = new ZCom_BitStream;
					data->addInt( RELOADED , 8);
					m_owner->sendWeaponMessage( index, data );
					delete data;
				}
			}
		}
	}
}

void Weapon::drawBottom(BITMAP* where, int x, int y )
{
	if ( m_type->laserSightIntensity > 0 )
	{
		Vec direction( m_owner->getAngle() );
		Vec inc;
		float intensityInc = 0;
		if ( fabs(direction.x) > fabs(direction.y) )
		{
			inc = direction / fabs(direction.x);
		}else
		{
			inc = direction / fabs(direction.y);
		}
		
		if ( m_type->laserSightRange > 0 )
		{
			intensityInc = - m_type->laserSightIntensity / m_type->laserSightRange;
		}
		Vec posDiff;
		float intensity = m_type->laserSightIntensity;
		while ( game.level.getMaterial( (int)(m_owner->pos.x+posDiff.x), (int)(m_owner->pos.y+posDiff.y) ).particle_pass )
		{
			if ( rnd() < intensity )
			{
				if ( m_type->laserSightBlender != NONE )
					gfx.setBlender( m_type->laserSightBlender, m_type->laserSightAlpha );
				putpixel(where, (int)(posDiff.x)+x,(int)(posDiff.y)+y, m_type->laserSightColour);
				solid_mode();
			}
			posDiff+= inc;
			intensity+= intensityInc;
		}
	}
}

void Weapon::recieveMessage( ZCom_BitStream* data )
{
	switch ( static_cast<Events>( data->getInt(8) ) )
	{
		case OUTOFAMMO:
		{
			outOfAmmo();
		}
		break;
		case RELOADED:
		{
			reload();
		}
		break;
	}
}

void Weapon::outOfAmmo()
{
	if (m_type->outOfAmmo) m_type->outOfAmmo->run((BaseObject*)m_owner, NULL, NULL, this );
	ammo = 0;
	reloading = true;
	reloadTime = m_type->reloadTime;
}

void Weapon::reload()
{
	if (m_type->reloadEnd) m_type->reloadEnd->run((BaseObject*)m_owner, NULL, NULL, this );
	reloading = false;
	ammo = m_type->ammo;
}

void Weapon::actionStart( Actions action )
{
	switch ( action )
	{
		case PRIMARY_TRIGGER:
			if ( !inactiveTime && m_type->primaryPressed && ammo > 0 )
			{
				m_type->primaryPressed->run( (BaseObject*)m_owner, NULL, NULL, this );
				--ammo;
			}
			primaryShooting = true;
		break;
		
		case SECONDARY_TRIGGER:
			//TODO
		break;
	}
}

void Weapon::actionStop( Actions action )
{
	switch ( action )
	{
		case PRIMARY_TRIGGER:
			if ( primaryShooting && m_type->primaryReleased )
			{
				m_type->primaryReleased->run( (BaseObject*)m_owner, NULL, NULL, this);
			}
			primaryShooting = false;
		break;
		
		case SECONDARY_TRIGGER:
			//TODO
		break;
	}
}

BaseWorm* Weapon::getOwner()
{
	return m_owner;
}

void Weapon::delay( int time )
{
	inactiveTime = time;
}

