#include "gui.h"
#include <allegro.h>
#include "gfx.h"

namespace gui
{

BITMAP *windowDecor;
Font *font;

void init()
{
    font = fontLocator.load("minifont");
}

void Container::addWidget(Widget* w)
{
    if (m_widgets.size() == 0)
        m_focused = w;
    m_widgets.push_back(w);
}

void Container::setFocus(Widget* w)
{
    for (size_t i = 0; i < m_widgets.size(); i++)
    {
        if (m_widgets[i] == w)
        {
            m_focused = w;
            return;
        }
    }
}

void Container::keys(bool k[256])
{
    Widget::keys(k);
    /* Widget switching
    if (k[KEY_TAB])
    {
        for (int i = 0; i < m_widgets.size(); i++)
        {
            if (m_widgets[i] == m_focused)
            {
                if (i + 1 >= m_widgets.size())
                    m_focused = m_widgets[0];
                else
                    m_focused = m_widgets[i + 1];
                break;
            }
        }
        k[KEY_TAB] = 0;
    } else
    */
    if (m_focused)
        m_focused->keys(k);
}

void Container::render()
{
    if (!isVisible())
        return;
    for (size_t i = 0; i < m_widgets.size(); i++)
        m_widgets[i]->render();
}

bool List::addItem(std::string item)
{
    list.push_back(item);
	return true;
}

void List::clear()
{
    list.clear();
}

void List::keys(bool k[256])
{
    for (size_t i = 0; i < m_events.size(); i++)
    {
        if (k[m_events[i]->key])
        {
            m_events[i]->callback(list[selected]);
            k[m_events[i]->key] = false;
        }
    }

    if (k[KEY_UP])
    {
        selected--;
        k[KEY_UP] = false;
    }
    else if (k[KEY_DOWN])
    {
        selected++;
        k[KEY_DOWN] = false;
    }
    if (selected < 0)
        selected = 0;
    if (selected >= (int)list.size())
        selected = (int)list.size() - 1;
}

void List::render()
{
    if (!isVisible())
        return;
    //fill background
    rectfill(gfx.buffer, getX(), getY(), getX() + getW(), getY() + getH(), colBackground);

    //selection
    rectfill(gfx.buffer, getX(), getY() + selected * 8, getX() + getW(), getY() + selected * 8 + 8, colSelected);

    //Render items
    for (size_t i = 0; i < list.size(); i++)
    {
        font->draw(gfx.buffer, list[i].c_str(), getX() + 4, getY() + i * 8 + 2, 0);
    }
}

bool ListContainer::addItem(ListItem *item)
{
    list.push_back(item);
    return true;
}

void ListContainer::clear()
{
    list.clear();
}

void ListContainer::keys(bool k[256])
{
    /*
    for (int i = 0; i < m_events.size(); i++)
    {
        if (k[m_events[i]->key])
        {
            m_events[i]->callback(list[selected]);
            k[m_events[i]->key] = false;
        }
    }
    */

    if (k[KEY_UP])
    {
        selected--;
        k[KEY_UP] = false;
    }
    else if (k[KEY_DOWN])
    {
        selected++;
        k[KEY_DOWN] = false;
    } else
    {
        list[selected]->keys(k);
    }

    if (selected < 0)
        selected = 0;
    if (selected >= (int)list.size())
        selected = (int)list.size() - 1;
}

void ListContainer::render()
{
    if (!isVisible())
        return;

    //fill background
    rectfill(gfx.buffer, getX(), getY(), getX() + getW(), getY() + getH(), colBackground);

    //selection
    rectfill(gfx.buffer, getX(), getY() + 4 + selected * 8, getX() + getW(), getY() + 4 + selected * 8 + 8, colSelected);

    //Render items
    for (size_t i = 0; i < list.size(); i++)
    {
        list[i]->render(getX() + 4, getY() + 4 + i * 8, getW() - 8, 8);
    }
}

void Window::render()
{
    if (!isVisible())
        return;
    //draw title/border
    int x = getX();
    int y = getY();
    int w = getW();
    int h = getH();
    int i;

    rectfill(gfx.buffer, x, y, w, h, 0);

    Container::render();

    //Window left
    for (i = 0; i < h; i += 8)
    {
        masked_blit(windowDecor, gfx.buffer, 6 * 8, 0, x - 8, y + i, 8, 8); 
    }

    //Window right
    for (i = 0; i < h; i += 8)
    {
        masked_blit(windowDecor, gfx.buffer, 7 * 8, 0, x + w, y + i, 8, 8);
    }

    //Window top
    masked_blit(windowDecor, gfx.buffer, 0 * 8, 0, x - 8, y - 8, 8, 8);
    for (i = 0; i < w; i += 8)
    {
        masked_blit(windowDecor, gfx.buffer, 1 * 8, 0, x + i, y - 8, 8, 8);
    }
    masked_blit(windowDecor, gfx.buffer, 2 * 8, 0, x + w, y - 8, 8, 8);

    //Window bottom
    masked_blit(windowDecor, gfx.buffer, 3 * 8, 0, x - 8, y + h, 8, 8);
    for (i = 0; i < w; i += 8)
    {
        masked_blit(windowDecor, gfx.buffer, 4 * 8, 0, x + i, y + h, 8, 8);
    }
    masked_blit(windowDecor, gfx.buffer, 5 * 8, 0, x + w, y + h, 8, 8);

    font->draw(gfx.buffer, m_title.c_str(), x + 2, y - 6, 0);
}

} // end namespace gui

