#include "base_object.h"
#include "vec.h"

BaseObject::BaseObject()
{
	deleteMe = false;
}

BaseObject::~BaseObject()
{
}

Vec BaseObject::getPos()
{
	return pos;
}

Vec BaseObject::getSpd()
{
	return spd;
}

float BaseObject::getAngle()
{
	return 0;
}

void BaseObject::remove()
{
	deleteMe = true;
}
