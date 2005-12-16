#include "bindings-gui.h"

#include "luaapi/types.h"

#ifndef DEDSERV
#include "omfggui.h"
#include "omfggui_windows.h"
#endif
#include "util/macros.h"

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

namespace LuaBindings
{

#ifndef DEDSERV

//std::vector<LuaReference> guiWndMetaTable;
//LuaReference gui_listIterator;

/*! gui_load_xml(name)

	Loads an XML GUI hierarchy by name.
	**Note that the .xml extension is not included in the name**
*/
int l_gui_loadxml(lua_State* L)
{
	LuaContext context(L);
	
	OmfgGUI::Context& gui = *static_cast<OmfgGUI::Context *>(lua_touserdata(context, lua_upvalueindex(1)));

	int params = lua_gettop(context);
	                                                     
	if(params > 0)
	{
		char const* name = lua_tostring(context, 1);
		
		if(!name)
			return 1;
		
		OmfgGUI::Wnd* loadTo = gui.getRoot();
		
		if(params > 1)
		{
			loadTo = static_cast<OmfgGUI::Wnd *>(lua_touserdata(context, 2));
		}
			
		OmfgGUI::Wnd* w = gui.loadXMLFile(name, loadTo);
		if(w)
		{
			context.push(w->luaReference);
			return 1;
		}
	}
	
	return 0;
}

/*! gui_load_gss(name[, passive = false])

	Loads a GSS GUI sheet by name.
	If the parameter passive is supplied and is set to true, the styling of existing GUI
	elements won't be updated.
	**Note that the .gss extension is not included in the name**
*/

int l_gui_loadgss(lua_State* L)
{
	OmfgGUI::Context& gui = *static_cast<OmfgGUI::Context *>(lua_touserdata(L, lua_upvalueindex(1)));
	
	bool passive = false;
	
	int params = lua_gettop(L);
	
	if(params > 0)
	{
		char const* name = lua_tostring(L, 1);
		
		if(!name)
			return 1;
		
		if(params > 1)
			passive = lua_toboolean(L, 2);
			
		gui.loadGSSFile(name, passive);
	}
	
	return 0;
}

/*! gui_find(name)

	Finds a named GUI element and returns it as
	a Wnd object.
*/

int l_gui_find(lua_State* L)
{
	OmfgGUI::Context& gui = *static_cast<OmfgGUI::Context *>(lua_touserdata(L, lua_upvalueindex(1)));
	
	LuaContext context(L);
	
	char const* s = lua_tostring(context, 1);
	OmfgGUI::Wnd* w = gui.findNamedWindow(s);
	if(!w)
		return 0;

	context.push(w->luaReference);

	return 1;
}

int l_gui_windows_index(lua_State* L)
{
	OmfgGUI::Context& gui = *static_cast<OmfgGUI::Context *>(lua_touserdata(L, lua_upvalueindex(1)));
	
	LuaContext context(L);
	
	char const* s = lua_tostring(context, 2);
	if(!s)
		return 0;
	OmfgGUI::Wnd* w = gui.findNamedWindow(s);
	if(!w)
		return 0;

	context.push(w->luaReference);

	return 1;
}

/*! Wnd:attribute(name)

	Returns the value of the attribute //name// of the window.
*/
LMETHOD(OmfgGUI::Wnd, gui_wnd_attribute,
	char const* name = lua_tostring(context, 2);
	std::string res;
	if(p->getAttrib(name, res))
		lua_pushstring(context, res.c_str());
	else
		return 0;
	
	return 1;
)

/*! Wnd:set_visibility(value)

	If //value// is false, hides the window.
	Otherwise makes it visible.
*/

LMETHOD(OmfgGUI::Wnd, gui_wnd_set_visibility,
	p->setVisibility(lua_toboolean(context, 2));
	return 0;
)

/*! Wnd:switch_to()

	Hides all windows, except this one, in the parent window.
*/
LMETHOD(OmfgGUI::Wnd, gui_wnd_switch_to,
	context.push(p->switchTo());
	return 1;
)

/*! Wnd:is_visible()

	Returns true if the window is visible.
	Otherwise false.
*/
LMETHOD(OmfgGUI::Wnd, gui_wnd_is_visible,
	lua_pushboolean(context, p->isVisibile());
	return 1;
)

LMETHOD(OmfgGUI::Wnd, gui_wnd_is_active,
	lua_pushboolean(context, p->isActive());
	return 1;
)

LMETHOD(OmfgGUI::Wnd, gui_wnd_deactivate,
	p->doSetActivation(false);
	return 0;
)

/*! Wnd:get_text()

	Returns the text of the window.
*/
LMETHOD(OmfgGUI::Wnd, gui_wnd_get_text,
	if(p)
		lua_pushstring(context, p->getText().c_str());
	else
		return 0;
	
	return 1;
)

/*! Wnd:set_text(text)

	Sets the text of the window to //text//.
*/
LMETHOD(OmfgGUI::Wnd, gui_wnd_set_text,
	char const* s = lua_tostring(context, 2);
	if(s)
		p->setText(s);
	
	return 0;
)

/*! Wnd:focus()

	Focuses the window.
*/
LMETHOD(OmfgGUI::Wnd, gui_wnd_focus,
	p->focus();
	return 0;
)

/*! Wnd:set_sub_focus()

	Sets a window that will be focused when
	this window is focused.
*/
LMETHOD(OmfgGUI::Wnd, gui_wnd_set_sub_focus,
	OmfgGUI::Wnd* sub = static_cast<OmfgGUI::Wnd*>(lua_touserdata(context, 2));
	
	// Make sure that 'sub' is a child of 'p'
	OmfgGUI::Wnd* parent = sub->getParent();
	while(parent && parent != p)
		parent = parent->getParent();
	
	if(!parent)
		return 0; // 'sub' isn't a child
	
	parent = sub->getParent();
	while(parent)
	{
		parent->setSubFocus(sub);
		if(parent == p)
			return 0;
		sub = parent;
		parent = parent->getParent();
	}

	return 0;
)

/*! Wnd:activate()

	Makes the window recieve input from now on until it's deactivated.
*/
LMETHOD(OmfgGUI::Wnd, gui_wnd_activate,
	p->doSetActivation(true);
	return 0;
)

LMETHOD(OmfgGUI::Wnd, gui_wnd_bind,
	char const* cb = lua_tostring(context, 2);
	if(!cb) return 0;
	lua_pushvalue(context, 3);
	context.push(p->registerCallback(cb, context.createReference()));
	return 1;
)

LMETHOD(OmfgGUI::Wnd, gui_wnd_child,
	char const* name = lua_tostring(context, 2);
	if(!name) return 0;
	OmfgGUI::Wnd* ch = p->getChildByName(name);
	if(!ch) return 0;
	context.push(ch->luaReference);
	return 1;
)

//! Edit inherits Wnd

LMETHOD(OmfgGUI::Edit, gui_edit_set_lock,
	p->setLock(lua_toboolean(context, 1));
	return 0;
)

//! List inherits Wnd

/*! List:insert(columns...)

	Inserts an item into the list with the column values passed.
*/
LMETHOD(OmfgGUI::List, gui_list_insert,

	int c = lua_gettop(context);
	OmfgGUI::List::Node* n = LUA_NEW_KEEP(OmfgGUI::List::Node, (""), context);
	p->push_back(n);
	for(int i = 2; i <= c; ++i)
		n->setText(i - 2, lua_tostring(context, i));
	
	return 1;
)

LMETHOD(OmfgGUI::List, gui_list_each,

	context.pushRegObject("gui_listIterator");
	context.push(p->getFirstNode()->luaReference);
	lua_pushnil(context);

	return 3;
)

int l_gui_listIterator(lua_State* L)
{
	LuaContext context(L);
	
	if(lua_isnil(context, 2))
		lua_pushvalue(context, 1);
	else
	{
		OmfgGUI::List::Node* i = static_cast<OmfgGUI::List::Node *>(lua_touserdata(context, 2));
	
		i = OmfgGUI::List::Node::getNextVisible(i);
		
		if(!i)
			return 0;
	
		context.push(i->luaReference);
	}
	
	return 1;
}

/*! List:subinsert(node, columns...)

	Inserts an item into the list, under another node, with the column values passed.
*/
LMETHOD(OmfgGUI::List, gui_list_subinsert,

	//TODO: Check metatable of node
	OmfgGUI::List::Node* parent = static_cast<OmfgGUI::List::Node *>(lua_touserdata(context, 2));
	
	//if(!p->verify(parent))
	//	return 0;
			
	int c = lua_gettop(context);
	//void* mem = lua_newuserdata(context, sizeof(LuaListNode));
	//lua_pushvalue(context, -1);
	OmfgGUI::List::Node* n = LUA_NEW_KEEP(OmfgGUI::List::Node, (""), context);
	//LuaListNode* n = new (mem) LuaListNode(context.createReference(), "");
	p->push_back(n, parent);
	for(int i = 3; i <= c; ++i)
		n->setText(i - 3, lua_tostring(context, i));
	
	return 1;
)

/*! List:clear()

	Removes all items in the list.
*/
LMETHOD(OmfgGUI::List, gui_list_clear,
	p->clear();
	return 0;
)

/*! List:sort(criteria)

	Sorts the list by the column passed or by a comparer function.
	
	If //criteria// is an integer, the list is sorted numerically by the column with that zero-based index.
	
	If //criteria// is a function, it's used as a comparer function that gets passed
	two parameters. It should return true if parameter 1 is supposed to appear before
	parameter 2.
*/
LMETHOD(OmfgGUI::List, gui_list_sort,
	if(lua_isnumber(context, 2))
	{
		unsigned int column = static_cast<unsigned int>(lua_tointeger(context, 2));
		
		p->sortNumerically(column);
	}
	else if(lua_isfunction(context, 2))
	{
		context.pushvalue(2);
		LuaReference ref = context.createReference();
		p->sortLua(ref);
		context.destroyReference(ref);
	}

	return 0;
)

LMETHOD(OmfgGUI::List, gui_list_selection,
	if(!p->getMainSel())
		return 0;
		
	int c = 0;
	const_foreach(i, p->getMainSel()->getFields())
	{
		context.push(*i); ++c;
	}
	return c;
)

LMETHOD(OmfgGUI::List, gui_list_main_selection,
	if(OmfgGUI::List::Node* n = p->getMainSel())
	{
		context.push(n->luaReference);
		return 1;
	}
	return 0;
)

LMETHOD(OmfgGUI::List, gui_list_scroll_bottom,
	p->scrollBottom();
	return 0;
)

/*! List:add_column(title, width)

	Adds a column to a list with the title //title//.
	//width// is a value in (0, 1) that specifies the width of this column proportional to the total width of the list.
*/
LMETHOD(OmfgGUI::List, gui_list_add_column,
	char const* name = lua_tostring(context, 2);
	lua_Number widthFactor = lua_tonumber(context, 3);
	
	p->addColumn(OmfgGUI::List::ColumnHeader(name, widthFactor));

	return 0;
)

/*! ListNode:is_selected()

	Returns true if the node is selected, otherwise false.
*/
LMETHOD(OmfgGUI::List::Node, gui_list_node_is_selected,
	context.push(p->selected);
	return 1;
)

/*! ListNode:data()

	Returns a table associated with this node.
*/
LMETHOD(OmfgGUI::List::Node, gui_list_node_data,
	if(p->luaData)
	{
		context.pushReference(p->luaData);
	}
	else
	{
		lua_newtable(context);
		lua_pushvalue(context, -1);
		p->luaData = context.createReference();
	}
	
	return 1;
)

#endif


#ifndef DEDSERV
void addGUIWndFunctions(LuaContext& context)
{
	context.tableFunctions()
		("attribute", l_gui_wnd_attribute)
		("set_visibility", l_gui_wnd_set_visibility)
		("is_visible", l_gui_wnd_is_visible)
		("is_active", l_gui_wnd_is_active)
		("get_text", l_gui_wnd_get_text)
		("set_text", l_gui_wnd_set_text)
		("focus", l_gui_wnd_focus)
		("set_sub_focus", l_gui_wnd_set_sub_focus)
		("activate", l_gui_wnd_activate)
		("deactivate", l_gui_wnd_deactivate)
		("child", l_gui_wnd_child)
		("switch_to", l_gui_wnd_switch_to)
		//("bind", l_gui_wnd_bind)
	;
}

void addGUIListFunctions(LuaContext& context)
{
	context.tableFunctions()
		("insert", l_gui_list_insert)
		("subinsert", l_gui_list_subinsert)
		("clear", l_gui_list_clear)
		("add_column", l_gui_list_add_column)
		("sort", l_gui_list_sort)
		("selection", l_gui_list_selection)
		("main_selection", l_gui_list_main_selection)
		("each", l_gui_list_each)
		("scroll_bottom", l_gui_list_scroll_bottom)
	;
}

void addGUIEditFunctions(LuaContext& context)
{
	context.tableFunctions()
		("set_lock", l_gui_edit_set_lock)
	;
}

void GUIWndMetatable(LuaContext& context)
{
	lua_newtable(context);
	lua_pushcfunction(context, l_gui_wnd_bind);
	lua_setfield(context, -2, "__newindex");
}

#endif

void initGUI(OmfgGUI::Context& gui, LuaContext& context)
{
#ifndef DEDSERV

	// GUI Wnd method and metatable

	lua_pushlightuserdata(context, static_cast<void *>(&gui));
	context.function("gui_load_gss", l_gui_loadgss, 1);
	lua_pushlightuserdata(context, static_cast<void *>(&gui));
	context.function("gui_load_xml", l_gui_loadxml, 1);
	lua_pushlightuserdata(context, static_cast<void *>(&gui));
	context.function("gui_find", l_gui_find, 1);

	GUIWndMetatable(context);
	lua_pushstring(context, "__index");

	lua_newtable(context);
	
	addGUIWndFunctions(context);

	lua_rawset(context, -3);
	//LuaReference ref = context.createReference();
	context.regObjectKeep(OmfgGUI::Wnd::metaTable);
	context.regObjectKeep(OmfgGUI::Button::metaTable);
	
	context.regObject(OmfgGUI::Group::metaTable);
	
	// GUI Edit method and metatable
	
	GUIWndMetatable(context);
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	addGUIWndFunctions(context);
	addGUIEditFunctions(context);
	
	lua_rawset(context, -3);
	context.regObject(OmfgGUI::Edit::metaTable);
	
	// GUI List method and metatable
	
	GUIWndMetatable(context);
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	addGUIWndFunctions(context);
	addGUIListFunctions(context);

	lua_rawset(context, -3);
	context.regObject(OmfgGUI::List::metaTable);
	
	// GUI List node method and metatable
	
	lua_newtable(context);
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	context.tableFunctions()
		("is_selected", l_gui_list_node_is_selected)
		("data", l_gui_list_node_data)
	;
	
	lua_rawset(context, -3);
	context.regObject(OmfgGUI::List::Node::metaTable);
	
	context.push(l_gui_listIterator);
	context.regObject("gui_listIterator");
#endif
}

}
