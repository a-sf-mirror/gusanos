#ifndef BASE_PLAYER_H
#define BASE_PLAYER_H

struct PlayerOptions;

#include <string>
#include "vec.h"

class BaseWorm;

class BasePlayer
{
	public:
	
	BasePlayer();
	virtual ~BasePlayer();
	
	virtual void think() = 0;
	virtual void render() {};

	void assignWorm(BaseWorm* worm);
	
	PlayerOptions* getOptions();
	Vec getPos();
	
	protected:
	
	int deaths;
	int kills;

	BaseWorm* m_worm;
	PlayerOptions* m_options;
};

#endif  // _BASE_PLAYER_H_
