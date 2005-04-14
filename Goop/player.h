#ifndef PLAYER_H
#define PLAYER_H

#include "base_player.h"
#include <string>

class Viewport;
class Worm;
class PlayerOptions;
struct BITMAP;

class Player : public BasePlayer
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
		
	Player(PlayerOptions* options);
	~Player();
	
	void think();
	void render();
	
	void assignViewport(Viewport* Viewport);

	void actionStart( Actions action );
	void actionStop( Actions action );
	
private:
	
	bool aimingUp;
	bool aimingDown;
	bool changing;
	bool jumping;

	Viewport* m_viewport;
};

#endif  // _WORM_H_
