#include "vec.h"

Vec::Vec()
{
	x=0;
	y=0;
}

Vec::Vec(float x_, float y_)
{
	x=x_;
	y=y_;
}

Vec Vec::operator + (const Vec &A)
{
	return Vec(x + A.x, y + A.y);
}

Vec Vec::operator - (const Vec &A)
{
	return Vec(x - A.x, y - A.y);
}

Vec Vec::operator * (float number)
{
	return Vec(x * number, y * number);
}

