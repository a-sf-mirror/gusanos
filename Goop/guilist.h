//ListItem Classes
#ifndef __guilist_h__
#define __guilist_h__

#include "gui.h"
#include <allegro.h>
#include "gfx.h"

namespace gui
{

class ListButtonItem : public ListItem
{
    std::string text;
    void (*callback)(const std::string&);

    public:
        ListButtonItem() : text("") { }
        ~ListButtonItem() { }
        void keys(bool[256]);
        void render(int x, int y, int w, int h);
        void setText(const std::string &str) { text = str; }
        void setEvent(void (*c)(const std::string&)) { callback = c; }
};

class ListTextItem : public ListItem
{
    std::string prefix;
    std::string text;
    public:
        ListTextItem() : prefix(""), text("") { }
        ~ListTextItem() { }
        void keys(bool[256]);
        void render(int x, int y, int w, int h);
        void setPrefix(const std::string &str) { prefix = str; }
        std::string& getText() { return text; }
};

class ListDragItem : public ListItem
{
    float dragValue;
    float dragMin;
    float dragMax;
    float dragInc;

    public:
        ListDragItem() : dragValue(127.0), dragMin(0.0), dragMax(255.0), dragInc(1.0) { }
        ~ListDragItem() { }
        void keys(bool[256]);
        void render(int x, int y, int w, int h);
        void setMin(float f) { dragMin = f; }
        void setMax(float f) { dragMax = f; }
        void setInc(float f) { dragInc = f; }
        void setValue(float f) { dragValue = f; }
        int getValue() { return dragValue; }
};

class ListColorItem : public ListItem
{
    ListDragItem *red;
    ListDragItem *green;
    ListDragItem *blue;
    public:
        void keys(bool[256]) { }
        void setRed(ListDragItem *r) { red = r; }
        void setGreen(ListDragItem *g) { green = g; }
        void setBlue(ListDragItem *b) { blue = b; }
        void render(int x, int y, int w, int h)
        {
            int r = (int)red->getValue();
            int g = (int)green->getValue();
            int b = (int)blue->getValue();
            rect(gfx.buffer, x, y, w, h, makecol(r, g, b));
        }
};

} // end namespace gui

#endif

