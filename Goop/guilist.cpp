#include "guilist.h"
#include <allegro.h>
#include "gfx.h"

namespace gui
{

void ListButtonItem::keys(bool k[256])
{
    if (k[KEY_ENTER])
    {
        k[KEY_ENTER] = false;
        callback(text);
    }
}

void ListButtonItem::render(int x, int y, int w, int h)
{
    font->draw(gfx.buffer, text.c_str(), x + 2, y + 2, 0);
}

void ListTextItem::keys(bool k[256])
{
    if (k[KEY_BACKSPACE])
    {
        if (text.size() > 0)
            text.erase(text.size() - 1);
        k[KEY_BACKSPACE] = false;
    }
    else if (k[KEY_SPACE])
    {
        text += " ";
        k[KEY_SPACE] = false;
    }
    else
    {
        for (int i = 'a'; i <= 'z'; i++)
        {
            if (k[i])
            {
                if (k[KEY_RSHIFT] | k[KEY_LSHIFT])
                    text += i + ('A' - 'a');
                else
                    text += i;
                k[i] = false;
            }
        }
    }
}

void ListTextItem::render(int x, int y, int w, int h)
{
    //printString(screen, font, (prefix + text).c_str(), x + 2, y + 2);
}

void ListDragItem::keys(bool k[256])
{
    if (k[KEY_LEFT])
    {
//        k[KEY_LEFT] = false;
        dragValue -= dragInc;
    }
    else if (k[KEY_RIGHT])
    {
//        k[KEY_RIGHT] = false;
        dragValue += dragInc;
    }

    if (dragValue < dragMin)
        dragValue = dragMin;
    else if (dragValue > dragMax)
        dragValue = dragMax;
}

void ListDragItem::render(int x, int y, int w, int h)
{
    int dragX = dragValue / ((dragMax - dragMin) / w);

    //draw base bar
    rectfill(gfx.buffer, x, y + h / 2, x + w, y + h / 2, makecol(255, 255, 255));

    //draw drag thingy
    rectfill(gfx.buffer, x + dragX, y + 2, x + dragX + 1, y + 2 + 4, makecol(255, 0, 0));
}

} //end namespace gui

