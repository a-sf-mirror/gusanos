#ifndef OMFG_GUI_COMMON_H
#define OMFG_GUI_COMMON_H

typedef unsigned char uchar;
typedef unsigned long ulong;

template<class T>
class BasicRect
{
public:
	BasicRect(void)
	{
	}

	BasicRect(T x1_, T y1_, T x2_, T y2_)
	: x1(x1_), y1(y1_), x2(x2_), y2(y2_)
	{

	}

	T x1;
	T y1;
	T x2;
	T y2;
	
	T centerX() const
	{
		return (x1 + x2) / T(2);
	}
	
	T centerY() const
	{
		return (y1 + y2) / T(2);
	}

	T getWidth() const
	{
		return x2 - x1;
	}

	T getHeight() const
	{
		return y2 - y1;
	}
	
	BasicRect flip() const
	{
		return BasicRect<T>(y1, x1, y2, x2);
	}

	bool isValid()
	{
		return x1 <= x2 && y1 <= y2;
	}
	
	void join(BasicRect const& b)
	{
		if(b.x1 < x1)
			x1 = b.x1;
		if(b.y1 < y1)
			y1 = b.y1;
		if(b.x2 > x2)
			x2 = b.x2;
		if(b.y2 > y2)
			y2 = b.y2;
	}

	bool isIntersecting(BasicRect const& b) const
	{
		if(b.y2 < y1
			|| b.y1 > y2
			|| b.x2 < x1
			|| b.x1 > x2)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool intersect(BasicRect const& b)
	{
		if(!isIntersecting(b))
		{	  
			return false;
		}

		if(b.x1 > x1)
			x1 = b.x1;
		if(b.y1 > y1)
			y1 = b.y1;
		if(b.x2 < x2)
			x2 = b.x2;
		if(b.y2 < y2)
			y2 = b.y2;

		return true;
	}

	bool isInside(T x, T y) const
	{
		T diffX = x - x1;
		T diffY = y - y1;
		
		return diffX < getWidth() && diffX >= T(0)
		    && diffY < getHeight() && diffY >= T(0);
		
	}
};

typedef BasicRect<int> Rect;
typedef BasicRect<float> FRect;

#endif //OMFG_GUI_COMMON_H
