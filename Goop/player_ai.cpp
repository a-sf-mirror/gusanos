#include "player_ai.h"
#include "player_options.h"
#include "worm.h"
#include <vector>
#include <cmath>

inline float distance(float x1, float y1, float x2, float y2)
{
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

const float cAimAccuracy = 8.0;

PlayerAI::PlayerAI()
{
	m_options = new PlayerOptions;
}

PlayerAI::~PlayerAI()
{
	delete m_options;
}

void PlayerAI::getTarget()
{
	//iterate through players
	Vec pos = m_worm->getPos();
	int x = pos.x;
	int y = pos.y;
	float tmpDist = 0;
	BasePlayer* tmpTarget = NULL;
	for (std::vector<BasePlayer*>::iterator iter = game.players.begin(); iter != game.players.end(); iter++)
	{
		//(*iter)->think();
		Vec tmpPos = (*iter)->getPos(); // WARNING: There can be players without worms! "spectators".
		int tmpX = tmpPos.x;
		int tmpY = tmpPos.y;
		if (abs(distance(x, y, tmpX, tmpY)) < tmpDist || tmpTarget == NULL)
		{
			if (*iter != this)
			{
				tmpTarget = *iter;
				tmpDist = abs(distance(x, y, tmpX, tmpY));
			}
		}
	}
	m_target = tmpTarget;
}

void PlayerAI::think()
{
	getTarget();
	if (!m_target)
		return;
	
	//movement
	Vec pos = m_worm->getPos();
	int x = pos.x;
	int y = pos.y;
	
	Vec tmpPos = m_target->getPos();
	int tmpX = tmpPos.x;
	int tmpY = tmpPos.y;
	
	int dist = abs(distance(x, y, tmpX, tmpY));
	if (dist > 48)
	{ 
		if (x < tmpX)
			m_worm->actionStart(Worm::MOVERIGHT);
		else
			m_worm->actionStop(Worm::MOVERIGHT);
		if (x > tmpX)
			m_worm->actionStart(Worm::MOVELEFT);
		else
			m_worm->actionStop(Worm::MOVELEFT);
	} else if (dist < 24)
	{
		if (x > tmpX)
			m_worm->actionStart(Worm::MOVERIGHT);
		else
			m_worm->actionStop(Worm::MOVERIGHT);
		if (x < tmpX)
			m_worm->actionStart(Worm::MOVELEFT);
		else
			m_worm->actionStop(Worm::MOVELEFT);
	} else
	{
		m_worm->actionStop(Worm::MOVELEFT);
		m_worm->actionStop(Worm::MOVERIGHT);
	}
	
	
	//aiming
	float curAngle = m_worm->getAngle() / m_worm->getDir();
	float targetAngle = atan2(tmpY - y, tmpX - x);
	
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
		m_worm->actionStart(Worm::AIMUP);
	else
		m_worm->actionStop(Worm::AIMUP);
	
	if (curAngle + cAimAccuracy < rad2deg(targetAngle))
		m_worm->actionStart(Worm::AIMDOWN);
	else
		m_worm->actionStop(Worm::AIMDOWN);
	
	//shooting
	if (curAngle - cAimAccuracy < rad2deg(targetAngle) && curAngle + cAimAccuracy > rad2deg(targetAngle))
	{
		//aimed accuratley
		m_worm->actionStart(Worm::FIRE);
	} else
		m_worm->actionStop(Worm::FIRE);
}

