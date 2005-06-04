#include "player_ai.h"
#include "player_options.h"
#include "worm.h"
#include <vector>
#include <cmath>

// DONT USE THIS NYM, USE Vec::length() ! ;D
inline float distance(float x1, float y1, float x2, float y2)
{
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

const float cAimAccuracy = 8.0;

PlayerAI::PlayerAI()
	: m_pathSteps(100)
{
	m_options = new PlayerOptions;
}

PlayerAI::~PlayerAI()
{
	delete m_options;
}

void PlayerAI::getTarget()
{
	// Added random to make it not be instant
	// I also wonder why is this here and not in the think function
	if ( !m_worm->isActive() && rnd()*100 < 1)
		baseActionStart(RESPAWN);

	//iterate through players
	Vec pos = m_worm->getPos();
	int x = static_cast<int>(pos.x);
	int y = static_cast<int>(pos.y);
	float tmpDist = 0;
	BasePlayer* tmpTarget = NULL;
	for (std::vector<BasePlayer*>::iterator iter = game.players.begin(); iter != game.players.end(); iter++)
	{
		//(*iter)->think();
		Vec tmpPos = (*iter)->getPos(); // WARNING: There can be players without worms! "spectators".
		int tmpX = static_cast<int>(tmpPos.x);
		int tmpY = static_cast<int>(tmpPos.y);
		if (distance(x, y, tmpX, tmpY) < tmpDist || tmpTarget == NULL)
		{
			if (*iter != this)
			{
				tmpTarget = *iter;
				tmpDist = distance(x, y, tmpX, tmpY);
			}
		}
	}
	m_target = tmpTarget;
}

void PlayerAI::getPath()
{
	Vec pos = m_worm->getPos();		//AI position
	Vec target = m_worm->getPos();		//Target position
	
	//create "nodes" array
	for (int y = 0; y < 128; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			m_nodes[y][x] = 0;	//0 - unwalkable
			m_nodes[y][x] = 1;	//1 - walkable (uncalculated)
		}
	}
}

void PlayerAI::subThink()
{
	getTarget();
	if (!m_target)
		return;
	
	//movement
	Vec pos = m_worm->getPos();		//AI position
	Vec target = m_target->getPos();	//Target position
	
	float dist = distance(pos.x, pos.y, target.x, target.y); 
	// nym!! change this ^^^ to (pos - target).length() !!!
	if (dist > 48.f)
	{ 
		if (pos.x < target.x)
			baseActionStart(RIGHT);
		else
			baseActionStop(RIGHT);
		if (pos.x > target.x)
			baseActionStart(LEFT);
		else
			baseActionStop(LEFT);
	} else
	{
		baseActionStop(LEFT);
		baseActionStop(RIGHT);

		// The ai player shouldnt modify the dir like this
		// Use the baseActions to move the player, or else it will not
		// work over network games :<
		//face
		if (target.x > pos.x)
			;//m_worm->setDir(1);
		else
			;//m_worm->setDir(-1);
	}
	
	
	//aiming
	float curAngle = m_worm->getAngle() / m_worm->getDir();
	float targetAngle = atan2(target.y - pos.y, target.x - pos.x); // use Vec::getAngle()
	
	//convert to gusanos angle system
	targetAngle -= deg2rad(90);
	targetAngle = deg2rad(180) + targetAngle;
	if (m_worm->getDir() == -1)
	targetAngle = deg2rad(180) - (targetAngle - deg2rad(180));
	
	while (curAngle < 0)
		curAngle += 360;
	while (curAngle >= 360)
		curAngle -= 360;
	
	while (rad2deg(targetAngle) < 0)
		targetAngle += deg2rad(360);
	while (rad2deg(targetAngle) >= 360)
		targetAngle -= deg2rad(360);
	
	//aim
	if (curAngle - cAimAccuracy > rad2deg(targetAngle))
                m_worm->aimSpeed = -0.48;
	
	if (curAngle + cAimAccuracy < rad2deg(targetAngle))
                m_worm->aimSpeed = 0.48;

	//shooting
	if (curAngle - cAimAccuracy < rad2deg(targetAngle) && curAngle + cAimAccuracy > rad2deg(targetAngle))
	{
		//aimed accuratley
		baseActionStart(FIRE);
	} else
		baseActionStop(FIRE);


	// TODO: Make decent behaviour
	//jump
	if (rand() % 32 == 0)
		baseActionStart(JUMP);

        //rope
	if (rand() % 128 == 0)
		baseActionStart(NINJAROPE);
	else if (rand() % 96 == 0)
		baseActionStop(NINJAROPE);

	
}

