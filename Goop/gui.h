//GUI Widgets

#ifndef __GUI_H__
#define __GUI_H__

#include <iostream>
#include <vector>
#include <allegro.h>
#include "font.h"

namespace gui
{

extern BITMAP *windowDecor;
extern Font *font;

void init();

//Event Structure
struct Event
{
    Event(unsigned short k, void (*c)(const std::string&)) : key(k), callback(c) { }
    unsigned short key;     //Key which triggers event
    void (*callback)(const std::string&); //Event callback
};

//Base Window Gadget
class Widget
{
    int x, y;           //widget X, y Coords
    int w, h;           //widget width and height
    int z;              //Z-order
    bool m_visible;     //Visible?
    bool m_active;      //Active?

    public:
        std::vector<Event*> m_events;

        Widget() : m_visible(true), m_active(true) { }
        virtual ~Widget()
        {
            for (int i = 0; i < m_events.size(); i++)
            {
                delete m_events[i];
            }
        }
        //cirtual ~Widget() = 0;

        virtual void setPosition(int xx, int yy) { x = xx; y = yy; }
        virtual void setSize(int ww, int hh) { w = ww; h = hh; }
        virtual bool isVisible() { return m_visible; }
        virtual bool setVisible(bool v) { m_visible = v; }
        virtual int getX() { return x; }
        virtual int getY() { return y; }
        virtual int getW() { return w; }
        virtual int getH() { return h; }

        virtual void addEvent(int k, void (*ev)(const std::string&)) { m_events.push_back(new Event(k, ev)); }
        virtual void keys(bool k[256])
        {
            for (int i = 0; i < m_events.size(); i++)
            {
                if (k[m_events[i]->key])
                {
                    m_events[i]->callback("");
                    k[m_events[i]->key] = false;
                }
            }
        }
        virtual void render() = 0;
};

//Plain Container
class Container : public Widget
{
    std::vector<Widget*> m_widgets;
    Widget* m_focused;

    public:
        Container() : m_focused(NULL) { }
        ~Container() { }
        void addWidget(Widget*);
        int size() { return m_widgets.size(); }
        void setFocus(Widget*);

        void keys(bool[256]);
        void render();
};

//Plain List
class List : public Widget
{
    std::vector<std::string> list;
    int selected;

    unsigned int colBackground;
    unsigned int colSelected;

    public:
        List() : selected(0) { }
        ~List() { }
        bool addItem(std::string item);
        void clear();
        void keys(bool[256]);

        void setBackColor(unsigned int c) { colBackground = c; }
        void setSelColor(unsigned int c) { colSelected = c; }

        void render();
};

//List Item for List Container
class ListItem
{
    public:
        ListItem() { }
        ~ListItem() { }
        virtual void keys(bool[256]) = 0;
        virtual void render(int x, int y, int w, int h) = 0;
};

//List Container (holds ListItem)
class ListContainer : public Widget
{
    std::vector<ListItem*> list;
    int selected;

    unsigned int colBackground;
    unsigned int colSelected;

    public:
        ListContainer() : selected(0) { }
        ~ListContainer() { }
        bool addItem(ListItem *item);
        void clear();
        void keys(bool[256]);

        void setBackColor(unsigned int c) { colBackground = c; }
        void setSelColor(unsigned int c) { colSelected = c; }

        void render();
};

//Themed container
class Window : public Container
{
    std::string m_title;
    public:
        void setTitle(const std::string& t) { m_title = t; }
        void render();
};

} //end namespace gui

#endif

