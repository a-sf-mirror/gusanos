#ifndef AI_H
#define AI_H

#include "player.h"
#include "worm.h"
#include "base_object.h"

class PlayerAI : public Player
{
    BasePlayer* m_target;
    public:
        PlayerAI(PlayerOptions *options) : Player(options) { }
        void getTarget();       //Find target
        void think();           //AI processing
};

#endif
