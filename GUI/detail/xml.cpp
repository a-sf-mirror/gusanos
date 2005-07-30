#include "xml-grammar.h"
#include "context.h"
#include "wnd.h"
#include "list.h"
#include "button.h"
#include <sstream>
#include <iostream>

using namespace std;

namespace OmfgGUI
{
	

struct GSSHandler
{
	GSSHandler(Context::GSSselectorMap& style_)
	: style(style_)
	{
		
	}
	
	void error(std::string err)
	{
		cout << err << endl;
	}
	
	void selector(std::string const& tagLabel, std::string const& className, std::string const& id, std::string const& state
	, std::string const& property, std::vector<std::string> const& value)
	{
		Context::GSSpropertyMap& dest = style.insert(tagLabel).insert(className).insert(id).insert(state);

		dest[property] = value;
	}
	
	Context::GSSselectorMap& style;
};

void Context::loadGSS(std::istream& s)
{
	GSSHandler handler(m_gss);
	gssSheet(s, handler);
}

struct XMLHandler
{
	struct Tag
	{
		Tag(std::string const& label_)
		: label(label_)
		{
		}
		
		std::string label;
		std::map<std::string, std::string> attributes;
	};
	
	struct WndInfo
	{
		WndInfo(Wnd* wnd_)
		: wnd(wnd_)
		{
			/* TODO
			curX = wnd->getFormatting().spacing;
			curY = wnd->getFormatting().spacing;*/
			nextY = curY;
		}
		
		Wnd* wnd;

		int  curX, curY;
		int  nextY;
	};
	
	XMLHandler(Context& context_, Wnd* dest_, Context::GSSselectorMap& style_)
	: tag(""), context(context_), style(style_)
	{
		windows.push(WndInfo(dest_));
	}
	
	void error(std::string err)
	{
		cout << err << endl;
	}
	
	void beginTag(std::string const& label)
	{
		cerr << "Tag: " << label << endl;
		tag = Tag(label);
	}
	
	void beginAttributes()
	{
	}
	
	void attribute(std::string const& name, std::string const& value)
	{
		tag.attributes[name] = value;
	}
	
	std::string const& getAttrib(std::string const& name, std::string const& def)
	{
		std::map<std::string, std::string>::iterator i = tag.attributes.find(name);
		if(i == tag.attributes.end())
			return def;
		return i->second;
	}

	void endAttributes()
	{
		std::string className = getAttrib("class", "");
		std::string label = getAttrib("label", "");
		std::string id = getAttrib("id", "");
				
		Wnd* newWindow = 0;
					
		if(tag.label == "window")
		{
			newWindow = new Wnd(windows.top().wnd, tag.label, className, id, tag.attributes, label);
		}
		else if(tag.label == "list")
		{
			List* l = new List(windows.top().wnd, tag.label, className, id, tag.attributes);
			newWindow = l;
			List::Node* n = new List::Node(":o");
			l->push_back(n);
			n = new List::Node(">:O");
			n->selected = true;
			l->push_back(n);
		}
		else if(tag.label == "button")
		{
			newWindow = new Button(windows.top().wnd, tag.label, className, id, tag.attributes, label);
		}
		
		if(!windows.top().wnd)
		{
			// Set as root
			context.setRoot(newWindow);
		}
		
		if(newWindow)
		{
			newWindow->applyGSS(style);
			newWindow->updatePlacement();

			windows.push(WndInfo(newWindow)); // Done last
		}
	}
	
	void endTag(std::string const& label)
	{
		cerr << "End tag: " << label << endl;
		windows.pop();
	}
	
	Tag tag;
	
	std::stack<WndInfo> windows;
	Context& context;
	Context::GSSselectorMap& style;
};

void Context::buildFromXML(std::istream& s, Wnd* dest)
{
	if(dest && dest->m_context != this)
	{
		return; // The destination window belongs to a different context
	}

	XMLHandler handler(*this, dest, m_gss);
	xmlDocument(s, handler);
}
	
void Context::testParseXML()
{
	istringstream rootGSS(
		"#root { background: #000080 ; left: 0 ; top: 0 ; bottom : -1 ; right: -1; padding: 29; spacing: 20 }");
		
	istringstream rootXML("<window id=\"root\" />");
	
	loadGSS(rootGSS);
	buildFromXML(rootXML, 0);
	
	// ========================== test XML and GSS ====================
	
	istringstream gss(
		"button { background: #00AF00 ; "
		" border: #AFFFAF; border-right: #00A000; border-bottom: #00A000 }"
		"button:focused { background: #AF0000 ; border-right: #A00000; border-bottom: #A00000; }"
		"#options { width: 100 ; height: 150 ; bottom: -10 ; right: -10 }");
		
	
	istringstream xml(
		"<button id=\"f\" label=\"Fullscreen\" command=\"vid_fullscreen 1\" />"
		"<button id=\"w\" label=\"Windowed\" command=\"vid_fullscreen 0\" />"
		"<button id=\"o\" label=\"Options\" command=\"gui_loadgss OPTIONS.GSS passive ; gui_loadxml options.xml ; gui_focus options\" />");
	
	// ================================================================
	
	loadGSS(gss);
	
	buildFromXML(xml, getRoot());
	
	setFocus(findNamedWindow("f"));
}

}
