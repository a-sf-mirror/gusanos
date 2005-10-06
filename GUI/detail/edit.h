#ifndef OMFG_GUI_EDIT
#define OMFG_GUI_EDIT

#include "omfgutil_math.h"
#include "omfgutil_common.h"
#include <string>
#include "wnd.h"

namespace OmfgGUI
{

class Edit : public Wnd
{
public:
	Edit(Wnd* parent, std::string const& tagLabel, std::string const& className,
	  std::string const& id, std::map<std::string, std::string> const& attributes,
	  std::string const& text = std::string(""))
	: Wnd(parent, tagLabel, className, id, attributes, text)
	, m_select(false), m_caretPos(0), m_selTo(0)
	, m_hscroll(0)
	{

	}
	
	virtual bool render(Renderer* renderer);
	
	virtual void process();

	void setCaretPos(long caretPos)
	{
		if(m_caretPos != caretPos)
		{
			m_caretPos = caretPos;
			//InvalidateWhole();
		}
	}
	
	void setSelTo(long selTo)
	{
		if(m_selTo != selTo)
		{
			m_selTo = selTo;
			//InvalidateWhole();
		}
	}
	
	virtual void setText(std::string const& aStr);
	
	virtual bool keyDown(int key);
	virtual bool keyUp(int key);
	
	virtual bool charPressed(char c, int key);
	
	void assertCaretVisibility(Renderer* renderer);
	
	/*
	virtual bool keyUp(int key);*/
	
	virtual int classID();
	
protected:

	bool        m_select;
	ulong		m_caretPos;
	ulong		m_selTo;
	long        m_hscroll;
	
/*
	bool		m_OnlyNumbers;
	bool		m_ShowUpDownButton;
	long		m_StepSizeI;
	long		m_LowLimitI;
	long		m_HighLimitI;
	
	long		m_ScrollStartY;
	bool		m_Scrolling;*/
};

}

#endif

