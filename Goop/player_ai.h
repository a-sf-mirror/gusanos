#ifndef AI_H
#define AI_H

#include "base_player.h"
#include "worm.h"
#include "base_object.h"

class PlayerAI : public BasePlayer
{
    BasePlayer* m_target;
    public:
        PlayerAI() { }
        void getTarget();       //Find target
        void think();           //AI processing
};

#endif
