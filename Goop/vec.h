#ifndef VEC_H
#define VEC_H

#include <cmath>

#include <boost/random.hpp>

extern boost::mt19937 rndgen;
extern boost::variate_generator<boost::mt19937, boost::uniform_real<> > rnd;
extern boost::variate_generator<boost::mt19937, boost::uniform_real<> > midrnd;

float const PI = float(3.14159265358979323846);

inline float deg2rad( float Degrees )
{
	return (Degrees * PI) / 180;
}

inline float rad2deg( float Radians )
{
	return (Radians * 180) / PI;
}

struct Vec
{
	
	inline Vec()
	{
		x=0;
		y=0;
	}

	inline Vec(float x_, float y_) : x(x_) , y(y_)
	{
	}
	
	inline Vec operator - ( Vec const &A ) const
	{
		return Vec( x - A.x, y - A.y );
	}
	
	inline Vec operator + ( Vec const &A ) const
	{
		return Vec(x+A.x,y+A.y);
	}
	
	friend Vec operator * ( float A, Vec const &B )
	{
		return Vec(A*B.x,A*B.y);
	}
	
	inline Vec operator * ( float A ) const
	{
		return Vec(A*x,A*y);
	}
	
	inline Vec operator / ( float A ) const
	{
		return Vec( x / A, y / A );
	}
	
	inline Vec& operator += ( Vec const &A )
	{
		x += A.x;
		y += A.y;
		return *this;
	}
	
	inline Vec& operator -= ( Vec const &A )
	{
		x -= A.x;
		y -= A.y;
		return *this;
	}
	
	inline Vec& operator *= ( float A )
	{
		x *= A;
		y *= A;
		return *this;
	}
	
	inline float dotProduct( Vec const &A ) const
	{
		return x*A.x + y*A.y;
	}
	
	inline float perpDotProduct( Vec const &A ) const
	{
		return x * A.y - y * A.x;
	}
	
	inline Vec perp() const
	{
		return Vec(-y,x);
	}
	
	inline float length() const
	{
		return sqrt(x*x + y*y);
	}
	
	inline Vec normal() const
	{
		float invLength = 1 / length();
		return Vec(invLength*x, invLength*y);
	}
	
	inline float lengthSqr() const
	{
		return x*x + y*y;
	}
	
	inline float getAngle() const
	{
		return rad2deg( atan2(x,-y) );
	}
	
	float x;
	float y;
	
};

inline Vec angleVec( float angle, float m )
{
	return Vec( sin( deg2rad(angle) ) * m, -cos( deg2rad(angle) ) * m );
}

#endif // _VEC_H_
