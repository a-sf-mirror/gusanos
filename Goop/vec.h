#ifndef VEC_H
#define VEC_H

#include <cmath>
#include "angle.h"
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

template<class T>
struct BaseVec
{
	
	BaseVec()
	: x(0), y(0)
	{
		
	}
	
	BaseVec(Angle angle, double length)
	{
		double radians = angle.toRad();
		x = sin(radians) * length;
		y = -cos(radians) * length;
	}
	
	BaseVec(Angle angle)
	{
		double radians = angle.toRad();
		x = sin(radians);
		y = -cos(radians);
	}
	
	template<class T2>
	BaseVec(BaseVec<T2> const& b)
	: x(static_cast<T>(b.x)), y(static_cast<T>(b.y))
	{
		
	}
	
	BaseVec(BaseVec const& a, BaseVec const& b)
	: x(b.x - a.x), y(b.y - a.y)
	{
		
	}

	BaseVec(T const& x_, T const& y_)
	: x(x_) , y(y_)
	{
	}
	
	void zero()
	{
		x = static_cast<T>(0);
		y = static_cast<T>(0);
	}
	
	BaseVec operator - (BaseVec const &A) const
	{
		return BaseVec(x - A.x, y - A.y);
	}
	
	BaseVec operator + (BaseVec const &A) const
	{
		return BaseVec(x + A.x, y + A.y);
	}
	
	friend BaseVec operator * (T const& A, BaseVec const& B)
	{
		return BaseVec(A * B.x, A * B.y);
	}
	
	BaseVec operator * (T const& A) const
	{
		return BaseVec(A * x, A * y);
	}
	
	BaseVec operator / (T const& A) const
	{
		return BaseVec(x / A, y / A);
	}
	
	BaseVec& operator += (BaseVec const& A)
	{
		x += A.x;
		y += A.y;
		return *this;
	}
	
	BaseVec& operator -= (BaseVec const& A)
	{
		x -= A.x;
		y -= A.y;
		return *this;
	}
	
	BaseVec& operator *= (T const& A)
	{
		x *= A;
		y *= A;
		return *this;
	}
	
	T dotProduct(BaseVec const& A) const
	{
		return x * A.x + y * A.y;
	}
	
	T perpDotProduct(BaseVec const& A) const
	{
		return x * A.y - y * A.x;
	}
	
	BaseVec perp() const
	{
		return BaseVec(-y, x);
	}
	
	double length() const
	{
		return sqrt(x*x + y*y);
	}
	
	BaseVec normal() const
	{
		double invLength = 1.0 / length();
		return BaseVec(invLength*x, invLength*y);
	}
	
	T lengthSqr() const
	{
		return x*x + y*y;
	}
	
	Angle getAngle() const
	{
		// TODO: Optimize this
		return Angle( rad2deg( atan2(double(x), double(-y)) ) );
	}
	
	T x;
	T y;
	
};

/*
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
	
};*/

typedef BaseVec<float> Vec;

/*
inline Vec angleVec( float angle, float m )
{
	return Vec( sin( deg2rad(angle) ) * m, -cos( deg2rad(angle) ) * m );
}*/

#endif // _VEC_H_
