#ifndef GFX_H
#define GFX_H

#include <allegro.h>

#include <string>
#include <list>

class Gfx
{
	public:
		
	Gfx();
	~Gfx();
	
	void init();
	void shutDown();
	void registerInConsole();
	void fullscreenChange();
	void doubleResChange();
	
	BITMAP* loadBitmap(const std::string &filename, RGB* palette = NULL);
	bool saveBitmap(const std::string &filename, BITMAP* image, RGB* palette = NULL);
	
	void updateScreen();

	
	BITMAP* buffer;
	
	private:
	
	int m_fullscreen;
	int m_doubleRes;
	int m_vwidth;
	int m_vheight;
	int m_vsync;
	int m_clearBuffer;
	int m_filter;
	
	BITMAP* m_doubleResBuffer;
	
	enum Filters
	{
		NO_FILTER,
		SCANLINES
	};

};

std::string screenShot(const std::list<std::string> &args);

extern Gfx gfx;

#endif // _GFX_H_
