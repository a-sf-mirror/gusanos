#ifndef VEC_H
#define VEC_H

struct Vec
{
	Vec();
	Vec(float x_, float y_);
	
	Vec operator + (const Vec &A);
	Vec operator - (const Vec &A);
	Vec operator * (float number);
	
	float x;
	float y;
};

#endif // _VEC_H_
