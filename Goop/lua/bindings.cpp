#include "bindings.h"

#include "../base_player.h"
#include "../gconsole.h"
#include "../vec.h"
#include "../sprite_set.h"
#include "../sprite.h"
#include "../gfx.h"
#include "../menu.h"
#include "omfggui.h"
#include <cmath>
#include <string>
#include <iostream>
#include <vector>
using std::cerr;
using std::endl;
#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

int LuaBindings::playerIterator = 0;
int LuaBindings::playerMetaTable = 0;
std::vector<int> LuaBindings::guiWndMetaTable;

inline lua_Number luaL_checknumber(lua_State *L, int narg)
{
	lua_Number d = lua_tonumber(L, narg);
	if(d == 0 && !lua_isnumber(L, narg))
		; // TODO: tag_error(L, narg, LUA_TNUMBER);
	return d;
}

int LuaBindings::print(lua_State* L)
{
	const char* s = lua_tostring(L, 1);
	console.addLogMsg(s);
	
	return 0;
}

int l_bind(lua_State* L)
{
	const char* callback = lua_tostring(L, 1);
	const char* file = lua_tostring(L, 2);
	const char* function = lua_tostring(L, 3);
	game.luaCallbacks.bind(callback, file, function);
	
	return 0;
}

int l_sqrt(lua_State* L)
{
	lua_pushnumber(L, sqrt(luaL_checknumber(L, 1)));
	return 1;
}

int l_abs(lua_State* L)
{
	lua_pushnumber(L, abs(luaL_checknumber(L, 1)));
	return 1;
}

int l_randomint(lua_State* L)
{
	int l = (int)luaL_checknumber(L, 1);
	int u = (int)luaL_checknumber(L, 2);
	
	lua_pushnumber(L, l + (unsigned int)(rndgen()) % (u - l + 1));
	
	return 1;
}

int l_randomfloat(lua_State* L)
{
	lua_Number l = luaL_checknumber(L, 1);
	lua_Number u = luaL_checknumber(L, 2);
	
	lua_pushnumber(L, l + rnd() * (u - l));
	
	return 1;
}

int l_vector(lua_State* L)
{
	Vec& vec = *(Vec *)lua_newuserdata (L, sizeof(Vec));
	vec.x = (float)luaL_checknumber(L, 1);
	vec.y = (float)luaL_checknumber(L, 1);
	// TODO: Set metatable
	return 1;
}

int l_vector_add(lua_State* L)
{
	Vec& vecA = *(Vec *)lua_touserdata(L, 1);
	Vec& vecB = *(Vec *)lua_touserdata(L, 2);
	
	Vec& vec = *(Vec *)lua_newuserdata (L, sizeof(Vec));
	vec = vecA + vecB;

	return 1;
}

int l_vector_tostring(lua_State* L)
{
	Vec& vec = *(Vec *)lua_touserdata(L, 1);

	std::string s = "(" + lexical_cast<std::string>(vec.x) + ", " + lexical_cast<std::string>(vec.y) + ")";
	lua_pushstring(L, s.c_str());

	return 1;
}

int l_vector_lengthSqr(lua_State* L)
{
	Vec& vec = *(Vec *)lua_touserdata(L, 1);

	lua_pushnumber(L, vec.lengthSqr());

	return 1;
}

int l_vector_length(lua_State* L)
{
	Vec& vec = *(Vec *)lua_touserdata(L, 1);

	lua_pushnumber(L, vec.length());

	return 1;
}

int l_sprites_load(lua_State* L)
{
	const char* n = lua_tostring(L, 1);

	SpriteSet *s = spriteList.load(n);
	lua_pushlightuserdata(L, s);

	return 1;
}

int l_sprites_render(lua_State* L)
{
	SpriteSet *s = (SpriteSet *)lua_touserdata(L, 1);
	if(!s)
		return 0;
		
	int frame = (int)lua_tonumber(L, 2);
	int x = (int)lua_tonumber(L, 3);
	int y = (int)lua_tonumber(L, 4);
	s->getSprite(frame)->draw(gfx.buffer, x, y);

	return 0;
}

int l_map_is_loaded(lua_State* L)
{
	lua_pushboolean(L, game.level.loaded);
	
	return 1;
}

int l_game_players(lua_State* L)
{
	game.lua.pushReference(LuaBindings::playerIterator);
	typedef std::list<BasePlayer*>::iterator iter;
	iter& i = *(iter *)lua_newuserdata (L, sizeof(iter));
	i = game.players.begin();
	lua_pushnil(L);
	
	return 3;
}

int l_player_kills(lua_State* L)
{
	//if(BasePlayer* p = dynamic_cast<BasePlayer *>(lua_touserdata (L, 1)))
	BasePlayer* p = *static_cast<BasePlayer **>(lua_touserdata (L, 1));
	lua_pushnumber(L, p->kills);
	/*
	else
		lua_pushnil(L);*/
	
	return 1;
}

int l_player_name(lua_State* L)
{
	//if(BasePlayer* p = dynamic_cast<BasePlayer *>(lua_touserdata(L, 1)))
	BasePlayer* p = *static_cast<BasePlayer **>(lua_touserdata (L, 1));
	lua_pushstring(L, p->m_name.c_str());
	/*else
		lua_pushnil(L);*/
	
	return 1;
}

int l_game_playerIterator(lua_State* L)
{
	typedef std::list<BasePlayer*>::iterator iter;
	iter& i = *(iter *)lua_touserdata(L, 1);
	if(i == game.players.end())
		lua_pushnil(L);
	else
	{
		//lua_pushlightuserdata(L, *i);
		BasePlayer** p = (BasePlayer **)lua_newuserdata(L, sizeof(BasePlayer *));
		*p = *i;
		game.lua.pushReference(LuaBindings::playerMetaTable);
		if(!lua_istable(L, -1))
			cerr << "Metatable is not a table!" << endl;
		if(!lua_setmetatable(L, -2))
			cerr << "Couldn't set player metatable!" << endl;
		++i;
	}
	
	return 1;
}

int l_gui_find(lua_State* L)
{
	char const* s = lua_tostring(L, 1);
	OmfgGUI::Wnd* w = OmfgGUI::menu.findNamedWindow(s);
	if(!w)
	{
		lua_pushnil(L);
		return 1;
	}
	
	OmfgGUI::Wnd** wp = (OmfgGUI::Wnd **)lua_newuserdata(L, sizeof(OmfgGUI::Wnd *));
	*wp = w;
	game.lua.pushReference(LuaBindings::guiWndMetaTable[w->classID()]);
	lua_setmetatable(L, -2);
	
	return 1;
}

int l_gui_wnd_attribute(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));
	char const* name = lua_tostring(L, 2);
	std::string res;
	if(p->getAttrib(name, res))
		lua_pushstring(L, res.c_str());
	else
		lua_pushnil(L);
	
	return 1;
}

int l_gui_wnd_set_visibility(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));
	
	p->setVisibility(lua_toboolean(L, 2));

	return 0;
}

int l_gui_list_insert(lua_State* L)
{
	OmfgGUI::List* p = *static_cast<OmfgGUI::List **>(lua_touserdata (L, 1));
	
	int c = lua_gettop(L);
	OmfgGUI::List::Node* n = new OmfgGUI::List::Node("");
	p->push_back(n);
	for(int i = 2; i <= c; ++i)
		n->setText(i - 2, lua_tostring(L, i));
	
	//TODO: Push reference to element
	
	return 0;
}

int l_gui_list_clear(lua_State* L)
{
	OmfgGUI::List* p = *static_cast<OmfgGUI::List **>(lua_touserdata (L, 1));
	
	p->clear();

	return 0;
}

int l_gui_list_add_column(lua_State* L)
{
	OmfgGUI::List* p = *static_cast<OmfgGUI::List **>(lua_touserdata (L, 1));
	char const* name = lua_tostring(L, 2);
	lua_Number widthFactor = lua_tonumber(L, 3);
	
	p->addColumn(OmfgGUI::List::ColumnHeader(name, widthFactor));

	return 0;
}



void LuaBindings::addGUIWndFunctions(LuaContext& context)
{
	lua_pushstring(context, "attribute");
	lua_pushcfunction(context, l_gui_wnd_attribute);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "set_visibility");
	lua_pushcfunction(context, l_gui_wnd_set_visibility);
	lua_rawset(context, -3);
}

void LuaBindings::addGUIListFunctions(LuaContext& context)
{
	lua_pushstring(context, "insert");
	lua_pushcfunction(context, l_gui_list_insert);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "clear");
	lua_pushcfunction(context, l_gui_list_clear);
	lua_rawset(context, -3);

	lua_pushstring(context, "add_column");
	lua_pushcfunction(context, l_gui_list_add_column);
	lua_rawset(context, -3);
}

void LuaBindings::init(LuaContext& context)
{
	context.function("print", print);
	context.function("sqrt", l_sqrt);
	context.function("abs", l_abs);
	
	context.function("randomint", l_randomint);
	context.function("randomfloat", l_randomfloat);
	
	context.function("vector", l_vector);
	context.function("vector_add", l_vector_add);
	context.function("vector_tostring", l_vector_tostring);
	context.function("vector_lengthSqr", l_vector_lengthSqr);
	context.function("vector_length", l_vector_length);
	
	context.function("sprites_load", l_sprites_load);
	context.function("sprites_render", l_sprites_render);
	
	context.function("map_is_loaded", l_map_is_loaded);
	
	context.function("game_players", l_game_players);
	lua_pushcfunction(context, l_game_playerIterator);
	LuaBindings::playerIterator = context.createReference();
	
	context.function("player_kills", l_player_kills);
	context.function("player_name", l_player_name);
	
	context.function("gui_find", l_gui_find);

	context.function("bind", l_bind);	
	
	/*
	lua_newtable(context);
	lua_pushstring(context, "__add");
	lua_pushcfunction(context, l_vector_add);
	lua_rawset(context, -3);*/
	
	// Player method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	lua_pushstring(context, "kills");
	lua_pushcfunction(context, l_player_kills);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "name");
	lua_pushcfunction(context, l_player_name);
	lua_rawset(context, -3);
	
	lua_rawset(context, -3);
	LuaBindings::playerMetaTable = context.createReference();
	
	LuaBindings::guiWndMetaTable.resize(OmfgGUI::Context::WndCount);
	
	// GUI Wnd method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	addGUIWndFunctions(context);

	lua_rawset(context, -3);
	LuaBindings::guiWndMetaTable[OmfgGUI::Context::Unknown] = context.createReference();
	
	// GUI List method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	addGUIWndFunctions(context);
	addGUIListFunctions(context);

	lua_rawset(context, -3);
	LuaBindings::guiWndMetaTable[OmfgGUI::Context::List] = context.createReference();
}