#ifndef SFX_H
#define SFX_H

class BaseObject;

#include <vector>

class Sfx
{
	public:
		
	Sfx();
	~Sfx();
	
	void init();
	void shutDown();
	void registerInConsole();
	void updateChanPositions();
	void setChanObject( int chan, BaseObject* obj );
	
	// *** THIS MUST BE PLACED BEFORE THE OBJECT GETS REALLY DELETED *** // or... CRASH! :>
	void checkForDeletedObjects(); 
	
	private:
	
	std::vector< BaseObject* > chanObject;
	
	int m_vsync;
	int m_clearBuffer;

};

extern Sfx sfx;

#endif // _GFX_H_
