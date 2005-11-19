#include "bindings.h"

#include "lua/bindings-gui.h"
#include "bindings-math.h"
#include "bindings-network.h"
#include "bindings-objects.h"
#include "bindings-resources.h"
#include "bindings-gfx.h"
#include "bindings-game.h"

#include "luaapi/types.h"

#include "../gconsole.h"
//#include "../game.h"
//#include "../vec.h"
//#include "../gfx.h"
#include "../network.h"
#include "../glua.h"
#include "../menu.h"
#include "http.h"

#ifndef DEDSERV
#include "../blitters/context.h"
#include "../viewport.h"
#endif
#include <cmath>
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <vector>
#include <allegro.h>
using std::cerr;
using std::endl;
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
using boost::lexical_cast;
namespace fs = boost::filesystem;

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
	//network.connect(s);
	console.addQueueCommand(std::string("connect ") + s);
	return 0;
}

int l_host(lua_State* L)
{
	char const* map = lua_tostring(L, 1);
	if(!map)
		return 0;

	/*
	game.options.host = 1;
	if(!game.changeLevelCmd( map ))
		return 0;
	lua_pushboolean(L, true);*/
	
	console.addQueueCommand("host 1");
	console.addQueueCommand(std::string("map \"") + map + '"');
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

int l_dump(lua_State* L)
{
	LuaContext context(L);
	
	char const* s = lua_tostring(context, 1);
	if(!s)
		return 0;
	
	// Allow only [A-Za-z0-9\-]
	for(char const* p = s; *p; ++p)
	{
		if(!isalnum(*p) && *p != '-')
			return 0;
	}

	try
	{	
		fs::path dumpPath(fs::path("persistance") / (std::string(s) + ".lpr"));
		fs::create_directories( dumpPath.branch_path() );
		fs::ofstream f(dumpPath, std::ios::binary);
		
		if(!f.is_open())
			return 0;
			
		context.serializeT(f, 2);
	}
	catch(std::exception& e)
	{
		cerr << "dump() failed with error: " << e.what() << endl;
		return 0;
	}

	return 0;
}

int l_undump(lua_State* L)
{
	LuaContext context(L);
	
	char const* s = lua_tostring(context, 1);
	if(!s)
		return 0;
	
	// Don't allow '.'
	for(char const* p = s; *p; ++p)
	{
		if(*p == '.')
			return 0;
	}
	
	try
	{
		fs::path dumpPath(fs::path("persistance") / (std::string(s) + ".lpr"));
	
		fs::ifstream f(dumpPath, std::ios::binary);
		
		if(!f.is_open())
			return 0;
			
		context.deserialize(f);
	}
	catch(std::exception& e)
	{
		cerr << "undump() failed with error: " << e.what() << endl;
		return 0;
	}

	return 1;
}


std::string runLua(LuaReference ref, std::list<std::string> const& args)
{
	AssertStack as(lua);
	
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
	
	if(char const* s = lua_tostring(lua, -1))
	{
		std::string ret(s);
		lua.pop(1);
		return ret;
	}

	lua.pop(1);
	
	return "";
}

void serverListCallb(lua_State* L, LuaReference ref, HTTP::Request* req)
{
	static char const* fields[] =
	{
		"ip",
		"title",
		"desc",
		"mod",
		"map"
	};
	
	LuaContext context(L);
	AssertStack as(context);
	
	context.pushReference(ref);
	
	if(!req->success)
	{
		lua_pushnil(context);
		lua.call(1, 0);
		return;
	}
	
	lua_newtable(context);
	
	std::string::size_type p = 0, endp;
	
	for(int i = 1; (endp = req->data.find('|', p)) != std::string::npos; p = endp + 1, ++i)
	{
		std::string::size_type p2 = p, endp2;
		lua_newtable(context);
		
		for(int f = 0; f < 5 && (endp2 = req->data.find('^', p2)) < endp; p2 = endp2 + 1, ++f)
		{
			lua_pushlstring(context, req->data.c_str() + p2, endp2 - p2);
			lua_setfield(context, -2, fields[f]);
		}
		
		lua_rawseti(context, -2, i);
	}
	
	delete req; // We don't need this anymore

	lua.call(1, 0);
}

int l_fetch_server_list(lua_State* L)
{
	lua_pushvalue(L, 1);
	LuaReference ref = lua.createReference();
	
	HTTP::Request* req = network.fetchServerList();
	
	if(!req)
		return 0;

	network.addHttpRequest(req,
		boost::bind(serverListCallb, L, ref, _1));

	return 0;
}

void init()
{
	LuaContext& context = lua;
	
	initMath();
	initGUI(OmfgGUI::menu, context);
	initNetwork(context);
	initObjects();
	initResources();
	initGfx();
	initGame();

	context.functions()
		("print", print)
		("console_register_command", l_console_register_command)
		("dump", l_dump)
		("undump", l_undump)
		("fetch_server_list", l_fetch_server_list)
#ifndef DEDSERV
		("clear_keybuf", l_clear_keybuf)
#endif

		("quit", l_quit)
		("bind", l_bind)
		("connect", l_connect)
		("host", l_host)
	;

	// Bindings table and metatable
	lua_pushstring(context, "bindings");
	lua_newtable(context); // Bindings table
	
	lua_newtable(context); // Bindings metatable
	lua_pushstring(context, "__newindex");
	lua_pushcfunction(context, l_bind);
	lua_rawset(context, -3);
	
	lua_setmetatable(context, -2);

	lua_rawset(context, LUA_GLOBALSINDEX);
	
	lua_pushstring(context, "DEDSERV");
#ifdef DEDSERV
	lua_pushboolean(context, 1);
#else
	lua_pushboolean(context, 0);
#endif
	lua_rawset(context, LUA_GLOBALSINDEX);
	
	cerr << "LuaBindings::init() done." << endl;
}

}
