#include "base_object.h"
#include "util/vec.h"
#include "util/angle.h"
#include "base_player.h"
#include "glua.h"
#include "lua/bindings-objects.h"

BaseObject::BaseObject( BasePlayer* owner, Vec pos_, Vec spd_ )
: deleteMe(false), luaData(0), m_owner(owner)
, pos(pos_), spd(spd_)
, nextS_(0), nextD_(0), prevD_(0), cellIndex_(-1)
{
}

BaseObject::~BaseObject()
{
	if(luaData)
		lua.destroyReference(luaData);
}

Vec BaseObject::getRenderPos()
{
	return pos;
}

Angle BaseObject::getAngle()
{
	return Angle(0);
}

int BaseObject::getDir()
{
	return 1;
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

void BaseObject::removeRefsToPlayer(BasePlayer* player)
{
	if ( m_owner == player )
		m_owner = NULL;
}

void BaseObject::pushLuaReference()
{
	lua.pushFullReference(*this, LuaBindings::baseObjectMetaTable);
}
