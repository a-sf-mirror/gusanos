#ifndef VIEWPORT_H
#define VIEWPORT_H

#ifdef DEDSERV
#error "Can't use this in dedicated server"
#endif //DEDSERV

//#include "game.h"
#include "vec.h"
//#include <allegro.h>

struct Listener;
struct BITMAP;
class BasePlayer;

class Viewport
{
public:
		
	Viewport();
	~Viewport();
	
	void setDestination(BITMAP* where, int x, int y, int w, int h);
	void render(BasePlayer* player);

	void setPos(float x, float y);
	void interpolateTo(float x, float y, float factor);
	void interpolateTo(Vec dest, float factor);
	
	BITMAP* getBitmap() { return m_dest; }
	
	int luaReference;
	
private:

	BITMAP* m_dest;
	Listener* m_listener;
	Vec m_pos;
	
};

#endif // _VIEWPORT_H_
