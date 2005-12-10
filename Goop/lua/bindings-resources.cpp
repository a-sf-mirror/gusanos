#include "bindings-resources.h"
#include "bindings-gfx.h"
#include "bindings.h"

#include "luaapi/types.h"

#include "../game.h"
#include "../gfx.h"
#include "../script.h"
#include "../part_type.h"
#include "../weapon_type.h"
#include "../glua.h"

//TEMP:
#include "../sprite_set.h"
#include "../sprite.h"

#ifndef DEDSERV
#include "../font.h"
#endif
#include <cmath>
#include <string>
#include <list>
#include <iostream>
#include <vector>
using std::cerr;
using std::endl;
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
using boost::lexical_cast;

namespace LuaBindings
{

#ifndef DEDSERV
LuaReference fontMetaTable(0);
LuaReference spritesMetaTable(0);
#endif
LuaReference partTypeMetaTable(0);
LuaReference weaponTypeMetaTable(0);
LuaReference mapIterator(0);

enum FontFlags
{
	None        = 0,
	CenterV     = (1<<0),
	CenterH     = (1<<1),
	Shadow      = (1<<2),
	Formatting  = (1<<3),
};

#ifndef DEDSERV

/*! sprites_load(name)

	Loads and returns a SpriteSet object of the sprite set with the passed name.
	
	If the sprite set couldn't be loaded, nil is returned.
*/
int l_sprites_load(lua_State* L)
{
	LuaContext context(L);
	
	const char* n = lua_tostring(context, 1);
	if(!n) return 0;

	SpriteSet *s = spriteList.load(n);
	if(!s) return 0;
	
	context.pushFullReference(*s, LuaBindings::spritesMetaTable);

	return 1;
}

/*! SpriteSet:render(bitmap, frame, x, y)

	Draws the frame //frame// of the sprite set on //bitmap// with the pivot at position (x, y).
*/
METHOD(SpriteSet, sprites_render,
	BITMAP* b = *static_cast<BITMAP **>(lua_touserdata(context, 2));
		
	int frame = lua_tointeger(context, 3);
	int x = lua_tointeger(context, 4);
	int y = lua_tointeger(context, 5);
	p->getSprite(frame)->draw(b, x, y);
	
	return 0;
)

/*! SpriteSet:render(bitmap, x1, y1, x2, y2, r, g, b)

	Draws the sprite set as a skinned box with the rectangle (x1, y1) - (x2, y2)
	and background color (r, g, b).
*/
METHOD(SpriteSet, sprites_render_skinned_box,
	BITMAP* b = *static_cast<BITMAP **>(lua_touserdata(context, 2));
	
	int x1 = lua_tointeger(context, 3);
	int y1 = lua_tointeger(context, 4);
	int x2 = lua_tointeger(context, 5);
	int y2 = lua_tointeger(context, 6);
	int cr = lua_tointeger(context, 7);
	int cg = lua_tointeger(context, 8);
	int cb = lua_tointeger(context, 9);
	p->drawSkinnedBox(b, blitter, Rect(x1, y1, x2, y2), makecol(cr, cg, cb));
	
	return 0;
)

/*! SpriteSet:frames()

	Returns the number of frames in this sprite set.
*/
METHOD(SpriteSet, sprites_frames,
	context.push(static_cast<int>(p->getFramesWidth()));
	return 1;
)


/*! font_load(name)

	Loads and returns a Font object of the font with the passed name.
	
	If the font couldn't be loaded, nil is returned.
*/
int l_font_load(lua_State* L)
{
	LuaContext context(L);
	
	char const* n = lua_tostring(L, 1);
	if(!n) return 0;

	Font *f = fontLocator.load(n);
	if(!f) return 0;
	
	context.pushFullReference(*f, LuaBindings::fontMetaTable);

	return 1;
}

/*! Font:render(bitmap, string, x, y[, r, g, b[, flags]])

	Draws the text 'string' on 'bitmap' at the position (x, y).
	
	If (r, g, b) is supplied, it draws the text with that color,
	otherwise it draws the text white.

	//flags// can be a sum of these values:
	Font.None : No flags.
	Font.CenterV : Center the text vertically with y at the middle.
	Font.CenterH : Center the text horizontally with x at the middle.
	Font.Shadow : Draw a shadow under the text.
	Font.Formatting : Draw the text with formatting.
*/
int l_font_render(lua_State* L)
{
	Font *f = *static_cast<Font **>(lua_touserdata(L, 1));
	if(!f || lua_gettop(L) < 5)
		return 0;
		
	BITMAP* b = *static_cast<BITMAP **>(lua_touserdata(L, 2));
	
	char const* sc = lua_tostring(L, 3);
	if(!sc)
		return 0;
		
	std::string s(sc);
		
	int x = lua_tointeger(L, 4);
	int y = lua_tointeger(L, 5);
	
	int cr = 255, cg = 255, cb = 255;
	int flags = 0;
	
	if(lua_gettop(L) >= 8)
	{
		cr = lua_tointeger(L, 6);
		cg = lua_tointeger(L, 7);
		cb = lua_tointeger(L, 8);
		
		if(lua_gettop(L) >= 9)
		{
			flags = lua_tointeger(L, 9);
		}
	}
	
	int realFlags = 0;
	
	if(flags & Shadow)
		realFlags |= Font::Shadow;
	if(flags & Formatting)
		realFlags |= Font::Formatting;
	
	if(flags & (CenterV | CenterH))
	{
		std::pair<int, int> dim = f->getDimensions(s, 0, realFlags);

		if(flags & CenterH)
			x -= (dim.first - 1) / 2;
		if(flags & CenterV)
			y -= (dim.second - 1) / 2;
	}

	f->draw(b, s, x, y, 0, 256, cr, cg, cb, realFlags);
	
	return 0;
}
#endif

/*! map_is_loaded()

	Returns true if a map is loaded, otherwise false.
*/
int l_map_is_loaded(lua_State* L)
{
	lua_pushboolean(L, game.level.loaded);
	
	return 1;
}


int l_load_script(lua_State* L)
{
	char const* s = lua_tostring(L, 2);
	if(!s)
		return 0;
	Script* script = scriptLocator.load(s);
	
	if(!script)
		return 0;
	
	// Return the allocated table
	lua_pushvalue(L, 2);
	lua_rawget(L, LUA_GLOBALSINDEX);
	return 1;
}

/*! load_particle(name)

	Loads a particle type with the name //name// and returns it as a
	ParticleType object.
*/
int l_load_particle(lua_State* L)
{
	char const* s = lua_tostring(L, 1);
	if(!s)
		return 0;
		
	PartType* type = partTypeList.load(s);
	if(!type)
		return 0;
	
	LuaContext context(L);
	
	context.pushFullReference(*type, partTypeMetaTable);
	return 1;
}

/*! weapon_random()

	Returns a random WeaponType object.
*/
int l_weapon_random(lua_State* L)
{
	LuaContext context(L);
	WeaponType* p = game.weaponList[rndInt(game.weaponList.size())];
	context.pushFullReference(*p, weaponTypeMetaTable);
	return 1;
}

/*! weapon_count()

	Returns the total number of weapons.
*/
int l_weapon_count(lua_State* L)
{
	lua_pushinteger(L, game.weaponList.size());
	return 1;
}

/*! WeaponType:next()

	Returns the next weapon type after this one.
*/
METHOD(WeaponType, weapon_next,
	size_t n = p->getIndex() + 1;
	if(n >= game.weaponList.size())
		n = 0;
	context.pushFullReference(*game.weaponList[n], weaponTypeMetaTable);
	return 1;
)

/*! WeaponType:prev()

	Returns the previous weapon type after this one.
*/
METHOD(WeaponType, weapon_prev,
	size_t n = p->getIndex();
	if(n == 0)
		n = game.weaponList.size() - 1;
	else
		--n;
	context.pushFullReference(*game.weaponList[n], weaponTypeMetaTable);
	return 1;
)

/*! WeaponType:name()

	Returns the name of this weapon type.
*/
METHOD(WeaponType, weapon_name,
	lua_pushstring(context, p->name.c_str());
	return 1;
)

/*! WeaponType:reload_time()

	Returns the time it takes for this weapon type to reload.
*/
METHOD(WeaponType, weapon_reload_time,
	context.push(p->reloadTime);
	return 1;
)

/*! WeaponType:ammo()

	Returns the amount of ammo for this weapon type when reloaded.
*/
METHOD(WeaponType, weapon_ammo,
	context.push(p->ammo);
	return 1;
)

BINOP(WeaponType, weapon_eq,
	context.push(a == b);
	return 1;
)

/*! maps()

	Returns an iterator that iterates through all maps.
*/
int l_maps(lua_State* L)
{
	LuaContext context(L);
	
	context.pushReference(mapIterator);
	
	typedef ResourceLocator<Level>::NamedResourceMap::const_iterator iter;
	
	iter& i = *(iter *)lua_newuserdata (L, sizeof(iter));
	i = levelLocator.getMap().begin();
	lua_pushnil(L);
	
	return 3;
}

int l_mapIterator(lua_State* L)
{
	LuaContext context(L);
	
	typedef ResourceLocator<Level>::NamedResourceMap::const_iterator iter;
	
	iter& i = *(iter *)lua_touserdata(L, 1);
	if(i == levelLocator.getMap().end())
		lua_pushnil(L);
	else
	{
		//lua.pushReference((*i)->luaReference);
		context.push(i->first);
		++i;
	}
	
	return 1;
}

void initResources()
{
	LuaContext& context = lua;
	
	lua_pushcfunction(context, l_mapIterator);
	mapIterator = context.createReference();
	
	context.functions()
		("load_particle", l_load_particle)
		("weapon_random", l_weapon_random)
		("weapon_count", l_weapon_count)
#ifndef DEDSERV
		("sprites_load", l_sprites_load)
		("font_load", l_font_load)
#endif
		("map_is_loaded", l_map_is_loaded)
		("maps", l_maps)
		
	;
	
	CLASS(partType,
		/* Insert stuff here */
	)
	
	CLASSM(weaponType,
		("__eq", l_weapon_eq)
	,
		("next", l_weapon_next)
		("prev", l_weapon_prev)
		("name", l_weapon_name)
		("reload_time", l_weapon_reload_time)
		("ammo", l_weapon_ammo)
	)

#ifndef DEDSERV
	// Font method and metatable

	CLASS(font,
		("render", l_font_render)
	)
	
	ENUM(Font,
		("None", None)
		("CenterV", CenterV)
		("CenterH", CenterH)
		("Shadow", Shadow)
		("Formatting", Formatting)
	)
	
	CLASS(sprites,
		("render", l_sprites_render)
		("render_skinned_box", l_sprites_render_skinned_box)
		("frames", l_sprites_frames)
	)

#endif
	// Global metatable
	
	lua_pushvalue(context, LUA_GLOBALSINDEX);
	
	lua_newtable(context);
	context.tableFunction("__index", l_load_script);
	
	lua_setmetatable(context, -2);
}

}
