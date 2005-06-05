#include "base_object.h"
#include "vec.h"
#include "base_player.h"

BaseObject::BaseObject( BasePlayer* owner ) :
	deleteMe(false), m_owner(owner)
{
}

BaseObject::~BaseObject()
{
}

Vec BaseObject::getPos()
{
	return pos;
}

Vec BaseObject::getRenderPos()
{
	return pos;
}

Vec BaseObject::getSpd()
{
	return spd;
}

float BaseObject::getAngle()
{
	return 0;
}

BasePlayer* BaseObject::getOwner()
{
	return m_owner;
}

void BaseObject::remove()
{
	deleteMe = true;
}

bool BaseObject::isCollidingWith( const Vec& point, float radius )
{
	return (pos - point).lengthSqr() < radius*radius;
}
