#ifndef VIEWPORT_H
#define VIEWPORT_H

#ifdef DEDSERV
#error "Can't use this in dedicated server"
#endif //DEDSERV

//#include "game.h"
#include "omfgutil_math.h"
#include "lua/types.h"
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
/*
<<<<<<< viewport.h
	
	
	BITMAP* getBitmap() { return m_dest; }
=======
*/
	void drawLight(IVec const& v); // TEMP
	
	IVec getPos()
	{
		return IVec(m_pos);
	}
	
	IVec convertCoords( IVec const & coord )
	{
		return coord - IVec(m_pos);
	}
	
	Vec convertCoordsPrec( Vec const & coord )
	{
		return coord - m_pos;
	}
	
	BITMAP* getBitmap() { return dest; }
/*
>>>>>>> 1.9
	*/
	LuaReference luaReference;
	
	BITMAP* dest;
	BITMAP* testFade;
	
private:

	Listener* m_listener;
	Vec m_pos;
	
};

#endif // _VIEWPORT_H_
