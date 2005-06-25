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
	reloading = false;
	reloadTime = 0;
}

Weapon::~Weapon()
{
	primaryShooting = false;
	ammo = m_type->ammo;
	inactiveTime = 0;
	reloading = false;
	reloadTime = 0;
}

void Weapon::reset()
{
	
}

void Weapon::think()
{
	if (inactiveTime > 0) inactiveTime--;
	else
	{
		if ( primaryShooting && ammo > 0)
		{
			if (m_type->primaryShoot) m_type->primaryShoot->run((BaseObject*)m_owner, NULL, NULL, this );
			ammo--;
		}
	}
	if ( ammo <= 0 && !reloading )
	{
		if (m_type->outOfAmmo) m_type->outOfAmmo->run((BaseObject*)m_owner, NULL, NULL, this );
		reloading = true;
		reloadTime = m_type->reloadTime;
	}
	if ( reloading )
	{
		--reloadTime;
		if ( reloadTime <= 0 )
		{
			if (m_type->reloadEnd) m_type->reloadEnd->run((BaseObject*)m_owner, NULL, NULL, this );
			reloading = false;
			ammo = m_type->ammo;
		}
	}
	
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

