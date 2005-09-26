#include "bindings-math.h"

#include "types.h"
#include "../vec.h"
#include "../angle.h"
#include "../glua.h"
#include "../math_func.h"

#include <cmath>
#include <string>
#include <iostream>

using std::cerr;
using std::endl;
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
using boost::lexical_cast;

namespace LuaBindings
{

inline lua_Number luaL_checknumber(lua_State *L, int narg)
{
	lua_Number d = lua_tonumber(L, narg);
	if(d == 0 && !lua_isnumber(L, narg))
		; // TODO: tag_error(L, narg, LUA_TNUMBER);
	return d;
}

/*! sqrt(n)

	Returns the squareroot of n.
*/
int l_sqrt(lua_State* L)
{
	lua_pushnumber(L, sqrt(luaL_checknumber(L, 1)));
	return 1;
}

/*! abs(n)

	Returns the absolute value of n.
*/
int l_abs(lua_State* L)
{
	lua_pushnumber(L, fabs(luaL_checknumber(L, 1)));
	return 1;
}

/*! floor(n)

	Returns the number n rounded down towards infinity.
*/
int l_floor(lua_State* L)
{
	lua_pushnumber(L, floor(luaL_checknumber(L, 1)));
	return 1;
}

/*! randomint(l, u)

	Returns a random integer in the interval [l, u].
*/
int l_randomint(lua_State* L)
{
	int l = (int)luaL_checknumber(L, 1);
	int u = (int)luaL_checknumber(L, 2);
	
	lua_pushnumber(L, l + (unsigned int)(rndgen()) % (u - l + 1));
	
	return 1;
}

/*! randomfloat(l, u)

	Returns a random floating point number in the interval [l, u].
*/
int l_randomfloat(lua_State* L)
{
	lua_Number l = luaL_checknumber(L, 1);
	lua_Number u = luaL_checknumber(L, 2);
	
	lua_pushnumber(L, l + rnd() * (u - l));
	
	return 1;
}

/*! vector_diff(x1, y1, x2, y2)

	Returns a tuple equal to (x2 - x1, y2 - y1)
*/
int l_vector_diff(lua_State* L)
{
	lua_pushnumber(L, lua_tonumber(L, 3) - lua_tonumber(L, 1));
	lua_pushnumber(L, lua_tonumber(L, 4) - lua_tonumber(L, 2));
	
	return 2;
}

/*! vector_distance(x1, y1, x2, y2)

	Returns the distance from (x1, y1) to (x2, y1)
*/
int l_vector_distance(lua_State* L)
{
	lua_Number vx = lua_tonumber(L, 3) - lua_tonumber(L, 1);
	lua_Number vy = lua_tonumber(L, 4) - lua_tonumber(L, 2);
	vx *= vx;
	vy *= vy;
	lua_pushnumber(L, sqrt(vx + vy));
	
	return 1;
}

/*! vector_direction(x1, y1, x2, y2)

	Returns the angle between (x1, y1) and (x2, y1)
	in degrees.
*/
int l_vector_direction(lua_State* L)
{
	lua_Number vx = lua_tonumber(L, 3) - lua_tonumber(L, 1);
	lua_Number vy = lua_tonumber(L, 4) - lua_tonumber(L, 2);

	lua_pushnumber(L, rad2deg(atan2(vx, -vy)) );
	
	return 1;
}

/*! vector_add(x1, y1, x2, y2)

	Returns a tuple equal to (x1 + x2, y1 + y2)
*/
int l_vector_add(lua_State* L)
{
	lua_pushnumber(L, lua_tonumber(L, 1) + lua_tonumber(L, 3));
	lua_pushnumber(L, lua_tonumber(L, 2) + lua_tonumber(L, 4));
	
	return 2;
}


/*! angle_diff(a, b)

	Returns the relative angle in (-180, 180) between
	angle //a// and //b// such that (a + angle_diff(a, b)) = b (mod 360)
*/
int l_angle_diff(lua_State* L)
{
	AngleDiff diff(AngleDiff(lua_tonumber(L, 1)).relative(AngleDiff(lua_tonumber(L, 2))));
	
	lua_pushnumber(L, diff.toDeg());
	
	return 1;
}

/*! angle_clamp(angle)

	Returns //angle// normalized to [0, 360).
*/
int l_angle_clamp(lua_State* L)
{
	//lua_Number ang = lua_tonumber(L, 1);
	Angle ang((double)lua_tonumber(L, 1));
	
	ang.clamp();
	
	lua_pushnumber(L, ang.toDeg());
	
	return 1;
}

/*! angle_vector(angle[, length = 1])

	Returns a tuple representing the angle
	with length //length//.
*/
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

void initMath()
{
	LuaContext& context = lua;
	
	context.function("sqrt", l_sqrt);
	context.function("abs", l_abs);
	context.function("floor", l_floor);
	
	context.function("randomint", l_randomint);
	context.function("randomfloat", l_randomfloat);
	
	context.function("vector_diff", l_vector_diff);
	context.function("vector_distance", l_vector_distance);
	context.function("vector_direction", l_vector_direction);
	context.function("vector_add", l_vector_add);
	
	context.function("angle_clamp", l_angle_clamp);
	context.function("angle_diff", l_angle_diff);
	context.function("angle_vector", l_angle_vector);
}

}
