#include "bindings-gfx.h"

#include "types.h"

#include "../glua.h"

#ifndef DEDSERV
#include "../viewport.h"
#include "../gfx.h"
#include "../blitters/context.h"
#endif

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
	
#ifndef DEDSERV
LuaReference viewportMetaTable(0);
BlitterContext blitter;
#endif

/*! gfx_draw_box(bitmap, x1, y1, x2, y2, r, g, b)

	Draws a filled box with the corners (x1, y1) and (x2, y2)
	with the color (r, g, b) using the currently selected blender.
*/
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

/*! gfx_set_alpha(alpha)

	Activates the alpha blender.
	//alpha// is a value in [0, 255] that specifies the opacity
	of things drawn after this is called.
*/
int l_gfx_set_alpha(lua_State* L)
{
#ifndef DEDSERV
	int alpha = (int)lua_tonumber(L, 1);
	blitter.set(BlitterContext::alpha(), alpha);
#endif
	return 0;
}

/*! gfx_reset_blending()

	Deactivates any blender that was active.
	Everything drawn after this is called will be drawn solid.
*/
int l_gfx_reset_blending(lua_State* L)
{
#ifndef DEDSERV
	blitter.set(BlitterContext::none());
#endif
	return 0;
}

#ifndef DEDSERV

/*! Viewport:get_bitmap()

	Returns the HUD bitmap of this viewport.
*/
int l_viewport_getBitmap(lua_State* L)
{
	Viewport* p = *static_cast<Viewport **>(lua_touserdata (L, 1));

	lua_pushlightuserdata(L, (void *)p->getBitmap());
	return 1;
}

#endif


void initGfx()
{
	LuaContext& context = lua;

	context.function("gfx_draw_box", l_gfx_draw_box);
	context.function("gfx_set_alpha", l_gfx_set_alpha);
	context.function("gfx_reset_blending", l_gfx_reset_blending);


#ifndef DEDSERV
	// Viewport method and metatable
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	context.tableFunction("get_bitmap", l_viewport_getBitmap);
	
	lua_rawset(context, -3);
	viewportMetaTable = context.createReference();

#endif	
}

}
