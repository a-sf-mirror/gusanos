#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Viewport;
class Worm;
struct BITMAP;

class Player
{
	public:
	
	enum Actions
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		FIRE,
		JUMP,
		CHANGE
	};
		
	Player();
	~Player();
	
	void think();
	void render();
	
	void assignViewport(Viewport* Viewport);
	void assignWorm(Worm* worm);
	
	void actionStart( Actions action );
	void actionStop( Actions action );
	
	private:
	
	std::string name;
	int team;
	int deaths;
	int kills;
	int color;

	Worm* m_worm;
	Viewport* m_viewport;

};

#endif  // _WORM_H_
