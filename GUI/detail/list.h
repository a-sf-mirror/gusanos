#ifndef OMFG_GUI_LIST_H
#define OMFG_GUI_LIST_H

#include "wnd.h"
#include "llist.h"

#include <string>
//#include <list>

namespace OmfgGUI
{

class List : public Wnd
{
public:
	struct Node;
	
	// typedef std::list<Node> list_t;
	typedef LList<Node> list_t;
	typedef list_t::iterator node_iter_t;
	typedef list_t::reference node_ref_t;
	static const long rowHeight = 12;
	
	struct ColumnHeader
	{
		ColumnHeader(std::string const& name_, double widthFactor_)
		: name(name_), widthFactor(widthFactor_)
		{
		}
		
		std::string name;
		double      widthFactor;
	};
	
	struct Node : public LNodeImp<Node>
	{
		friend class List;
		
		Node(std::string const& aText)
			: text(aText), selected(false), expanded(true),
			  parent(0), level(0)
		{
		}
		
		static node_iter_t push_back(node_iter_t self, Node* aNode)
		{
			self->children.insert(aNode);
			
			aNode->level = self->level + 1;
			//i->parentList = &self->children;
			aNode->parent = self;
			//i->hasParent = true;

			return node_iter_t(aNode);
		}
		
		//Disjoint insert (does not have a parent)
		node_iter_t push_back(Node* aNode)
		{
			children.insert(aNode);
			
			aNode->level = level + 1;
			//aNode->parentList = &children;
			

			return node_iter_t(aNode);
		}

		void render(Renderer* renderer, long& y, List& list);
		//void renderChildren(Renderer* aRenderer, long& y, List& list);
		void renderFrom(Renderer* renderer, long& y, List& list);
		static node_iter_t findByIdx(node_iter_t i, long aIdx);
		
		/*
		static bool isValid(node_iter_t aIter)
		{
			return aIter != aIter->parentList->end();
		}*/
		
		std::string text;
		bool        selected;
		bool        expanded;
		//list_t*     parentList;
		node_iter_t parent;
		//bool        hasParent;
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
		m_columnHeaders.push_back(ColumnHeader("Moo", 1.0));
		
	}
	
	node_iter_t push_back(Node* aNode)
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
		return false;
	}

	virtual bool render(Renderer* renderer);
	virtual bool mouseDown(ulong newX, ulong newY, Context::MouseKey::type button);

private:
	//list_t           m_Nodes;
	Node             m_RootNode;
	list_t::iterator m_Base;
	list_t::iterator m_MainSel;
	std::vector<ColumnHeader> m_columnHeaders;
};

}

#endif //OMFG_GUI_LIST_H
