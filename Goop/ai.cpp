#include "ai.h"
#include "worm.h"
#include <vector>
#include <cmath>

inline float distance(float x1, float y1, float x2, float y2)
{
    return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
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
        Vec tmpPos = (*iter)->getPos();
        int tmpX = tmpPos.x;
        int tmpY = tmpPos.y;
        if (abs(distance(x, y, tmpX, tmpY)) < tmpDist || tmpTarget == NULL)
        {
            tmpTarget = *iter;
        }
    }
    m_target = tmpTarget;
}

void PlayerAI::think()
{
    Player::think();
    getTarget();

    Vec pos = m_worm->getPos();
    int x = pos.x;
    int y = pos.y;

    Vec tmpPos = m_target->getPos();
    int tmpX = tmpPos.x;
    int tmpY = tmpPos.y;

    if (abs(distance(x, y, tmpX, tmpY)) > 32)
    { 
        if (x < tmpX)
            actionStart(RIGHT);
        else
            actionStop(RIGHT);
        if (x > tmpX)
            actionStart(LEFT);
        else
            actionStop(LEFT);
    } else
    {
        actionStop(LEFT);
        actionStop(RIGHT);
    }
}

