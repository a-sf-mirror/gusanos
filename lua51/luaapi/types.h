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
	
	void reset()
	{
		idx = 0;
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
		
#define LBINOP(type_, name_, body_) \
	int l_##name_(lua_State* L_) { \
		LuaContext context(L_); \
		type_* a = static_cast<type_ *>(lua_touserdata (context, 1)); \
		if(!a) return 0; \
		type_* b = static_cast<type_ *>(lua_touserdata (context, 2)); \
		if(!b) return 0; \
		body_ }

#define CLASS(name_, body_) { \
	lua_newtable(context); \
	lua_pushstring(context, "__index"); \
	lua_newtable(context); \
	context.tableFunctions() \
		body_ ; \
	lua_rawset(context, -3); \
	name_##MetaTable = context.createReference(); }
	
#define CLASS_(name_, body_) { \
	lua_newtable(context); \
	lua_pushstring(context, "__index"); \
	lua_newtable(context); \
	context.tableFunctions() \
		body_ ; \
	lua_rawset(context, -3); \
	name_::metaTable = context.createReference(); }
	
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
	
#define CLASSM_(name_, meta_, body_) { \
	lua_newtable(context); \
	context.tableFunctions() \
		meta_ ; \
	lua_pushstring(context, "__index"); \
	lua_newtable(context); \
	context.tableFunctions() \
		body_ ; \
	lua_rawset(context, -3); \
	name_::metaTable = context.createReference(); }

#define ENUM(name_, body_) { \
	lua_pushstring(context, #name_); \
	lua_newtable(context); \
	context.tableItems() \
		body_ ; \
	lua_rawset(context, LUA_GLOBALSINDEX); }
	
#define REQUEST_TABLE(name_, func_) { \
	lua_pushstring(context, name_); \
	lua_newtable(context); \
	lua_newtable(context); \
	lua_pushstring(context, "__index"); \
	lua_pushcfunction(context, func_); \
	lua_rawset(context, -3); \
	lua_setmetatable(context, -2); \
	lua_rawset(context, LUA_GLOBALSINDEX); }

#define LUA_NEW_(t_, param_, lua_) \
({ \
	LuaContext& lua_once_ = lua_; \
	lua_once_.pushRegObject(t_::metaTable); \
	void* space_ = lua_once_.pushObject(sizeof(t_)); \
	t_* p_ = new (space_) t_ param_; \
	p_->luaReference = lua_once_.createReference(); \
	p_; \
})

#define LUA_NEW_KEEP(t_, param_, lua_) \
({ \
	LuaContext& lua_once_ = lua_; \
	lua_once_.pushRegObject(t_::metaTable); \
	void* space_ = lua_once_.pushObject(sizeof(t_)); \
	t_* p_ = new (space_) t_ param_; \
	lua_pushvalue(lua_once_, -1); \
	p_->luaReference = lua_once_.createReference(); \
	p_; \
})

#define lua_new(t_, param_, lua_) \
({ \
	LuaContext& lua_once_ = lua_; \
	lua_once_.push(t_::metaTable); \
	void* space_ = lua_once_.pushObject(sizeof(t_)); \
	t_* p_ = new (space_) t_ param_; \
	p_->luaReference = lua_once_.createReference(); \
	p_; \
})

#define lua_new_m(t_, param_, lua_, metatable_) \
({ \
	LuaContext& lua_once_ = lua_; \
	lua_once_.push(metatable_); \
	void* space_ = lua_once_.pushObject(sizeof(t_)); \
	t_* p_ = new (space_) t_ param_; \
	p_->luaReference = lua_once_.createReference(); \
	p_; \
})

#define lua_new_m_keep(t_, param_, lua_, metatable_) \
({ \
	LuaContext& lua_once_ = lua_; \
	lua_once_.push(metatable_); \
	void* space_ = lua_once_.pushObject(sizeof(t_)); \
	t_* p_ = new (space_) t_ param_; \
	lua_pushvalue(lua_once_, -1); \
	p_->luaReference = lua_once_.createReference(); \
	p_; \
})

#endif //LUA_TYPES_H
