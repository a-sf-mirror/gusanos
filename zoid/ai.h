// AI Player
// nym 16.10.04 - Created.

#ifndef AI_H
#define AI_H

#include "player.h"

#define DIR_LEFT -1
#define DIR_RIGHT 1

#define WALK_ACCEL 100
#define WALK_MAX 500

#define PI 3.14159
#define TODEG(x) (x)/(PI/180)

class wormai : public worm
{
  int target;//Index of targetted worm
 public:
  wormai();
  void update();
};

#endif

