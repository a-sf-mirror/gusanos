#ifndef SFX_H
#define SFX_H

#include "vec.h"
#include <list>
#include <vector>

class BaseObject;

struct Listener
{
	Vec pos;
	Vec spd;
};

class Sfx
{
	public:
		
	Sfx();
	~Sfx();
	
	void init();
	void shutDown();
	void registerInConsole();
	void think();
	void setChanObject( int chan, BaseObject* obj );
	void clear();
	Listener* newListener();
	void freeListener(Listener* listener);
	
	// *** THIS MUST BE PLACED BEFORE THE OBJECT GETS REALLY DELETED *** // or... CRASH! :>
	
	operator bool()
	{ return m_initialized; } // Returns true if it's safe to use this object
	
	private:
	
	bool m_initialized;
	
	std::list< std::pair< int, BaseObject* > > chanObject;
	std::vector<Listener*> listeners;
	
	int m_listenerDistance;
	int m_outputMode;

};

extern Sfx sfx;

#endif // _GFX_H_
