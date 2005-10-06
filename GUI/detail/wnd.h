#ifndef OMFG_GUI_WND_H
#define OMFG_GUI_WND_H

#include <string>
#include "omfgutil_math.h"
#include "omfgutil_common.h"
#include "renderer.h"
#include "context.h"

#include <iostream>
#include <list>
#include <map>
using std::cerr;
using std::endl;

namespace OmfgGUI
{

class Wnd
{
public:
	friend class Context;
	
	enum Dir
	{
		Up = 0,
		Right,
		Down,
		Left
	};
	
	Wnd(Wnd* parent, std::string const& tagLabel, std::string const& className,
	  std::string const& id, std::map<std::string, std::string> const& attributes,
	  std::string const& text = std::string(""))
	: m_focusable(true), m_text(text), m_parent(parent), m_lastChildFocus(0)
	, m_font(0), m_tagLabel(tagLabel), m_className(className), m_id(id)
	, m_attributes(attributes), m_visible(true), m_active(false)
	{
		if(m_parent)
		{
			setContext_(m_parent->m_context);
			m_parent->m_children.push_back(this);
		}
		else
			m_context = 0;

	}
	
	virtual ~Wnd();

/*
	signal<void (Wnd&, std::string const&,
		std::string const&)> sigTextChange;*/
		
	bool doRender(Renderer* renderer, Rect const& clip);
	
	void doProcess();
	
	/*
		returns: True if anything was rendered, False otherwise
	*/
	virtual bool render(Renderer* renderer);
	
	virtual void process();
	
	virtual void setActivation(bool active);
	
	void doSetActivation(bool active);
	
	void toggleActivation()
	{
		doSetActivation(!m_active);
	}
	
	virtual void setText(std::string const& aStr);

	std::string const& getText() const
	{
		return m_text;
	}
	
	Rect const& getRect()
	{ return m_rect; }
	
	Wnd* getParent()
	{ return m_parent; }
	
	std::string const& getID()
	{ return m_id; }
	
	bool getAttrib(std::string const& name, std::string& dest);
	
	void getCoord(int& dx, int& dy, int x, int y);
	void getCoordX(int& dx, int x);
	void getCoordY(int& dy, int y);
	
	void applyGSSnoState(Context::GSSselectorMap const& style);
	void applyGSSstate(Context::GSSselectorMap const& style, std::string const& state);
	virtual void applyGSS(Context::GSSselectorMap const&);
	virtual void applyFormatting(Context::GSSpropertyMap const&);
	void updatePlacement();
	
	void allocateSpace(int& x, int& y, int width, int height);
	
	Wnd* findClosestChild(Wnd* org, Dir direction);
	
	void doUpdateGSS();
		
	//Sends a cursor relocation event
	bool doMouseMove(ulong aNewX, ulong aNewY);
	
	//Sends a mouse button down event
	bool doMouseDown(ulong aNewX, ulong aNewY, Context::MouseKey::type aButton);
	
	//Sends a mouse button up event
	bool doMouseUp(ulong aNewX, ulong aNewY, Context::MouseKey::type aButton);
	
	//Sends a cursor relocation event
	virtual bool mouseMove(ulong aNewX, ulong aNewY);
	
	//Sends a mouse button down event
	virtual bool mouseDown(ulong aNewX, ulong aNewY, Context::MouseKey::type aButton);
	
	//Sends a mouse button up event
	virtual bool mouseUp(ulong aNewX, ulong aNewY, Context::MouseKey::type aButton);
	
	virtual bool keyDown(int key);
	
	virtual bool keyUp(int key);
	
	virtual bool charPressed(char c, int key);
	
	virtual int classID();
	
	void setVisibility(bool v)
	{
		m_visible = v;
		if(!v)
			notifyHide();
		else
			notifyShow();
	}
	
	void notifyHide();
	
	void notifyShow();
	
	bool isVisibile();
	
	//std::string const& getText() const;
	
	bool                 m_focusable;
	
protected:

	void setContext_(Context* context);
	
	bool readSpriteSet(BaseSpriteSet*& dest, std::string const& str);
	
	bool readSkin(BaseSpriteSet*& dest, std::string const& str);

/*
	//Transfers ownership
	Wnd* add_(Wnd* wnd)
	{
		if(m_context)
		{
			wnd->setContext_(m_context); //Inherit root
		}

		m_children.push_back(wnd);
		return wnd;
	}*/
	
	std::string          m_text;
	Rect                 m_rect;
	std::list<Wnd *>     m_children;
	Wnd                 *m_parent;
	Wnd                 *m_lastChildFocus;
	Context             *m_context;
	BaseFont            *m_font;
	
	std::string          m_tagLabel;
	std::string          m_className;
	std::string          m_id;
	
	std::map<std::string, std::string> m_attributes;
	
	bool                 m_visible;
	bool                 m_active;
	
	Rect                 m_freeRect;
	int                  m_freeNextX;
	int                  m_freeNextY;
	
	// Formatting
	struct Formatting
	{
		enum Flags
		{
			HasLeft   = 1 << 0,
			HasTop    = 1 << 1,
			HasRight  = 1 << 2,
			HasBottom = 1 << 3,
		};
		
		Formatting()
		: width(50), height(50), spacing(5), padding(5), flags(0)
		, alpha(255), rect(10, 10, 0, 0), fontColor(255, 255, 255)
		{
			
		}

		struct Border
		{
			Border()
			: color(255, 255, 255)
			{
			}
			
			RGB color;
		} borders[4];
		
		struct Background
		{
			Background()
			: color(128, 128, 128), spriteSet(0), skin(0)
			{
			}
			
			~Background()
			{
				//cerr << "Deleting spriteSet " << spriteSet << endl;
				delete spriteSet;
			}
			
			RGB color;
			BaseSpriteSet *spriteSet;
			BaseSpriteSet *skin;
		} background;
		
		int         width;
		int         height;
		
		int         spacing;
		int         padding;
		long        flags;
		int         alpha;
	
		Rect        rect;
		RGB         fontColor;

	} m_formatting;
};

}

#endif //OMFG_GUI_WND_H
