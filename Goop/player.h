#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player
{
	public:
	
	enum Actions
	{
		ACTION_LEFT;
		ACTION_RIGHT;
		ACTION_UP;
		ACTION_DOWN;
		ACTION_FIRE;
		ACTION_JUMP;
		ACTION_CHANGE;
	}
		
	Player();
	~Player();
	
	void think();
	void render();
	
	void assignViewport(Viewport* Viewport);
	void assignWorm(Worm* worm);
	
	void actionStart( Actions action );
	void actionEnd( Actions action );
	
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
