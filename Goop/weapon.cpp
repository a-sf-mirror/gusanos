#include "weapon.h"
#include "weapon_type.h"

#include "base_worm.h"

Weapon::Weapon(WeaponType* type, BaseWorm* owner)
{
	m_type = type;
	m_owner = owner;
	primaryShooting = false;
	ammo = type->ammo;
	inactiveTime = 0;
}

Weapon::~Weapon()
{
}

void Weapon::think()
{
	if (inactiveTime > 0) inactiveTime--;
	else
	{
		if ( primaryShooting )
		{
			if (m_type->primaryShoot) m_type->primaryShoot->run((BaseObject*)m_owner, NULL, NULL, this );
		}
	}
}

void Weapon::actionStart( Actions action )
{
	switch ( action )
	{
		case PRIMARY_TRIGGER:
			if ( !inactiveTime && m_type->primaryPressed )
			{
				m_type->primaryPressed->run( (BaseObject*)m_owner, NULL, NULL, this );
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
			if ( !inactiveTime && m_type->primaryReleased )
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

