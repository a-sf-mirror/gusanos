#ifndef OMFG_GUI_LIST_H
#define OMFG_GUI_LIST_H

#include "wnd.h"

#include <string>
#include <list>

namespace OmfgGUI
{

class List : public Wnd
{
public:
	struct Node;
	
	typedef std::list<Node> list_t;
	typedef list_t::iterator node_iter_t;
	typedef list_t::reference node_ref_t;
	static const long rowHeight = 12;
	
	struct Node
	{
		Node(std::string const& aText)
			: text(aText), selected(false), expanded(false), level(0),
			  parentList(0), hasParent(false)
		{
		}
		
		static node_iter_t push_back(node_iter_t self, Node const& aNode)
		{
			self->children.push_back(aNode);
				
			node_iter_t i = self->children.end();
			
			--i;
			
			i->level = self->level + 1;
			i->parentList = &self->children;
			i->parent = self;
			i->hasParent = true;

			return i;
		}
		
		//Disjoint insert (does not have a parent)
		node_iter_t push_back(Node const& aNode)
		{
			children.push_back(aNode);
				
			node_iter_t i = children.end();
			
			--i;
			
			i->level = level + 1;
			i->parentList = &children;

			return i;
		}

		void render(List::node_iter_t self, Renderer* renderer, long& y, List& list);
		//void renderChildren(Renderer* aRenderer, long& y, List& list);
		static void renderFrom(node_iter_t i, Renderer* renderer, long& y, List& list);
		static node_iter_t findByIdx(node_iter_t i, long aIdx);
		
		static bool isValid(node_iter_t aIter)
		{
			return aIter != aIter->parentList->end();
		}
		
		std::string text;
		bool        selected;
		bool        expanded;
		list_t*     parentList;
		node_iter_t parent;
		bool        hasParent;
		long        level;
		//TODO: columns
	private:
		list_t      children;
	};
	
	friend struct Node;
	
	List(Wnd* parent, std::string const& tagLabel, std::string const& className,
	  std::string const& id, std::map<std::string, std::string> const& attributes)
	: Wnd(parent, tagLabel, className, id, attributes, ""), m_RootNode("root")
	{
		//m_MainSel = m_Base = m_Nodes.end(); //TODO
		
	}
	
	node_iter_t push_back(Node const& aNode)
	{
	/*
		m_Nodes.push_back(aNode);
		
		if(!isValid(m_Base))
			m_Base =  m_Nodes.begin();
			
		node_iter_t i = m_Nodes.end();
		
		--i;*/
		
		node_iter_t i = m_RootNode.push_back(aNode);
		

		return i;
	}
	
	bool isValid()
	{
	}

	virtual bool render(Renderer* renderer);
	virtual bool mouseDown(ulong newX, ulong newY, Context::MouseKey::type button);

private:
	//list_t           m_Nodes;
	Node             m_RootNode;
	list_t::iterator m_Base;
	list_t::iterator m_MainSel;
	
};

}

#endif //OMFG_GUI_LIST_H
