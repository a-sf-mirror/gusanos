#include "base_object.h"
#include "vec.h"
#include "base_player.h"
#include "glua.h"
#include "lua/bindings.h"

BaseObject::BaseObject( BasePlayer* owner, int dir ) :
	deleteMe(false), luaData(0), m_dir(dir), m_owner(owner)
{
}

BaseObject::~BaseObject()
{
	if(luaData)
		lua.destroyReference(luaData);
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

Angle BaseObject::getAngle()
{
	return Angle(0);
}

int BaseObject::getDir()
{
	return m_dir;
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
