#ifndef AI_H
#define AI_H

#include "base_player.h"
#include "worm.h"
#include "base_object.h"

//AI Worm Player
class PlayerAI : public BasePlayer
{
	public:
	PlayerAI();
	~PlayerAI();
	void getTarget();	//Find target
	void getPath();		//Create path for AI worm to follow (to get to target)
	void subThink();	//AI processing
	
	private:
	unsigned int m_pathSteps;	//"steps" to take in A* pathfinding
	int m_nodes[128][128];	//A* nodes
	BasePlayer* m_target;	//Target worm
	float randomError; // I temporally use this to make it aim worse or sth.
};

#endif
