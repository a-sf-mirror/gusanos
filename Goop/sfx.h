#ifndef SFX_H
#define SFX_H

class Sfx
{
	public:
		
	Sfx();
	~Sfx();
	
	void init();
	void shutDown();
	void registerInConsole();
	
	private:
	
	int m_vsync;
	int m_clearBuffer;

};

extern Sfx sfx;

#endif // _GFX_H_
