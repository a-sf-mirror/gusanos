#ifndef GUSANOS_MENU_H
#define GUSANOS_MENU_H

#include "omfggui.h"
#include "resource_locator.h"
#include <boost/filesystem/fstream.hpp>

class Font;
class SpriteSet;

namespace OmfgGUI
{
	
struct GusanosFont : public BaseFont
{
	GusanosFont(Font* font_)
	: font(font_)
	{
	}
	
	Font *font;
};

struct GusanosSpriteSet : public BaseSpriteSet
{
	GusanosSpriteSet(SpriteSet* spriteSet_)
	: spriteSet(spriteSet_)
	{
	}
	
	virtual int getFrameCount();
	
	SpriteSet *spriteSet;
};

struct AllegroRenderer : public Renderer
{

	// Draws a box
	virtual void drawBox(
		Rect const& rect,
		RGB const& color,
		RGB const& borderLeftColor,
		RGB const& borderTopColor,
		RGB const& borderRightColor,
		RGB const& borderBottomColor);
	
	// Draws text
	virtual void drawText(BaseFont const& font, std::string const& str, ulong flags, ulong x, ulong y, RGB const& aColor);
	
	virtual void drawSprite(BaseSpriteSet const& spriteSet, int frame, ulong x, ulong y);

	virtual void setClip(Rect const& rect);
	virtual Rect const& getClip();
	virtual Rect const& getViewportRect();
	
	virtual void setBlending(int alpha);
	virtual void resetBlending();
	
private:
	Rect clipRect;
	Rect screenRect;
};

//extern GusanosFont defaultMenuFont;

class GContext : public Context
{
public:
	GContext()
	{
	}
	
	void init();
	
	bool eventKeyDown(int k);
	bool eventKeyUp(int k);
	
	void clear();
	
	virtual BaseFont* loadFont(std::string const& name);
	virtual BaseSpriteSet* loadSpriteSet(std::string const& name);
};

extern GContext menu;

}

class XMLFile
{
public:
	operator bool()
	{
		return f;
	}
	
	fs::ifstream f;
};

extern ResourceLocator<XMLFile, false, false> xmlLocator;

class GSSFile
{
public:
	operator bool()
	{
		return f;
	}
	
	fs::ifstream f;
};

extern ResourceLocator<GSSFile, false, false> gssLocator;

#endif //GUSANOS_MENU_H
