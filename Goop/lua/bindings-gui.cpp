#include "bindings-gui.h"

#include "types.h"

#include "../glua.h"

#ifndef DEDSERV
#include "../menu.h"
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

namespace LuaBindings
{

#ifndef DEDSERV

std::vector<LuaReference> guiWndMetaTable;

/*! gui_load_xml(name)

	Loads an XML GUI hierarchy by name.
	**Note that the .xml extension is not included in the name**
*/
int l_gui_loadxml(lua_State* L)
{
	OmfgGUI::Wnd* loadTo = OmfgGUI::menu.getRoot();
	
	int params = lua_gettop(L);
	
	if(params > 0)
	{
		char const* name = lua_tostring(L, 1);
		
		if(!name)
			return 1;

		XMLFile f;
		if(xmlLocator.load(&f, name))
		{
			OmfgGUI::menu.buildFromXML(f.f, loadTo);
			return 0;
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
	bool passive = false;
	
	int params = lua_gettop(L);
	
	if(params > 0)
	{
		char const* name = lua_tostring(L, 1);
		
		if(!name)
			return 1;
		
		if(params > 1)
			passive = lua_toboolean(L, 2);

		GSSFile f;
		if(gssLocator.load(&f, name))
		{
			OmfgGUI::menu.loadGSS(f.f);
			if(!passive)
				OmfgGUI::menu.updateGSS();
			return 0;
		}
	}
	
	return 0;
}

/*! gui_find(name)

	Finds a named GUI element and returns it as
	a Wnd object.
*/

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


/*! Wnd:attribute(name)

	Returns the value of the attribute //name// of the window.
*/
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

/*! Wnd:set_visibility(value)

	If //value// is false, hides the window.
	Otherwise makes it visible.
*/

int l_gui_wnd_set_visibility(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));
	
	p->setVisibility(lua_toboolean(L, 2));

	return 0;
}

/*! Wnd:is_visible()

	Returns true if the window is visible.
	Otherwise false.
*/
int l_gui_wnd_is_visible(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));
	
	lua_pushboolean(L, p->isVisibile());

	return 1;
}

/*! Wnd:get_text()

	Returns the text of the window.
*/
int l_gui_wnd_get_text(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));

	if(p)
		lua_pushstring(L, p->getText().c_str());
	else
		lua_pushnil(L);
	
	return 1;
}

/*! Wnd:set_text(text)

	Sets the text of the window to //text//.
*/
int l_gui_wnd_set_text(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));
	
	char const* s = lua_tostring(L, 2);
	if(s)
		p->setText(s);
	
	return 0;
}

/*! Wnd:focus()

	Focuses the window.
*/
int l_gui_wnd_focus(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));

	OmfgGUI::menu.setFocus(p);
	
	return 0;
}

/*! Wnd:activate()

	Makes the window recieve input from now on until it's deactivated.
*/
int l_gui_wnd_activate(lua_State* L)
{
	OmfgGUI::Wnd* p = *static_cast<OmfgGUI::Wnd **>(lua_touserdata (L, 1));

	p->doSetActivation(true);
	
	return 0;
}

//! List inherits Wnd

/*! List:insert(columns...)

	Inserts an item into the list with the column values passed.
*/
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

/*! List:clear()

	Removes all items in the list.
*/
int l_gui_list_clear(lua_State* L)
{
	OmfgGUI::List* p = *static_cast<OmfgGUI::List **>(lua_touserdata (L, 1));
	
	p->clear();

	return 0;
}

/*! List:sort(column)

	Sorts the list by the column passed. //column// is a zero-based index.
*/
int l_gui_list_sort(lua_State* L)
{
	OmfgGUI::List* p = *static_cast<OmfgGUI::List **>(lua_touserdata (L, 1));
	unsigned int column = static_cast<unsigned int>(lua_tonumber(L, 2));
	
	p->sortNumerically(column);

	return 0;
}

/*! List:add_column(title, width)

	Adds a column to a list with the title //title//.
	//width// is a value in (0, 1) that specifies the width of this column proportional to the total width of the list.
*/
int l_gui_list_add_column(lua_State* L)
{
	OmfgGUI::List* p = *static_cast<OmfgGUI::List **>(lua_touserdata (L, 1));
	char const* name = lua_tostring(L, 2);
	lua_Number widthFactor = lua_tonumber(L, 3);
	
	p->addColumn(OmfgGUI::List::ColumnHeader(name, widthFactor));

	return 0;
}

#endif


#ifndef DEDSERV
void addGUIWndFunctions(LuaContext& context)
{
	context.tableFunction("attribute", l_gui_wnd_attribute);
	context.tableFunction("set_visibility", l_gui_wnd_set_visibility);
	context.tableFunction("is_visible", l_gui_wnd_is_visible);
	context.tableFunction("get_text", l_gui_wnd_get_text);
	context.tableFunction("set_text", l_gui_wnd_set_text);
	context.tableFunction("focus", l_gui_wnd_focus);
	context.tableFunction("activate", l_gui_wnd_activate);
}

void addGUIListFunctions(LuaContext& context)
{
	context.tableFunction("insert", l_gui_list_insert);
	context.tableFunction("clear", l_gui_list_clear);
	context.tableFunction("add_column", l_gui_list_add_column);
	context.tableFunction("sort", l_gui_list_sort);
}
#endif

void initGUI()
{
#ifndef DEDSERV
	LuaContext& context = lua;
	
	// GUI Wnd method and metatable

	context.function("gui_load_gss", l_gui_loadgss);
	context.function("gui_load_xml", l_gui_loadxml);
	context.function("gui_find", l_gui_find);

	guiWndMetaTable.resize(OmfgGUI::Context::WndCount);
	
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	addGUIWndFunctions(context);

	lua_rawset(context, -3);
	LuaReference ref = context.createReference();
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
	
}

}
