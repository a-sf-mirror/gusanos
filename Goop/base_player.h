#ifndef BASE_PLAYER_H
#define BASE_PLAYER_H

struct PlayerOptions;

#include <string>
#include "vec.h"

class Worm;

class BasePlayer
{
	public:
	
	BasePlayer();
	virtual ~BasePlayer();
	
	virtual void think() = 0;
	virtual void render() {};

	void assignWorm(Worm* worm);
	
	PlayerOptions* getOptions();
	Vec getPos();
	
	protected:
	
	int deaths;
	int kills;

	Worm* m_worm;
	PlayerOptions* m_options;
};

#endif  // _BASE_PLAYER_H_
