#include "wnd.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

using std::cout;
using std::cerr;
using std::endl;

namespace OmfgGUI
{

Wnd::~Wnd()
{
	if(m_context)
	{
		//Remove references in context
		
		m_context->deregisterWindow(this);
	}
	
	if(m_parent)
	{
		m_parent->m_children.remove(this);
	}

	//cerr << "I'm (" << this << ", '" << m_id << "') being deleted :OOO , and my children too :<<<" << endl;

	std::list<Wnd *>::iterator i = m_children.begin(), e = m_children.end();
/*
	if(i == e)
		cerr << "Oh, I have no children :o" << endl;
*/
	for(; i != e;)
	{
		//cerr << "I'm going to lose " << *i << " :OOO" << endl;
		std::list<Wnd *>::iterator next = i; ++next;
		delete (*i);
		//cerr << "I lost " << *i << " :(" << endl;
		i = next;
	}
	
	/*if(m_font)
		cerr << "I have a font (" << m_font << ") :O, they're taking it away from me :(" << endl;
	*/
	delete m_font;
	
	//cerr << "I'm (" << this << ", '" << m_id << "') gone :((" << endl;
}

void Wnd::getCoord(int& dx, int& dy, int x, int y)
{
	if(!m_parent)
	{
		dx = x;
		dy = y;
		return;
	}
	
	if(x >= 0)
		dx = m_parent->getRect().x1 + x;
	else
		dx = m_parent->getRect().x2 + x;
		
	if(y >= 0)
		dy = m_parent->getRect().y1 + y;
	else
		dy = m_parent->getRect().y2 + y;
		
	return;
}

void Wnd::getCoordX(int& dx, int x)
{
	if(!m_parent)
	{
		if(x >= 0)
			dx = x;
		else
			dx = 320 + x; //TODO: Get the real viewport height
		return;
	}
	
	if(x >= 0)
		dx = m_parent->getRect().x1 + x;
	else
		dx = m_parent->getRect().x2 + x;

	return;
}

void Wnd::getCoordY(int& dy, int y)
{
	if(!m_parent)
	{
		if(y >= 0)
			dy = y;
		else
			dy = 240 + y; //TODO: Get the real viewport width
		return;
	}
	
	if(y >= 0)
		dy = m_parent->getRect().y1 + y;
	else
		dy = m_parent->getRect().y2 + y;

	return;
}

static int fromHex(char h)
{
	if(h >= '0' && h <= '9')
		return h - '0';
	else if(h >= 'a' && h <= 'f')
		return h - 'a' + 10;
	else if(h >= 'A' && h <= 'F')
		return h - 'A' + 10;
	else
		return -1;
}

static bool readColor(RGB& dest, std::string const& str)
{
	if(str.size() >= 7 && str[0] == '#')
	{
		dest.r = fromHex(str[1]) * 16 + fromHex(str[2]);
		dest.g = fromHex(str[3]) * 16 + fromHex(str[4]);
		dest.b = fromHex(str[5]) * 16 + fromHex(str[6]);
		return true;
	}
	return false;
}

bool Wnd::readSpriteSet(BaseSpriteSet*& dest, std::string const& str)
{
	if(str.size() == 0)
	{
		dest = 0;
		return true;
	}
	
	delete dest;
	dest = m_context->loadSpriteSet(str);
	//cerr << "Loaded: " << str << "(" << dest << ")" << endl;
	if(!dest)
		return false;

	return true;
}

void Wnd::applyGSSnoState(Context::GSSselectorMap const& style)
{
	applyFormatting(style(m_tagLabel)(           )(    )());
	if(!m_className.empty())
		applyFormatting(style(          )(m_className)(    )());
	if(!m_id.empty())
		applyFormatting(style(          )(           )(m_id)());
	if(!m_className.empty())
		applyFormatting(style(m_tagLabel)(m_className)(    )());
	if(!m_id.empty())
		applyFormatting(style(m_tagLabel)(           )(m_id)());
}

void Wnd::applyGSSstate(Context::GSSselectorMap const& style, std::string const& state)
{
	applyGSSnoState(style);
	
	applyFormatting(style(m_tagLabel)(           )(    )(state));
	if(!m_className.empty())
		applyFormatting(style(          )(m_className)(    )(state));
	if(!m_id.empty())
		applyFormatting(style(          )(           )(m_id)(state));
	if(!m_className.empty())
		applyFormatting(style(m_tagLabel)(m_className)(    )(state));
	if(!m_id.empty())
		applyFormatting(style(m_tagLabel)(           )(m_id)(state));
	
}

void Wnd::applyGSS(Context::GSSselectorMap const& style)
{
	//cout << "Context: " << m_context << endl;
	if(m_context && m_context->getFocus() == this)
		applyGSSstate(style, "focused");
	else
		applyGSSnoState(style);
		
	//updateFormatting();
}

void Wnd::applyFormatting(Context::GSSpropertyMap const& f)
{
	#define EACH_VALUE(i_) for(std::vector<std::string>::const_iterator i_ = i->second.begin(); i_ != i->second.end(); ++i_)
	
	for(Context::GSSpropertyMap::const_iterator i = f.begin(); i != f.end(); ++i)
	{
		//cout << "Applying to " << m_id << ": " << i->first << endl;
		if(i->first == "background")
		{
			EACH_VALUE(v)
			{
				readColor(m_formatting.background.color, *v);
			}
		}
		if(i->first == "background-image")
		{
			EACH_VALUE(v)
			{
				if(readSpriteSet(m_formatting.background.spriteSet, *v))
					break;
			}
		}
		else if(i->first == "border")
		{
			RGB color;
			EACH_VALUE(v)
			{
				readColor(color, *v);
			}
			
			m_formatting.borders[0].color = color;
			m_formatting.borders[1].color = color;
			m_formatting.borders[2].color = color;
			m_formatting.borders[3].color = color;
		}
		else if(i->first == "border-left")
		{
			EACH_VALUE(v)
			{
				readColor(m_formatting.borders[0].color, *v);
			}
		}
		else if(i->first == "border-top")
		{
			EACH_VALUE(v)
			{
				readColor(m_formatting.borders[1].color, *v);
			}
		}
		else if(i->first == "border-right")
		{
			EACH_VALUE(v)
			{
				readColor(m_formatting.borders[2].color, *v);
			}
		}
		else if(i->first == "border-bottom")
		{
			EACH_VALUE(v)
			{
				readColor(m_formatting.borders[3].color, *v);
			}
		}
		else if(i->first == "left")
		{
			EACH_VALUE(v)
			{
				m_formatting.rect.x1 = lexical_cast<int>(*v);
			}
			m_formatting.flags |= Formatting::HasLeft;
		}
		else if(i->first == "right")
		{
			EACH_VALUE(v)
			{
				m_formatting.rect.x2 = lexical_cast<int>(*v);
			}
			m_formatting.flags |= Formatting::HasRight;
		}
		else if(i->first == "top")
		{
			EACH_VALUE(v)
			{
				m_formatting.rect.y1 = lexical_cast<int>(*v);
			}
			m_formatting.flags |= Formatting::HasTop;
		}
		else if(i->first == "bottom")
		{
			EACH_VALUE(v)
			{
				m_formatting.rect.y2 = lexical_cast<int>(*v);
			}
			m_formatting.flags |= Formatting::HasBottom;
		}
		else if(i->first == "width")
		{
			EACH_VALUE(v)
			{
				m_formatting.width = lexical_cast<int>(*v);
			}
		}
		else if(i->first == "height")
		{
			EACH_VALUE(v)
			{
				m_formatting.height = lexical_cast<int>(*v);
			}
		}
		else if(i->first == "spacing")
		{
			EACH_VALUE(v)
			{
				m_formatting.spacing = lexical_cast<int>(*v);
			}
		}
		else if(i->first == "padding")
		{
			EACH_VALUE(v)
			{
				m_formatting.padding = lexical_cast<int>(*v);
			}
		}
		else if(i->first == "font-family")
		{
			delete m_font;
			
			EACH_VALUE(v)
			{
				m_font = m_context->loadFont(*v);
				if(m_font)
					break;
			}
		}
		
	}
	
	#undef EACH_VALUE
}

void Wnd::updatePlacement()
{
	switch(m_formatting.flags & (Formatting::HasLeft | Formatting::HasRight))
	{
		case 0:
		{
			//m_rect.x1 = formatting.x;
			if(m_parent)
			{
				m_parent->allocateSpace(m_rect.x1, m_rect.y1, m_formatting.width, m_formatting.height);
				m_rect.x2 = m_rect.x1 + m_formatting.width;
				m_rect.y2 = m_rect.y1 + m_formatting.height;
			}
			else
			{
				m_rect = Rect(0, 0, 320-1, 240-1); //TODO: Replace with real dimensions
			}
		}
		break;
		
		case Formatting::HasLeft:
		{
			getCoordX(m_rect.x1, m_formatting.rect.x1);
			m_rect.x2 = m_rect.x1 + m_formatting.width;
		}
		break;
		
		case Formatting::HasRight:
		{
			getCoordX(m_rect.x2, m_formatting.rect.x2);
			m_rect.x1 = m_rect.x2 - m_formatting.width;
		}
		break;
		
		default:
		{
			getCoordX(m_rect.x1, m_formatting.rect.x1);
			getCoordX(m_rect.x2, m_formatting.rect.x2);
		}
	}
	
	switch(m_formatting.flags & (Formatting::HasTop | Formatting::HasBottom))
	{
		case 0:
		{
			//m_rect.y1 = windows.top().curY;
		}
		break;
		
		case Formatting::HasTop:
		{
			getCoordY(m_rect.y1, m_formatting.rect.y1);
			m_rect.y2 = m_rect.y1 + m_formatting.height;
		}
		break;
		
		case Formatting::HasBottom:
		{
			getCoordY(m_rect.y2, m_formatting.rect.y2);
			m_rect.y1 = m_rect.y2 - m_formatting.height;
		}
		break;
		
		default:
		{
			getCoordY(m_rect.y1, m_formatting.rect.y1);
			getCoordY(m_rect.y2, m_formatting.rect.y2);
		}
	}
	
	m_freeNextX = m_freeRect.x1 = m_rect.x1 + m_formatting.padding;
	m_freeNextY = m_freeRect.y1 = m_rect.y1 + m_formatting.padding;
	m_freeRect.x2 = m_rect.x2 - m_formatting.padding;
	m_freeRect.y2 = m_rect.y2 - m_formatting.padding;

}

void Wnd::allocateSpace(int& x, int& y, int width, int height)
{
	if(m_freeNextX + width > m_freeRect.x2)
	{
		m_freeRect.y1 = m_freeNextY;
		m_freeNextX = m_freeRect.x1;
	}
	
	x = m_freeNextX;
	y = m_freeRect.y1;
	
	m_freeNextX += width + m_formatting.spacing;
	
	int bottom = m_freeRect.y1 + height + m_formatting.spacing;
	if(bottom > m_freeNextY)
		m_freeNextY = bottom;
}

Wnd* Wnd::findClosestChild(Wnd* org, Dir direction)
{
	std::list<Wnd *>::iterator i = m_children.begin(), e = m_children.end();
	
	long  minDist = 0x7FFFFFFF;
	Wnd  *minWnd = 0;
	
	int orgx = org->getRect().centerX();
	int orgy = org->getRect().centerY();
	
	for(; i != e; ++i)
	{
		if(*i != org)
		{
			Dir thisDir;
			int x = (*i)->getRect().centerX() - orgx;
			int y = (*i)->getRect().centerY() - orgy;
			if(x > 0)
			{
				if(y < -x) thisDir = Up;
				else if(y >= x) thisDir = Down;
				else thisDir = Right;
			}
			else
			{
				if(y <= x) thisDir = Up;
				else if(y > -x) thisDir = Down;
				else thisDir = Left;
			}
			
			if(thisDir == direction)
			{
				int dist = x*x + y*y;
				if(dist < minDist)
				{
					minWnd = *i;
					minDist = dist;
				}
			}
		}
	}
	
	return minWnd;
}

bool Wnd::doRender(Renderer* renderer, Rect const& clip)
{
	//Render parent first
	if(!m_visible)
		return false;
		
	Rect rect(clip);
	rect.intersect(m_rect);
	if(!rect.isValid())
		return false;
	renderer->setClip(rect);
		
	if(!render(renderer))
		return false;

	std::list<Wnd *>::iterator i = m_children.begin(), e = m_children.end();
	
	for(; i != e; ++i)
	{
		(*i)->doRender(renderer, rect);
	}
	
	return true;
}

void Wnd::doProcess()
{
	//Process children first
	std::list<Wnd *>::iterator i = m_children.begin(), e = m_children.end();
	
	for(; i != e; ++i)
	{
		(*i)->doProcess();
	}
	
	process();
}

bool Wnd::render(Renderer* renderer)
{
	return true;
}

void Wnd::process()
{
}

void Wnd::setText(std::string const& aStr)
{
	m_text = aStr;
}

std::string const& Wnd::getText() const
{
	return m_text;
}

bool Wnd::getAttrib(std::string const& name, std::string& dest)
{
	std::map<std::string, std::string>::iterator i = m_attributes.find(name);
	if(i == m_attributes.end())
		return false;
	
	dest = i->second;
	return true;
}

void Wnd::doUpdateGSS()
{
	m_formatting = Formatting(); // Reset formatting to default
	applyGSS(m_context->m_gss); // Apply GSS
	updatePlacement(); // Place window
	
	std::list<Wnd *>::iterator i = m_children.begin(), e = m_children.end();
	
	// Update GSS for children
	for(; i != e; ++i)
		(*i)->doUpdateGSS();
}

//Sends a cursor relocation event
bool Wnd::doMouseMove(ulong newX, ulong newY)
{
	if(!m_rect.isInside(newX, newY))
		return false;
		
	std::list<Wnd *>::iterator i = m_children.begin(), e = m_children.end();
	
	for(; i != e; ++i)
		if((*i)->doMouseMove(newX, newY))
			return true;

	return mouseMove(newX, newY);
}

//Sends a mouse button down event
bool Wnd::doMouseDown(ulong newX, ulong newY, Context::MouseKey::type button)
{
	if(!m_rect.isInside(newX, newY))
		return false;
		
	std::list<Wnd *>::iterator i = m_children.begin(), e = m_children.end();
	
	for(; i != e; ++i)
		if((*i)->doMouseDown(newX, newY, button))
			return true;
		
	return mouseDown(newX, newY, button);
}

//Sends a mouse button up event
bool Wnd::doMouseUp(ulong newX, ulong newY, Context::MouseKey::type button)
{
	if(!m_rect.isInside(newX, newY))
		return false;
		
	std::list<Wnd *>::iterator i = m_children.begin(), e = m_children.end();
	
	for(; i != e; ++i)
		if((*i)->doMouseUp(newX, newY, button))
			return true;
		
	return mouseUp(newX, newY, button);
}

//Sends a cursor relocation event
bool Wnd::mouseMove(ulong newX, ulong newY)
{
	return false;
}

//Sends a mouse button down event
bool Wnd::mouseDown(ulong newX, ulong newY, Context::MouseKey::type button)
{
	return false;
}

//Sends a mouse button up event
bool Wnd::mouseUp(ulong newX, ulong newY, Context::MouseKey::type button)
{
	return false;
}

void Wnd::setContext_(Context* context)
{
	m_context = context;
	m_context->registerWindow(this);
	
	std::list<Wnd *>::iterator i = m_children.begin(), e = m_children.end();
	
	for(; i != e; ++i)
		(*i)->setContext_(context);
}

int Wnd::classID()
{
	return Context::Unknown;
}

}
