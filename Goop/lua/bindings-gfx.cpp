#include "bindings-gfx.h"

#include "luaapi/types.h"

#include "../glua.h"

#ifndef DEDSERV
#include "../viewport.h"
#include "../gfx.h"
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
LuaReference bitmapMetaTable(0);
BlitterContext blitter;
#endif

/*! gfx_draw_box(bitmap, x1, y1, x2, y2, r, g, b)

	Draws a filled box with the corners (x1, y1) and (x2, y2)
	with the color (r, g, b) using the currently selected blender.
*/
int l_gfx_draw_box(lua_State* L)
{
#ifndef DEDSERV
	BITMAP* b = *static_cast<BITMAP **>(lua_touserdata(L, 1));
	
	int x1 = lua_tointeger(L, 2);
	int y1 = lua_tointeger(L, 3);
	int x2 = lua_tointeger(L, 4);
	int y2 = lua_tointeger(L, 5);
	int cr = lua_tointeger(L, 6);
	int cg = lua_tointeger(L, 7);
	int cb = lua_tointeger(L, 8);
	
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
	int alpha = lua_tointeger(L, 1);
	blitter.set(BlitterContext::alpha(), alpha);
#endif
	return 0;
}

int l_gfx_set_add(lua_State* L)
{
#ifndef DEDSERV
	int alpha = lua_tointeger(L, 1);
	blitter.set(BlitterContext::add(), alpha);
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
METHOD(Viewport, viewport_getBitmap,
	context.pushFullReference(*p->getBitmap(), bitmapMetaTable);
	return 1;
)

METHOD(BITMAP, bitmap_w,
	lua_pushinteger(context, p->w);
	return 1;
)

METHOD(BITMAP, bitmap_h,
	lua_pushinteger(context, p->h);
	return 1;
)

#endif


void initGfx()
{
	LuaContext& context = lua;

	context.functions()
		("gfx_draw_box", l_gfx_draw_box)
		("gfx_set_alpha", l_gfx_set_alpha)
		("gfx_set_add", l_gfx_set_add)
		("gfx_reset_blending", l_gfx_reset_blending)
	;


#ifndef DEDSERV
	// Viewport method and metatable
	
	CLASS(viewport,
		("get_bitmap", l_viewport_getBitmap)
	)

	// Bitmap method and metatable
	
	CLASS(bitmap,
		("w", l_bitmap_w)
		("h", l_bitmap_h)
	)
	
#endif	
}

}
