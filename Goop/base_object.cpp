#include "base_object.h"
#include "vec.h"

BaseObject::BaseObject()
{
	deleteMe = false;
}

Vec& BaseObject::getPos()
{
	return pos;
}

Vec& BaseObject::getSpd()
{
	return spd;
}

float BaseObject::getAngle()
{
	return 0;
}

