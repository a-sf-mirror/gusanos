#include "bindings-objects.h"

#include "bindings-resources.h"
#include "luaapi/types.h"

#include "../base_player.h"
#include "../player.h"
#include "../base_worm.h"
#include "../particle.h"
#include "../weapon.h"
#include "../game.h" //Do we need this?
#include "../glua.h"

#include <cmath>
#include <iostream>
#include <allegro.h>
using std::cerr;
using std::endl;
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
using boost::lexical_cast;

namespace LuaBindings
{

LuaReference wormMetaTable(0);
LuaReference baseObjectMetaTable(0);
LuaReference particleMetaTable(0);
LuaReference weaponMetaTable(0);

int shootFromObject(lua_State* L, BaseObject* object)
{
	void* typeP = lua_touserdata (L, 2);
	if(!typeP)
		return 0;
	PartType* p = *static_cast<PartType **>(typeP);
	
	int amount = 0;
	int amountVariation = 0;
	lua_Number speed = 0;
	lua_Number speedVariation = 0;
	lua_Number motionInheritance = 0;
	lua_Number distanceOffset = 0;
	AngleDiff distribution(360.0);
	AngleDiff angleOffset(0.0);
	
	int params = lua_gettop(L);
	switch(params)
	{
		default: if(params < 3) return 0;
		case 10: distanceOffset = lua_tonumber(L, 10);
		case 9:  angleOffset = AngleDiff(lua_tonumber(L, 9));
		case 8:  distribution = AngleDiff(lua_tonumber(L, 8));
		case 7:  amountVariation = lua_tointeger(L, 7);
		case 6:  motionInheritance = lua_tonumber(L, 6);
		case 5:  speedVariation = lua_tonumber(L, 5);
		case 4:  speed = lua_tonumber(L, 4);
		case 3:  amount = lua_tointeger(L, 3);
	}

	char dir = object->getDir();
	Angle baseAngle(object->getAngle() + angleOffset * dir);
	
	int realAmount = amount + rndInt(amountVariation); // int(rnd()*amountVariation);
	for(int i = 0; i < realAmount; ++i)
	{
		Angle angle = baseAngle + distribution * midrnd();
		Vec direction(angle);
		Vec spd(direction * (speed + midrnd()*speedVariation));
		if(motionInheritance)
		{
			spd += object->spd * motionInheritance;
			angle = spd.getAngle(); // Need to recompute angle
		}
		//game.insertParticle( new Particle( p, object->getPos() + direction * distanceOffset, spd, object->getDir(), object->getOwner(), angle ));
		p->newParticle(p, object->pos + direction * distanceOffset, spd, object->getDir(), object->getOwner(), angle);
	}
	
	return 0;
}

//! Worm inherits Object

int l_worm_getPlayer(lua_State* L)
{
	BaseWorm* p = static_cast<BaseWorm *>(lua_touserdata (L, 1));
	if(!p->getOwner())
		return 0;
	lua.pushReference(p->getOwner()->luaReference);
	return 1;
}


/*! Worm:get_health()

	Returns the health of this worm.
*/
int l_worm_getHealth(lua_State* L)
{
	BaseWorm* p = static_cast<BaseWorm *>(lua_touserdata (L, 1));
	lua_pushnumber(L, p->getHealth());
	return 1;
}

/*! Worm:set_weapon(slot, weapon)

	Sets the weapon of a slot in [1, max] to the WeaponType //weapon//.
*/
/* TODO
LMETHOD(BaseWorm, worm_setWeapon,
	p->m_weapons*/
	

int l_worm_remove(lua_State* L)
{
	BaseWorm* p = static_cast<BaseWorm *>(lua_touserdata (L, 1));
	p->deleteMe = true;
	return 0;
}

int l_worm_pos(lua_State* L)
{
	BaseWorm* p = static_cast<BaseWorm *>(lua_touserdata (L, 1));
	lua_pushnumber(L, p->pos.x);
	lua_pushnumber(L, p->pos.y);
	return 2;
}

int l_worm_spd(lua_State* L) //
{
	BaseWorm* p = static_cast<BaseWorm *>(lua_touserdata (L, 1));
	lua_pushnumber(L, p->spd.x);
	lua_pushnumber(L, p->spd.y);
	return 2;
}

int l_worm_push(lua_State* L) //
{
	BaseWorm* p = static_cast<BaseWorm *>(lua_touserdata (L, 1));
	p->spd.x += lua_tonumber(L, 2);
	p->spd.y += lua_tonumber(L, 3);
	return 0;
}

int l_worm_data(lua_State* L)
{
	BaseWorm* p = static_cast<BaseWorm *>(lua_touserdata (L, 1));
	if(p->luaData)
	{
		lua.pushReference(p->luaData);
	}
	else
	{
		lua_newtable(L);
		lua_pushvalue(L, -1);
		p->luaData = lua.createReference();
	}
	
	return 1;
}

int l_worm_shoot(lua_State* L)
{
	BaseWorm* object = static_cast<BaseWorm *>(lua_touserdata (L, 1));
	
	return shootFromObject(L, object);
}

/*! Worm:getAngle()

	Returns the current angle of the worm.
*/

LMETHOD(BaseWorm, worm_getAngle,
	lua_pushnumber(context, p->getAngle().toDeg());
	return 1;
)

LMETHOD(BaseWorm, worm_current_weapon,
	if(Weapon* w = p->getCurrentWeapon())
	{
		context.pushFullReference(*w, weaponMetaTable);
		return 1;
	}
	return 0;
)

/*! Object:remove()

	Removes the object in the next frame.
*/
int l_baseObject_remove(lua_State* L)
{
	BaseObject* p = *static_cast<BaseObject **>(lua_touserdata (L, 1));
	p->deleteMe = true;
	return 0;
}

/*! Object:pos()

	Returns the position of this object as a tuple.
	i.e.:
	<code>
	local x, y = object:pos()
	</code>
*/

int l_baseObject_pos(lua_State* L)
{
	BaseObject* p = *static_cast<BaseObject **>(lua_touserdata (L, 1));
	lua_pushnumber(L, p->pos.x);
	lua_pushnumber(L, p->pos.y);
	return 2;
}


/*! Object:spd()

	Returns the speed of this object as a tuple.
	e.g.:
	<code>
	local vx, vy = object:spd()
	</code>
*/
int l_baseObject_spd(lua_State* L) //
{
	BaseObject* p = *static_cast<BaseObject **>(lua_touserdata (L, 1));
	lua_pushnumber(L, p->spd.x);
	lua_pushnumber(L, p->spd.y);
	return 2;
}

int l_baseObject_push(lua_State* L) //
{
	BaseObject* p = *static_cast<BaseObject **>(lua_touserdata (L, 1));
	p->spd.x += lua_tonumber(L, 2);
	p->spd.y += lua_tonumber(L, 3);
	return 0;
}

/*! Object:data()

	Returns a table associated with this object that can
	be used by Lua scripts to store values.
*/

int l_baseObject_data(lua_State* L)
{
	BaseObject* p = *static_cast<BaseObject **>(lua_touserdata (L, 1));
	if(p->luaData)
	{
		lua.pushReference(p->luaData);
	}
	else
	{
		lua_newtable(L);
		lua_pushvalue(L, -1);
		p->luaData = lua.createReference();
	}
	
	return 1;
}

/*! Object:get_player()

	Returns a Player object of the player that owns this object.
*/
METHOD(BaseObject, baseObject_getPlayer,
	if(!p->getOwner())
		return 0;
	lua.pushReference(p->getOwner()->luaReference);
	return 1;
)

/*! Object:get_closest_worm()

	Returns the closest worm that fulfills these requirements:
	
	  * It is not the owner of the object.
	  * It is visible and active.
	  * The straight path to it from the object is not, for particles, blocked.
*/
int l_baseObject_getClosestWorm(lua_State* L)
{
	BaseObject* p = *static_cast<BaseObject **>(lua_touserdata (L, 1));
	
	Vec from = p->pos;
	
	int fromx = int(from.x);
	int fromy = int(from.y);
	
	BaseWorm* minWorm = 0;
	float minDistSqr = 10000000.f;
	
	for(std::list<BasePlayer*>::iterator playerIter = game.players.begin(); playerIter != game.players.end(); ++playerIter)
	{
		if(p->getOwner() != *playerIter)
		{
			BaseWorm* worm = (*playerIter)->getWorm();
		
			if(worm->isActive())
			//if(worm->isActive())
			{
				float distSqr = (worm->pos - from).lengthSqr();
				if(distSqr < minDistSqr && !game.level.trace(fromx, fromy, int(worm->pos.x), int(worm->pos.y), Level::ParticleBlockPredicate()))
				{
					minDistSqr = distSqr;
					minWorm = worm;
				}
			}
		}
	}
	
	if(!minWorm)
		return 0;
		
	minWorm->pushLuaReference();

	return 1;
}

/*! Object:shoot(type, amount, speed, speedVariation, motionInheritance, amountVariation, distribution, angleOffset, distanceOffset)

	Shoots an object of ParticleType 'type'. All parameters except 'type' are optional.
*/

int l_baseObject_shoot(lua_State* L)
{
	BaseObject* object = *static_cast<BaseObject **>(lua_touserdata (L, 1));
	
	return shootFromObject(L, object);
}

/*! Object:get_angle()

	Returns the current angle of the particle.
*/

METHOD(BaseObject, baseObject_getAngle,
	lua_pushnumber(context, p->getAngle().toDeg());
	return 1;
)

//! Particle inherits Object

/*! Particle:set_angle(angle)

	Changes the angle of the particle to //angle//.
*/

int l_particle_setAngle(lua_State* L)
{
	Particle* p = *static_cast<Particle **>(lua_touserdata (L, 1));
	p->setAngle(Angle((double)lua_tonumber(L, 2)));
	return 0;
}

/*! Weapon:is_reloading()

	Returns true if this weapon is reloading.
*/

METHOD(Weapon, weaponinstr_reloading,
	context.push(p->reloading);
	return 1;
)

/*! Weapon:reload_time()

	Returns the reload time left on this weapon.
	Does only make sense if is_reloading() is true.
*/
METHOD(Weapon, weaponinstr_reload_time,
	context.push(p->getReloadTime());
	return 1;
)

/*! Weapon:ammo()

	Returns the amount of ammo left in this weapon.
*/
METHOD(Weapon, weaponinstr_ammo,
	context.push(p->getAmmo());
	return 1;
)

/*! Weapon:type()

	Returns the weapon type in the form of a WeaponType object.
*/
METHOD(Weapon, weaponinstr_type,
	context.pushFullReference(*p->getType(), weaponTypeMetaTable);
	return 1;
)

void addBaseObjectFunctions(LuaContext& context)
{
	context.tableFunctions()
		("remove", l_baseObject_remove)
		("pos", l_baseObject_pos)
		("spd", l_baseObject_spd)
		("push", l_baseObject_push)
		("get_player", l_baseObject_getPlayer)
		("get_closest_worm", l_baseObject_getClosestWorm)
		("data", l_baseObject_data)
		("shoot", l_baseObject_shoot)
		("get_angle", l_baseObject_getAngle)
	;
}

void initObjects()
{
	LuaContext& context = lua;
	
	// BaseObject method and metatable
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	addBaseObjectFunctions(context);
	
	lua_rawset(context, -3);
	baseObjectMetaTable = context.createReference();
	
	// Particle method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	addBaseObjectFunctions(context);

	context.tableFunctions()
		("set_angle", l_particle_setAngle)
	;

	lua_rawset(context, -3);
	particleMetaTable = context.createReference();
	
	// Worm method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	//NOT COMPATIBLE: addBaseObjectFunctions(context); // BaseWorm inherits from BaseObject

	context.tableFunctions()
		("remove", l_worm_remove)
		("pos", l_worm_pos)
		("spd", l_worm_spd)
		("push", l_worm_push)
		("data", l_worm_data)
		("get_player", l_worm_getPlayer)
		("get_health", l_worm_getHealth)
		("shoot", l_worm_shoot)
		("get_angle", l_worm_getAngle)
		("current_weapon", l_worm_current_weapon)
	;
	
		
	lua_rawset(context, -3);
	wormMetaTable = context.createReference();
	
	CLASS(weapon,
		("is_reloading", l_weaponinstr_reloading)
		("reload_time", l_weaponinstr_reload_time)
		("ammo", l_weaponinstr_ammo)
		("type", l_weaponinstr_type)
	)
}

}
