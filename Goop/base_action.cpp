#include "base_action.h"

BaseAction::BaseAction(int flags_)
: flags(flags_)
{
}

BaseAction::~BaseAction() // <GLIP> Virtual dtor always needed for classes with virtual functions
{
}
