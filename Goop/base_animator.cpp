#include "base_animator.h"

BaseAnimator::BaseAnimator()
{
	freezeTicks = 0;
}

BaseAnimator::~BaseAnimator() // <GLIP> Virtual dtor always needed for classes with virtual functions
{
}

void BaseAnimator::freeze(int ticks)
{
	freezeTicks = ticks;
}
