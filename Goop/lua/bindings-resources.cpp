#include "bindings-resources.h"
#include "bindings.h"

#include "types.h"

#include "../game.h"
#include "../gfx.h"
#include "../script.h"
#include "../part_type.h"
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
#endif
LuaReference partTypeMetaTable(0);

int l_sprites_load(lua_State* L)
{
	const char* n = lua_tostring(L, 1);

	SpriteSet *s = spriteList.load(n);
	lua_pushlightuserdata(L, s);

	return 1;
}

int l_sprites_render(lua_State* L)
{
#ifndef DEDSERV
	SpriteSet* s = (SpriteSet *)lua_touserdata(L, 1);
	if(!s)
		return 0;
		
	BITMAP* b = (BITMAP *)lua_touserdata(L, 2);
		
	int frame = (int)lua_tonumber(L, 3);
	int x = (int)lua_tonumber(L, 4);
	int y = (int)lua_tonumber(L, 5);
	s->getSprite(frame)->draw(b, x, y);
#endif
	return 0;
}

#ifndef DEDSERV
/*! font_load(name)

	Loads and returns a Font object of the font with the passed name.
	
	If the font couldn't be loaded, nil is returned.
*/
int l_font_load(lua_State* L)
{
	LuaContext context(L);
	
	char const* n = lua_tostring(L, 1);

	Font *f = fontLocator.load(n);
	if(!f)
	{
		lua_pushnil(L);
		return 1;
	}
	context.pushFullReference(*f, LuaBindings::fontMetaTable);

	return 1;
}
/*! Font:render(bitmap, string, x, y[, r, g, b])

	Draws the text 'string' on 'bitmap' at the position (x, y).
	
	If (r, g, b) is supplied, it draws the text with that color,
	otherwise it draws the text white.
*/
int l_font_render(lua_State* L)
{
	Font *f = *(Font **)lua_touserdata(L, 1);
	if(!f || lua_gettop(L) < 5)
		return 0;
		
	BITMAP* b = *(BITMAP **)lua_touserdata(L, 2);
	
	char const* s = lua_tostring(L, 3);
	if(!s)
		return 0;
		
	int x = static_cast<int>(lua_tonumber(L, 4));
	int y = static_cast<int>(lua_tonumber(L, 5));
	
	if(lua_gettop(L) >= 8)
	{
		int cr = static_cast<int>(lua_tonumber(L, 6));
		int cg = static_cast<int>(lua_tonumber(L, 7));
		int cb = static_cast<int>(lua_tonumber(L, 8));
		f->draw(b, s, x, y, 0, 255, cr, cg, cb);
	}
	else
		f->draw(b, s, x, y, 0);
	
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

void initResources()
{
	LuaContext& context = lua;
	
	context.function("sprites_load", l_sprites_load);
	context.function("sprites_render", l_sprites_render);
	
	context.function("load_particle", l_load_particle);
	
#ifndef DEDSERV
	context.function("font_load", l_font_load);
#endif
	
	context.function("map_is_loaded", l_map_is_loaded);
	
	
	// Particle type method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);

	lua_rawset(context, -3);
	partTypeMetaTable = context.createReference();

#ifndef DEDSERV
	// Font method and metatable

	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	context.tableFunction("render", l_font_render);
		
	lua_rawset(context, -3);
	fontMetaTable = context.createReference();

#endif
	// Global metatable
	
	lua_pushvalue(context, LUA_GLOBALSINDEX);
	
	lua_newtable(context);
	context.tableFunction("__index", l_load_script);
	
	lua_setmetatable(context, -2);
}

}
