#include "list.h"

namespace OmfgGUI
{

void List::Node::render(List::node_iter_t self, Renderer* renderer, long& y, List& list)
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

	if(self == list.m_MainSel)
	{
		//TODO: aRenderer->drawFrame(Rect(list.m_Rect.x1 + 1, y, list.m_Rect.x2 - 2, y + rowHeight), RGB(0, 0, 0));
	}
	
	//renderer->drawText(list.m_context->m_defaultFont, text, BaseFont::CenterV, list.m_rect.x1 + 3 + level * 5, y + halfRowHeight, RGB(0, 0, 0));
	
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

void List::Node::renderFrom(node_iter_t i, Renderer* renderer, long& y, List& list)
{
	node_iter_t parent = i->parent;
	bool        hasParent = i->hasParent;
	list_t*     parentList = i->parentList;
	//assert(parentList);
	
	//Only the root element has parentList == 0, and we can't even obtain an iterator
	//to the root element, thus it's safe to assume that parentList is a valid pointer.
	
	while(y < list.getRect().y2)
	{
		i->render(i, renderer, y, list);
	
		y += rowHeight;
		++i;
		while(i == parentList->end())
		{
			if(!hasParent)
				return;
				
			i = parent;
			parentList = i->parentList;
			//assert(parentList);
			hasParent = i->hasParent;
			parent = i->parent;
			++i;
		}
	}
}

List::node_iter_t List::Node::findByIdx(node_iter_t i, long aIdx)
{
	node_iter_t parent = i->parent;
	bool        hasParent = i->hasParent;
	list_t*     parentList = i->parentList;
	//assert(parentList);
	
	//Only the root element has parentList == 0, and we can't even obtain an iterator
	//to the root element, thus it's safe to assume that parentList is a valid pointer.
	
	while(--aIdx >= 0)
	{
		node_iter_t lastValid = i;
		
		if(i->expanded)
		{
			parent = i;
			hasParent = true;
			parentList = &i->children;
			i = i->children.begin();
			
		}
		
		++i;
		while(i == parentList->end())
		{
			if(!hasParent)
				return lastValid;
				
			i = parent;
			parentList = i->parentList;
			//assert(parentList);
			hasParent = i->hasParent;
			parent = i->parent;
			++i;
		}
	}
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
	
	node_iter_t cur = m_Base;
	
	//Node::renderFrom(m_Base, renderer, y, *this);
	
	/*
	while(isValid(cur) && y < m_Rect.y2)
	{
		cur->render(cur, aRenderer, y, m_Rect, *this);

		++cur;
		
		y += rowHeight;
	}
*/
}

bool List::mouseDown(ulong newX, ulong newY, Context::MouseKey::type mutton)
{
	long offset = (newY - getRect().y1) / rowHeight;
	
	node_iter_t cur = m_Base;
	
	//TODO
	/*
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
