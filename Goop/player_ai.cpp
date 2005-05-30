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

void PlayerAI::subThink()
{
	getTarget();
	if (!m_target)
		return;
	
	//movement
	Vec pos = m_worm->getPos();
	int x = static_cast<int>(pos.x);
	int y = static_cast<int>(pos.y);
	
	Vec tmpPos = m_target->getPos();
	int tmpX = static_cast<int>(tmpPos.x);
	int tmpY = static_cast<int>(tmpPos.y);
	
	float dist = distance(x, y, tmpX, tmpY);
	if (dist > 48.f)
	{ 
		if (x < tmpX)
			baseActionStart(RIGHT);
		else
			baseActionStop(RIGHT);
		if (x > tmpX)
			baseActionStart(LEFT);
		else
			baseActionStop(LEFT);
	} else
	{
		baseActionStop(LEFT);
		baseActionStop(RIGHT);
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
                m_worm->aimSpeed = -0.18;
	
	if (curAngle + cAimAccuracy < rad2deg(targetAngle))
                m_worm->aimSpeed = 0.18;

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
            m_worm->actionStart(Worm::JUMP);

        //rope
        if (rand() % 128 == 0)
            m_worm->actionStart(Worm::NINJAROPE);
        else if (rand() % 96 == 0)
            m_worm->actionStop(Worm::NINJAROPE);

	
}

