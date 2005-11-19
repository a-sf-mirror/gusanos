#include "renderer.h"
#include <iostream>
using std::cerr;
using std::endl;

namespace OmfgGUI
{

void Renderer::drawSkinnedBox(BaseSpriteSet const& skin, Rect const& rect, RGB const& backgroundColor)
{
	int skinWidth = skin.getFrameWidth(0), skinHeight = skin.getFrameHeight(0);
	
	int x, y;
	/*
	int boxWidth = rect.getWidth();
	int boxHeight = rect.getHeight();
	*/
	drawBox(Rect(rect.x1 + skinWidth, rect.y1 + skinHeight, rect.x2 - skinWidth - 1, rect.y2 - skinHeight - 1), backgroundColor);
	for(y = rect.y1 + skinHeight; y < rect.y2 - skinHeight * 2; y += skinHeight)
	{
		drawSprite(skin, 4, rect.x1, y);
		drawSprite(skin, 5, rect.x2 - skinWidth, y);
	}
	
	int cutOff = skinHeight*2 - rect.y2 + y;
	drawSprite(skin, 4, rect.x1, y, 0, 0, cutOff, 0);
	drawSprite(skin, 5, rect.x2 - skinWidth, y, 0, 0, cutOff, 0);
	
	for(x = rect.x1 + skinWidth; x < rect.x2 - skinWidth * 2; x += skinWidth)
	{
		drawSprite(skin, 6, x, rect.y1);
		drawSprite(skin, 7, x, rect.y2 - skinHeight);
	}
	
	cutOff = skinWidth*2 - rect.x2 + x;
	drawSprite(skin, 6, x, rect.y1, 0, 0, 0, cutOff);
	drawSprite(skin, 7, x, rect.y2 - skinHeight, 0, 0, 0, cutOff);

	drawSprite(skin, 0, rect.x1, rect.y1);
	drawSprite(skin, 1, rect.x2 - skinWidth, rect.y1);
	drawSprite(skin, 2, rect.x1, rect.y2 - skinHeight);
	drawSprite(skin, 3, rect.x2 - skinWidth, rect.y2 - skinHeight);
}

}
