#include "bindings.h"

#include "../base_player.h"
#include "../player.h"
#include "../base_worm.h"
#include "../particle.h"
#include "../gconsole.h"
#include "../game.h"
#include "../vec.h"
#include "../script.h"
#include "../gfx.h"
#include "../network.h"
#include "../glua.h"

//TEMP:
#include "../sprite_set.h"
#include "../sprite.h"

#ifndef DEDSERV
#include "../menu.h"
#include "../blitters/context.h"
#include "../viewport.h"
#include "../font.h"
#include "omfggui.h"
#include "omfggui_windows.h"
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

int playerIterator = 0;
int playerMetaTable = 0;
#ifndef DEDSERV
int viewportMetaTable = 0;
int fontMetaTable = 0;
std::vector<int> guiWndMetaTable;
BlitterContext blitter;
#endif
int wormMetaTable = 0;
int baseObjectMetaTable = 0;
int particleMetaTable = 0;
int partTypeMetaTable = 0;

template<class T>
inline void pushFullReference(T& x)
{
	T** i = (T **)lua_newuserdata (lua, sizeof(T*));
	*i = &x;
}

template<class T>
inline void pushObject(T const& x)
{
	T* i = (T *)lua_newuserdata (lua, sizeof(T));
	*i = x;
}

inline lua_Number luaL_checknumber(lua_State *L, int narg)
{
	lua_Number d = lua_tonumber(L, narg);
	if(d == 0 && !lua_isnumber(L, narg))
		; // TODO: tag_error(L, narg, LUA_TNUMBER);
	return d;
}

int print(lua_State* L)
{
	const char* s = lua_tostring(L, 1);
	//console.addLogMsg(s);
	cerr << "LUA: " << s << endl;
	
	return 0;
}

int l_bind(lua_State* L)
{
/*
	const char* callback = lua_tostring(L, 1);
	const char* file = lua_tostring(L, 2);
	const char* function = lua_tostring(L, 3);
	luaCallbacks.bind(callback, file, function);*/
	
	char const* s = lua_tostring(L, 2);
	if(!s)
		return 0;
		
	lua_pushvalue(L, 3);
	int ref = lua.createReference();
	luaCallbacks.bind(s, ref);

	return 0;
}

int l_sqrt(lua_State* L)
{
	lua_pushnumber(L, sqrt(luaL_checknumber(L, 1)));
	return 1;
}

int l_abs(lua_State* L)
{
	lua_pushnumber(L, fabs(luaL_checknumber(L, 1)));
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
/*
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
*/

int l_vector_diff(lua_State* L)
{
	lua_pushnumber(L, lua_tonumber(L, 3) - lua_tonumber(L, 1));
	lua_pushnumber(L, lua_tonumber(L, 4) - lua_tonumber(L, 2));
	
	return 2;
}

int l_vector_distance(lua_State* L)
{
	lua_Number vx = lua_tonumber(L, 3) - lua_tonumber(L, 1);
	lua_Number vy = lua_tonumber(L, 4) - lua_tonumber(L, 2);
	vx *= vx;
	vy *= vy;
	lua_pushnumber(L, sqrt(vx + vy));
	
	return 1;
}

int l_vector_direction(lua_State* L)
{
	lua_Number vx = lua_tonumber(L, 3) - lua_tonumber(L, 1);
	lua_Number vy = lua_tonumber(L, 4) - lua_tonumber(L, 2);

	lua_pushnumber(L, rad2deg(atan2(vx, -vy)) );
	
	return 1;
}

int l_vector_add(lua_State* L)
{
	lua_pushnumber(L, lua_tonumber(L, 1) + lua_tonumber(L, 3));
	lua_pushnumber(L, lua_tonumber(L, 2) + lua_tonumber(L, 4));
	
	return 2;
}



int l_angle_diff(lua_State* L)
{
	//lua_Number ang = lua_tonumber(L, 2) - lua_tonumber(L, 1);
	AngleDiff diff(AngleDiff((int)lua_tonumber(L, 2)) - AngleDiff((int)lua_tonumber(L, 1)));
	
	/*
	if(ang < -180.0)
	{
		do
		{
			ang += 360.0;
		}
		while(ang < -180.0);
	}
	else if(ang > 180.0)
	{
		do
		{
			ang -= 360.0;
		}
		while(ang > 180.0);
	}*/

	lua_pushnumber(L, diff.toDeg());
	
	return 1;
}

int l_angle_clamp(lua_State* L)
{
	//lua_Number ang = lua_tonumber(L, 1);
	Angle ang((double)lua_tonumber(L, 1));
	
	/*
	if(ang < 0.0)
	{
		do
		{
			ang += 360.0;
		}
		while(ang < 0.0);
	}
	else if(ang > 360.0)
	{
		do
		{
			ang -= 360.0;
		}
		while(ang > 360.0);
	}*/
	
	lua_pushnumber(L, ang.toDeg());
	
	return 1;
}

int l_angle_vector(lua_State* L)
{
	Angle ang((double)lua_tonumber(L, 1));
	lua_Number len = 1.0;
	
	if(lua_gettop(L) >= 2)
		len = lua_tonumber(L, 2);
	
	Vec vec(ang, len);
	lua_pushnumber(L, vec.x);
	lua_pushnumber(L, vec.y);
	
	return 2;
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
int l_font_load(lua_State* L)
{
	char const* n = lua_tostring(L, 1);

	Font *f = fontLocator.load(n);
	if(!f)
	{
		lua_pushnil(L);
		return 1;
	}
	pushFullReference(*f);
	lua.pushReference(LuaBindings::fontMetaTable);
	if(!lua_istable(L, -1))
		cerr << "Metatable is not a table!" << endl;
	if(!lua_setmetatable(L, -2))
		cerr << "Couldn't set player metatable!" << endl;

	return 1;
}

int l_font_render(lua_State* L)
{
	Font *f = *(Font **)lua_touserdata(L, 1);
	if(!f || lua_gettop(L) < 5)
		return 0;
		
	BITMAP* b = (BITMAP *)lua_touserdata(L, 2);
	
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
		f->draw(b, s, x, y, 0, cr, cg, cb);
	}
	else
		f->draw(b, s, x, y, 0);
	
	return 0;
}
#endif

int l_map_is_loaded(lua_State* L)
{
	lua_pushboolean(L, game.level.loaded);
	
	return 1;
}

int l_quit(lua_State* L)
{
	quit = true;
	return 0;
}

int l_game_players(lua_State* L)
{
	lua.pushReference(LuaBindings::playerIterator);
	typedef std::list<BasePlayer*>::iterator iter;
	iter& i = *(iter *)lua_newuserdata (L, sizeof(iter));
	i = game.players.begin();
	lua_pushnil(L);
	
	return 3;
}

int l_game_localPlayer(lua_State* L)
{
	int i = (int)lua_tonumber(L, 1);
	if(i >= 0 && i < game.localPlayers.size())
	{
		lua.pushReference(game.localPlayers[i]->luaReference);
		return 1;
	}
	else
		return 0;
}

#ifndef DEDSERV
int l_clear_keybuf(lua_State* L)
{
	clear_keybuf();
	return 0;
}
#endif

int l_player_kills(lua_State* L)
{
	BasePlayer* p = static_cast<BasePlayer *>(lua_touserdata (L, 1));
	lua_pushnumber(L, p->kills);

	return 1;
}

int l_player_deaths(lua_State* L)
{
	BasePlayer* p = static_cast<BasePlayer *>(lua_touserdata (L, 1));
	lua_pushnumber(L, p->deaths);

	return 1;
}

int l_player_name(lua_State* L)
{
	BasePlayer* p = static_cast<BasePlayer *>(lua_touserdata (L, 1));
	lua_pushstring(L, p->m_name.c_str());

	return 1;
}

int l_player_say(lua_State* L)
{
	BasePlayer* p = static_cast<BasePlayer *>(lua_touserdata (L, 1));
	char const* s = lua_tostring(L, 2);
	if(s)
		p->sendChatMsg(s);

	return 0;
}



/*
void pushPlayer(BasePlayer* player)
{
	pushFullReference(*player, LuaBindings::playerMetaTable);
}*/

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
		case 7:  amountVariation = (int)lua_tonumber(L, 7);
		case 6:  motionInheritance = lua_tonumber(L, 6);
		case 5:  speedVariation = lua_tonumber(L, 5);
		case 4:  speed = lua_tonumber(L, 4);
		case 3:  amount = (int)lua_tonumber(L, 3);
	}

	char dir = object->getDir();
	Angle baseAngle(object->getAngle() + angleOffset * dir);
	
	int realAmount = amount + int(rnd()*amountVariation);
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

int l_worm_getPlayer(lua_State* L)
{
	BaseWorm* p = static_cast<BaseWorm *>(lua_touserdata (L, 1));
	if(!p->getOwner())
		return 0;
	lua.pushReference(p->getOwner()->luaReference);
	return 1;
}


int l_worm_getHealth(lua_State* L)
{
	BaseWorm* p = static_cast<BaseWorm *>(lua_touserdata (L, 1));
	lua_pushnumber(L, p->getHealth());
	return 1;
}

/*
void pushWorm(BaseWorm* worm)
{
	pushFullReference(*worm, LuaBindings::wormMetaTable);
}*/

#ifndef DEDSERV
int l_viewport_getBitmap(lua_State* L)
{
	Viewport* p = *static_cast<Viewport **>(lua_touserdata (L, 1));

	lua_pushlightuserdata(L, (void *)p->getBitmap());
	return 1;
}

#endif
/*
void pushViewport(Viewport* viewport)
{
	pushFullReference(*viewport, LuaBindings::viewportMetaTable);
}

void push(BaseObject* obj)
{
	pushFullReference(*obj, LuaBindings::baseObjectMetaTable);
}*/

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

int l_baseObject_remove(lua_State* L)
{
	BaseObject* p = *static_cast<BaseObject **>(lua_touserdata (L, 1));
	p->deleteMe = true;
	return 0;
}

int l_baseObject_pos(lua_State* L)
{
	BaseObject* p = *static_cast<BaseObject **>(lua_touserdata (L, 1));
	lua_pushnumber(L, p->pos.x);
	lua_pushnumber(L, p->pos.y);
	return 2;
}

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

int l_baseObject_shoot(lua_State* L)
{
	BaseObject* object = *static_cast<BaseObject **>(lua_touserdata (L, 1));
	
	return shootFromObject(L, object);
}

int l_particle_setAngle(lua_State* L)
{
	Particle* p = *static_cast<Particle **>(lua_touserdata (L, 1));
	p->setAngle(Angle((double)lua_tonumber(L, 2)));
	return 0;
}

int l_particle_getAngle(lua_State* L)
{
	Particle* p = *static_cast<Particle **>(lua_touserdata (L, 1));
	lua_pushnumber(L, p->getAngle().toDeg());
	return 1;
}

int l_game_getClosestWorm(lua_State* L)
{
	Vec from((float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2));
	
	BaseWorm* minWorm = 0;
	float minDistSqr = 10000000.f;
	
	for(std::list<BasePlayer*>::iterator playerIter = game.players.begin(); playerIter != game.players.end(); ++playerIter)
	{
		BaseWorm* worm = (*playerIter)->getWorm();
		
		if(worm->isActive())
		{
			float distSqr = (worm->pos - from).lengthSqr();
			if(distSqr < minDistSqr)
			{
				minDistSqr = distSqr;
				minWorm = worm;
			}
		}
	}
	
	if(!minWorm)
		return 0;
		
	minWorm->pushLuaReference();
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
		lua.pushReference((*i)->luaReference);
		++i;
	}
	
	return 1;
}

int l_map_isBlocked(lua_State* L)
{
	int x1 = (int)lua_tonumber(L, 1);
	int y1 = (int)lua_tonumber(L, 2);
	int x2 = (int)lua_tonumber(L, 3);
	int y2 = (int)lua_tonumber(L, 4);
	lua_pushboolean(L, game.level.trace(x1, y1, x2, y2, Level::ParticleBlockPredicate()));
	return 1;
}

int l_map_isParticlePass(lua_State* L)
{
	int x = (int)lua_tonumber(L, 1);
	int y = (int)lua_tonumber(L, 2);

	lua_pushboolean(L, game.level.getMaterial(x, y).particle_pass);
	return 1;
}

#ifndef DEDSERV
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
	lua.pushReference(LuaBindings::guiWndMetaTable[w->classID()]);
	if(!lua_setmetatable(L, -2))
	{
		cerr << "Failed to set metatable for window " << s << "!" << endl;
	}

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

int l_gui_wnd_get_visibility(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));
	
	lua_pushboolean(L, p->isVisibile());

	return 0;
}

int l_gui_wnd_get_text(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));

	if(p)
		lua_pushstring(L, p->getText().c_str());
	else
		lua_pushnil(L);
	
	return 1;
}

int l_gui_wnd_set_text(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));
	
	char const* s = lua_tostring(L, 2);
	if(s)
		p->setText(s);
	
	return 0;
}

int l_gui_wnd_focus(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));

	OmfgGUI::menu.setFocus(p);
	
	return 0;
}

int l_gui_wnd_activate(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));

	p->doSetActivation(true);
	
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

int l_gui_list_sort(lua_State* L)
{
	OmfgGUI::List* p = *static_cast<OmfgGUI::List **>(lua_touserdata (L, 1));
	unsigned int column = static_cast<unsigned int>(lua_tonumber(L, 2));
	
	p->sortNumerically(column);

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

#endif

int l_connect(lua_State* L)
{
	char const* s = lua_tostring(L, 1);
	if(!s)
		return 0;
	network.connect(s);
	return 0;
}

int l_console_register_command(lua_State* L)
{
	char const* name = lua_tostring(L, 1);
	lua_pushvalue(L, 2);
	int ref = lua.createReference();
	
	console.registerCommands()
			(name, boost::bind(LuaBindings::runLua, ref, _1), true);
			
	return 0;
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

int l_gfx_draw_box(lua_State* L)
{
#ifndef DEDSERV
	BITMAP* b = (BITMAP *)lua_touserdata(L, 1);
	
	int x1 = (int)lua_tonumber(L, 2);
	int y1 = (int)lua_tonumber(L, 3);
	int x2 = (int)lua_tonumber(L, 4);
	int y2 = (int)lua_tonumber(L, 5);
	int cr = static_cast<int>(lua_tonumber(L, 6));
	int cg = static_cast<int>(lua_tonumber(L, 7));
	int cb = static_cast<int>(lua_tonumber(L, 8));
	
	blitter.rectfill(b, x1, y1, x2, y2, makecol(cr, cg, cb));
#endif
	return 0;
}

int l_gfx_set_alpha(lua_State* L)
{
#ifndef DEDSERV
	int alpha = (int)lua_tonumber(L, 1);
	blitter.set(BlitterContext::alpha(), alpha);
#endif
	return 0;
}

int l_gfx_reset_blending(lua_State* L)
{
#ifndef DEDSERV
	blitter.set(BlitterContext::none());
#endif
	return 0;
}

std::string runLua(int ref, std::list<std::string> const& args)
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

#ifndef DEDSERV
void addGUIWndFunctions(LuaContext& context)
{
	lua_pushstring(context, "attribute");
	lua_pushcfunction(context, l_gui_wnd_attribute);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "set_visibility");
	lua_pushcfunction(context, l_gui_wnd_set_visibility);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "get_text");
	lua_pushcfunction(context, l_gui_wnd_get_text);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "set_text");
	lua_pushcfunction(context, l_gui_wnd_set_text);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "focus");
	lua_pushcfunction(context, l_gui_wnd_focus);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "activate");
	lua_pushcfunction(context, l_gui_wnd_activate);
	lua_rawset(context, -3);
}

void addGUIListFunctions(LuaContext& context)
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
	
	lua_pushstring(context, "sort");
	lua_pushcfunction(context, l_gui_list_sort);
	lua_rawset(context, -3);
}
#endif
void addBaseObjectFunctions(LuaContext& context)
{
	lua_pushstring(context, "remove");
	lua_pushcfunction(context, l_baseObject_remove);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "pos");
	lua_pushcfunction(context, l_baseObject_pos);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "spd");
	lua_pushcfunction(context, l_baseObject_spd);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "push");
	lua_pushcfunction(context, l_baseObject_push);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "get_closest_worm");
	lua_pushcfunction(context, l_baseObject_getClosestWorm);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "data");
	lua_pushcfunction(context, l_baseObject_data);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "shoot");
	lua_pushcfunction(context, l_baseObject_shoot);
	lua_rawset(context, -3);
}

void init()
{
	LuaContext& context = lua;
	
	context.function("print", print);
	context.function("sqrt", l_sqrt);
	context.function("abs", l_abs);
	
	context.function("randomint", l_randomint);
	context.function("randomfloat", l_randomfloat);
	
/*
	context.function("vector", l_vector);
	context.function("vector_add", l_vector_add);
	context.function("vector_tostring", l_vector_tostring);
	context.function("vector_lengthSqr", l_vector_lengthSqr);
	context.function("vector_length", l_vector_length);*/
	
	context.function("vector_diff", l_vector_diff);
	context.function("vector_distance", l_vector_distance);
	context.function("vector_direction", l_vector_direction);
	context.function("vector_add", l_vector_add);
	
	context.function("angle_clamp", l_angle_clamp);
	context.function("angle_diff", l_angle_diff);
	context.function("angle_vector", l_angle_vector);
	
	context.function("sprites_load", l_sprites_load);
	context.function("sprites_render", l_sprites_render);
	
	context.function("load_particle", l_load_particle);
	
#ifndef DEDSERV
	context.function("font_load", l_font_load);
#endif
	
	context.function("map_is_loaded", l_map_is_loaded);
	context.function("console_register_command", l_console_register_command);
	
	context.function("game_players", l_game_players);
	lua_pushcfunction(context, l_game_playerIterator);
	playerIterator = context.createReference();
	
	context.function("game_local_player", l_game_localPlayer);
	context.function("game_get_closest_worm", l_game_getClosestWorm);
	
#ifndef DEDSERV
	context.function("clear_keybuf", l_clear_keybuf);
#endif
	
	context.function("map_is_blocked", l_map_isBlocked);
	context.function("map_is_particle_pass", l_map_isParticlePass);
	
	context.function("quit", l_quit);
	
#ifndef DEDSERV
	context.function("gui_find", l_gui_find);
#endif
	
	context.function("gfx_draw_box", l_gfx_draw_box);
	context.function("gfx_set_alpha", l_gfx_set_alpha);
	context.function("gfx_reset_blending", l_gfx_reset_blending);
	
	context.function("bind", l_bind);
	
	context.function("connect", l_connect);
	
	// Player method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	lua_pushstring(context, "kills");
	lua_pushcfunction(context, l_player_kills);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "deaths");
	lua_pushcfunction(context, l_player_deaths);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "name");
	lua_pushcfunction(context, l_player_name);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "say");
	lua_pushcfunction(context, l_player_say);
	lua_rawset(context, -3);
	
	lua_rawset(context, -3);
	playerMetaTable = context.createReference();
	
	// BaseObject method and metatable
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	addBaseObjectFunctions(context);
	
	lua_rawset(context, -3);
	baseObjectMetaTable = context.createReference();
	
	// Particle type method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	//addBaseObjectFunctions(context);
	

	lua_rawset(context, -3);
	partTypeMetaTable = context.createReference();
	
	// Particle method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	addBaseObjectFunctions(context);
	
	lua_pushstring(context, "set_angle");
	lua_pushcfunction(context, l_particle_setAngle);
	lua_rawset(context, -3);

	lua_pushstring(context, "get_angle");
	lua_pushcfunction(context, l_particle_getAngle);
	lua_rawset(context, -3);	

	lua_rawset(context, -3);
	particleMetaTable = context.createReference();
	
	// Worm method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	//NOT COMPATIBLE: addBaseObjectFunctions(context); // BaseWorm inherits from BaseObject
	
	lua_pushstring(context, "remove");
	lua_pushcfunction(context, l_worm_remove);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "pos");
	lua_pushcfunction(context, l_worm_pos);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "spd");
	lua_pushcfunction(context, l_worm_spd);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "push");
	lua_pushcfunction(context, l_worm_push);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "data");
	lua_pushcfunction(context, l_worm_data);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "get_player");
	lua_pushcfunction(context, l_worm_getPlayer);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "get_health");
	lua_pushcfunction(context, l_worm_getHealth);
	lua_rawset(context, -3);
	
	lua_pushstring(context, "shoot");
	lua_pushcfunction(context, l_worm_shoot);
	lua_rawset(context, -3);
		
	lua_rawset(context, -3);
	wormMetaTable = context.createReference();

#ifndef DEDSERV
	// Viewport method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	lua_pushstring(context, "get_bitmap");
	lua_pushcfunction(context, l_viewport_getBitmap);
	lua_rawset(context, -3);
	
	lua_rawset(context, -3);
	viewportMetaTable = context.createReference();

	// Font method and metatable
	

	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	lua_pushstring(context, "render");
	lua_pushcfunction(context, l_font_render);
	lua_rawset(context, -3);
		
	lua_rawset(context, -3);
	fontMetaTable = context.createReference();

	// GUI Wnd method and metatable
	
	guiWndMetaTable.resize(OmfgGUI::Context::WndCount);
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	addGUIWndFunctions(context);

	lua_rawset(context, -3);
	int ref = context.createReference();
	guiWndMetaTable[OmfgGUI::Context::Unknown] = ref;
	guiWndMetaTable[OmfgGUI::Context::Button] = ref;
	guiWndMetaTable[OmfgGUI::Context::Edit] = ref;
	guiWndMetaTable[OmfgGUI::Context::Group] = ref;
	
	// GUI List method and metatable
	
	lua_newtable(context);
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	addGUIWndFunctions(context);
	addGUIListFunctions(context);

	lua_rawset(context, -3);
	guiWndMetaTable[OmfgGUI::Context::List] = context.createReference();
#endif

	// Global metatable
	
	lua_pushvalue(context, LUA_GLOBALSINDEX);
	
	lua_newtable(context);
	lua_pushstring(context, "__index");
	lua_pushcfunction(context, l_load_script);
	lua_rawset(context, -3);
	
	lua_setmetatable(context, -2);
	
	// Bindings table and metatable
	lua_pushstring(context, "bindings");
	lua_newtable(context); // Bindings table
	
	lua_newtable(context); // Bindings metatable
	lua_pushstring(context, "__newindex");
	lua_pushcfunction(context, l_bind);
	lua_rawset(context, -3);
	
	lua_setmetatable(context, -2);

	lua_rawset(context, LUA_GLOBALSINDEX);
	
	cerr << "LuaBindings::init() done." << endl;
}

}
