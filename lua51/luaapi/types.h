#ifndef LUA_TYPES_H
#define LUA_TYPES_H

struct LuaReference
{
	LuaReference()
	: idx(0)
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

#define METHOD(type_, name_, body_) \
	int l_##name_(lua_State* L_) { \
		LuaContext context(L_); \
		void* p2 = lua_touserdata (context, 1); \
		if(!p2) return 0; \
		type_* p = *static_cast<type_ **>(p2); \
		body_ }

#define LMETHOD(type_, name_, body_) \
	int l_##name_(lua_State* L_) { \
		LuaContext context(L_); \
		type_* p = static_cast<type_ *>(lua_touserdata (context, 1)); \
		if(!p) return 0; \
		body_ }
		
#define BINOP(type_, name_, body_) \
	int l_##name_(lua_State* L_) { \
		LuaContext context(L_); \
		void* a_ = lua_touserdata (context, 1); \
		if(!a_) return 0; \
		void* b_ = lua_touserdata (context, 2); \
		if(!b_) return 0; \
		type_* a = *static_cast<type_ **>(a_); \
		type_* b = *static_cast<type_ **>(b_); \
		body_ }
		
#define CLASS(name_, body_) { \
	lua_newtable(context); \
	lua_pushstring(context, "__index"); \
	lua_newtable(context); \
	context.tableFunctions() \
		body_ ; \
	lua_rawset(context, -3); \
	name_##MetaTable = context.createReference(); }
	
#define CLASSM(name_, meta_, body_) { \
	lua_newtable(context); \
	context.tableFunctions() \
		meta_ ; \
	lua_pushstring(context, "__index"); \
	lua_newtable(context); \
	context.tableFunctions() \
		body_ ; \
	lua_rawset(context, -3); \
	name_##MetaTable = context.createReference(); }

#define ENUM(name_, body_) { \
	lua_pushstring(context, #name_); \
	lua_newtable(context); \
	context.tableItems() \
		body_ ; \
	lua_rawset(context, LUA_GLOBALSINDEX); }

#define LUA_NEW_(t_, param_, lua_) \
({ \
	LuaContext& lua = lua_; \
	lua.pushRegObject(t_::metaTable); \
	void* space_ = lua.pushObject(sizeof(t_)); \
	t_* p_ = new (space_) t_ param_; \
	p_->luaReference = lua.createReference(); \
	p_; \
})

#define LUA_NEW_KEEP(t_, param_, lua_) \
({ \
	LuaContext& lua = lua_; \
	lua.pushRegObject(t_::metaTable); \
	void* space_ = lua.pushObject(sizeof(t_)); \
	t_* p_ = new (space_) t_ param_; \
	lua_pushvalue(lua, -1); \
	p_->luaReference = lua.createReference(); \
	p_; \
})

#endif //LUA_TYPES_H
