#include "worm.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#include "base_player.h"
#include "player_options.h"
#include "base_animator.h"
#include "animators.h"
#include "sprite.h"
#include "weapon.h"
#include "ninjarope.h"

#include <math.h>
#include <zoidcom.h>

Worm::Worm() : BaseWorm()
{
}

Worm::~Worm()
{
}

void Worm::think()
{
	BaseWorm::think();
	renderPos = pos;
}

