#ifndef GFX_H
#define GFX_H

#include <allegro.h>

#include <string>
#include <list>
//#include "blitters/vtables.h"

enum Blenders
{
	ALPHA,
	ADD,
	NONE
};

class Gfx
{
public:
		
	Gfx();
	~Gfx();
	
	void init();
	void shutDown();
	void registerInConsole();
#ifndef DEDSERV
	void fullscreenChange();
	void doubleResChange();
	int  getGraphicsDriver(); // Selects and returns graphics driver
	
	void fullscreen( int oldValue );
	void doubleRes( int oldValue );
	
	void updateScreen();
	
	inline void setBlender( Blenders blender, int alpha )
	{
		drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
		switch ( blender )
		{
			case ALPHA:
				set_trans_blender(255, 255, 255, alpha);
			break;
			
			case ADD:
				set_add_blender( 255,255,255, alpha);
			break;
			
			case NONE:
				solid_mode();
			break;
		}
	}
	
	BITMAP* buffer;

	int m_distortionAA;
	
	int m_haxWormLight; //TEMP HAX
#endif
	
	BITMAP* loadBitmap(const std::string &filename, RGB* palette = NULL, bool keepAlpha = false);
	bool saveBitmap(const std::string &filename, BITMAP* image, RGB* palette = NULL);
	
	operator bool()
	{ return m_initialized; } // Returns true if it's safe to use this object

	
private:
	
	bool m_initialized;
	
#ifndef DEDSERV
	int m_fullscreen;
	int m_doubleRes;
	int m_vwidth;
	int m_vheight;
	int m_vsync;
	int m_clearBuffer;
	int m_filter;
	int m_driver;
	int m_bitdepth;

	BITMAP* m_doubleResBuffer;
	
	enum Filters
	{
		NO_FILTER,
		SCANLINES,
		SCANLINES2,
		BILINEAR
	};
#endif

};

#ifndef DEDSERV
std::string screenShot(const std::list<std::string> &args);
#endif

extern Gfx gfx;

struct LocalSetColorConversion
{
	LocalSetColorConversion(int flags)
	: old(get_color_conversion())
	{
		set_color_conversion(flags);
	}
	
	~LocalSetColorConversion()
	{
		set_color_conversion(old);
	}
	
private:
	int old;
};

struct LocalSetColorDepth
{
	LocalSetColorDepth(int depth)
	: old(get_color_depth())
	{
		set_color_depth(depth);
	}
	
	~LocalSetColorDepth()
	{
		set_color_depth(old);
	}
	
private:
	int old;
};

#endif // _GFX_H_
