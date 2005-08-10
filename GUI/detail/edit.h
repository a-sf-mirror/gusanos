#ifndef OMFG_GUI_EDIT
#define OMFG_GUI_EDIT

#include "common.h"
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
	: Wnd(parent, tagLabel, className, id, attributes, text), m_select(false), m_caretPos(0), m_selTo(0)
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
	
	//void Change(long a_Mult);
	
	//void SetText(std::string a_Text);
	
	virtual bool keyDown(int key);
	virtual bool keyUp(int key);
	
	virtual bool charPressed(char c, int key);
	
	/*
	virtual bool keyUp(int key);*/
	
	virtual int classID();
	
protected:

	ulong		m_caretPos;
	ulong		m_selTo;
	bool        m_select;
	
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

