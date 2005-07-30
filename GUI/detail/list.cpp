#include "list.h"

namespace OmfgGUI
{

void List::Node::render(Renderer* renderer, long& y, List& list)
{
	long halfRowHeight = rowHeight/2;
	
	if(selected)
	{
		renderer->drawBox(
			Rect(list.getRect().x1, y, list.getRect().x2, y + rowHeight),
			RGB(180, 180, 255),
			list.m_formatting.borders[0].color,
			list.m_formatting.borders[1].color,
			list.m_formatting.borders[2].color,
			list.m_formatting.borders[3].color);
	}

/*
	if(this == list.m_MainSel)
	{
		//TODO: aRenderer->drawFrame(Rect(list.m_Rect.x1 + 1, y, list.m_Rect.x2 - 2, y + rowHeight), RGB(0, 0, 0));
	}*/
	
	renderer->drawText(*list.m_font, text, BaseFont::CenterV, list.m_rect.x1 + 3 + level * 5, y + halfRowHeight, RGB(0, 0, 0));
	
	//renderChildren(aRenderer, y, list);
}
/*
void List::Node::renderChildren(Renderer* aRenderer, long& y, List& list)
{
	if(expanded)
	{
		for(node_iter_t i = children.begin(); y < list.m_Rect.y2 && i != children.end(); ++i)
		{
			y += rowHeight;
			i->render(i, aRenderer, y, r, list);
		}
	}
}*/

void List::Node::renderFrom(Renderer* renderer, long& y, List& list)
{
	node_iter_t i(this);
	//bool        hasParent = i->hasParent;
	//list_t*     parentList = i->parentList;
	//assert(parentList);
	
	//Only the root element has parentList == 0, and we can't even obtain an iterator
	//to the root element, thus it's safe to assume that parentList is a valid pointer.
	
	while(i && y < list.getRect().y2)
	{
		i->render(renderer, y, list);
			
		y += rowHeight;
		
		if(i->expanded)
		{
			i->children.getFirst()->renderFrom(renderer, y, list);
			
			if(y >= list.getRect().y2)
				break;
		}
		
		++i;
		
		/*
		while(i == 0)
		{
			if(!hasParent)
				return;
				
			i = parent;
			//parentList = i->parentList;
			//assert(parentList);
			hasParent = i->hasParent;
			parent = i->parent;
			++i;
		}*/
	}
}

List::node_iter_t List::Node::findByIdx(node_iter_t i, long aIdx)
{
	node_iter_t parent = i->parent;
	//bool        hasParent = i->hasParent;
	//list_t*     parentList = i->parentList;
	//assert(parentList);
	
	//Only the root element has parentList == 0, and we can't even obtain an iterator
	//to the root element, thus it's safe to assume that parentList is a valid pointer.
	
	while(--aIdx >= 0)
	{
		node_iter_t lastValid = i;
		
		if(i->expanded)
		{
			parent = i;
			//hasParent = true;
			//parentList = &i->children;
			i = i->children.begin();
			
		}
		
		++i;
		while(!i)
		{
			if(!parent)
				return lastValid;
				
			i = parent;
			//parentList = i->parentList;
			//assert(parentList);
			//hasParent = i->hasParent;
			parent = i->parent;
			++i;
		}
	}
	
	return i;
}

bool List::render(Renderer* renderer)
{
	renderer->drawBox(
		getRect(),
		m_formatting.background.color,
		m_formatting.borders[0].color,
		m_formatting.borders[1].color,
		m_formatting.borders[2].color,
		m_formatting.borders[3].color);
	
	long halfRowHeight = rowHeight/2;
	
	long y = getRect().y1;
	
	if(true) //TODO: Check view column headers parameter
	{
		renderer->drawBox(
			Rect(getRect().x1, getRect().y1, getRect().x2, getRect().y1 + rowHeight),
			RGB(170, 170, 255),
			m_formatting.borders[0].color,
			m_formatting.borders[1].color,
			m_formatting.borders[2].color,
			m_formatting.borders[3].color);
			
		// Render column headers
		double x = 3.0 + getRect().x1;
		for(std::vector<ColumnHeader>::const_iterator i = m_columnHeaders.begin();
			i != m_columnHeaders.end();
			++i)
		{
			renderer->drawText(*m_font, i->name, BaseFont::CenterV, long(x), y + halfRowHeight, RGB(0, 0, 0));
			x += i->widthFactor * getRect().getWidth();
		}
		
		y += rowHeight;
	}
	
	//node_iter_t cur = m_Base;
	
	m_RootNode.children.getFirst()->renderFrom(renderer, y, *this);
	
	/*
	while(isValid(cur) && y < m_Rect.y2)
	{
		cur->render(cur, aRenderer, y, m_Rect, *this);

		++cur;
		
		y += rowHeight;
	}
*/
	return true;
}

bool List::mouseDown(ulong newX, ulong newY, Context::MouseKey::type mutton)
{
	//TODO
	/*long offset = static_cast<long>((newY - getRect().y1) / rowHeight);
	
	node_iter_t cur = m_Base;
	
	
	
	for(node_iter_t cur = m_Base; isValid(cur); ++cur)
	{
		if(--offset < 0)
		{
			cur->selected = !cur->selected;
			m_MainSel = cur;
			return true;
		}
		
	}*/
	return false;
}

}
