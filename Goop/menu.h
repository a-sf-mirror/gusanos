#ifndef GUSANOS_MENU_H
#define GUSANOS_MENU_H

#include "omfggui.h"

class Font;

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
};

extern GContext menu;

}

#endif //GUSANOS_MENU_H
