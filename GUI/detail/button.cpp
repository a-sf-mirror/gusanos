#include "button.h"

using std::cout;
using std::endl;

namespace OmfgGUI
{

bool Button::render(Renderer* renderer)
{
	//Draw a flat, grey box with the text by default
	renderer->drawBox(
		getRect(), m_formatting.background.color,
		m_formatting.borders[0].color,
		m_formatting.borders[1].color,
		m_formatting.borders[2].color,
		m_formatting.borders[3].color);
	
	if(m_font)
		renderer->drawText(*m_font, m_text, BaseFont::CenterV | BaseFont::CenterH, m_rect.centerX(), m_rect.centerY(), RGB(0, 0, 0));
	
	return true;
}

void Button::process()
{
}

//Sends a mouse button down event
bool Button::mouseDown(ulong newX, ulong newY, Context::MouseKey::type button)
{
	return false;
}

//Sends a mouse button up event
bool Button::mouseUp(ulong newX, ulong newY, Context::MouseKey::type button)
{
	return false;
}

}
