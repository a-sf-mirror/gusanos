#include "bindings.h"

#include "types.h"
#include "bindings-gui.h"
#include "bindings-math.h"
#include "bindings-objects.h"
#include "bindings-resources.h"
#include "bindings-gfx.h"
#include "bindings-game.h"

#include "../gconsole.h"
#include "../game.h"
#include "../vec.h"
#include "../gfx.h"
#include "../network.h"
#include "../glua.h"

#ifndef DEDSERV
#include "../blitters/context.h"
#include "../viewport.h"
#endif
#include <cmath>
#include <string>
#include <list>
#include <iostream>
#include <vector>
#include <allegro.h>
using std::cerr;
using std::endl;
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
using boost::lexical_cast;

extern bool quit; // Extern this somewhere else (such as a gusanos.h)

namespace LuaBindings
{


int print(lua_State* L)
{
	const char* s = lua_tostring(L, 1);
	//console.addLogMsg(s);
	cerr << "LUA: " << s << endl;
	
	return 0;
}

int l_bind(lua_State* L)
{
	char const* s = lua_tostring(L, 2);
	if(!s)
		return 0;
		
	lua_pushvalue(L, 3);
	LuaReference ref = lua.createReference();
	luaCallbacks.bind(s, ref);

	return 0;
}


int l_quit(lua_State* L)
{
	quit = true;
	return 0;
}

#ifndef DEDSERV
int l_clear_keybuf(lua_State* L)
{
	clear_keybuf();
	return 0;
}
#endif


/*! connect(address)

	Connects to the address passed.
*/
int l_connect(lua_State* L)
{
	char const* s = lua_tostring(L, 1);
	if(!s)
		return 0;
	network.connect(s);
	return 0;
}

/*! console_register_command(name, function)

	Registers the function //function// as a command in the Gusanos console.
	When it is called, it will be passed each console parameter as a seperate parameter to the function.
	The command will be removed automatically when a new map is loaded.
*/
int l_console_register_command(lua_State* L)
{
	char const* name = lua_tostring(L, 1);
	lua_pushvalue(L, 2);
	LuaReference ref = lua.createReference();
	
	console.registerCommands()
			(name, boost::bind(LuaBindings::runLua, ref, _1), true);
			
	return 0;
}


std::string runLua(LuaReference ref, std::list<std::string> const& args)
{
	lua.pushReference(ref);
	int params = 0;
	for(std::list<std::string>::const_iterator i = args.begin();
		i != args.end();
		++i)
	{
		lua_pushstring(lua, i->c_str());
		++params;
	}
	
	int r = lua.call(params, 1);
	if(r != 1)
		return "";
		
	char const* s = lua_tostring(lua, -1);
	if(s)
	{
		std::string ret(s);
		lua_settop(lua, -2);
		return ret;
	}
	
	lua_settop(lua, -2);
	return "";
}


void init()
{
	LuaContext& context = lua;
	
	initMath();
	initGUI();
	initObjects();
	initResources();
	initGfx();
	initGame();
	
	context.function("print", print);
	
	context.function("console_register_command", l_console_register_command);

#ifndef DEDSERV
	context.function("clear_keybuf", l_clear_keybuf);
#endif

	context.function("quit", l_quit);
	
	context.function("bind", l_bind);
	
	context.function("connect", l_connect);

	// Bindings table and metatable
	lua_pushstring(context, "bindings");
	lua_newtable(context); // Bindings table
	
	lua_newtable(context); // Bindings metatable
	lua_pushstring(context, "__newindex");
	lua_pushcfunction(context, l_bind);
	lua_rawset(context, -3);
	
	lua_setmetatable(context, -2);

	lua_rawset(context, LUA_GLOBALSINDEX);
	
#ifdef DEDSERV
	lua_pushstring(context, "DEDSERV");
	lua_pushboolean(context, 1);
	lua_rawset(context, LUA_GLOBALSINDEX);
#else
	lua_pushstring(context, "DEDSERV");
	lua_pushboolean(context, 0);
	lua_rawset(context, LUA_GLOBALSINDEX);
#endif
	
	cerr << "LuaBindings::init() done." << endl;
}

}
