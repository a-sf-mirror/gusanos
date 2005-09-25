#ifndef LUA_TYPES_H
#define LUA_TYPES_H

struct LuaReference
{
	LuaReference()
	{
	}
	
	explicit LuaReference(int idx_)
	: idx(idx_)
	{
	}
	
	operator bool()
	{
		return idx != 0;
	}
	
	int idx;
};

#endif //LUA_TYPES_H
