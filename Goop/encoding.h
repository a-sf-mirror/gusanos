#ifndef GUSANOS_ENCODING_H
#define GUSANOS_ENCODING_H

#include <utility>
#include <cassert>
//#include <boost/dynamic_bitset.hpp>
#include "omfggui.h" // For Rect
#include "zoidcom.h"
#include <iostream>

//using boost::dynamic_bitset;
using std::cerr;
using std::endl;

namespace Encoding
{
	
inline unsigned int bitsOf(unsigned long n)
{
	unsigned int bits = 0;
	for(; n; n >>= 1)
		bits++;
		
	return bits;
}

inline void encode(ZCom_BitStream& stream, int i, int count)
{
	assert(count > 0);
	assert(i >= 0 && i < count);
	stream.addInt(i, bitsOf(count - 1));
}

inline int decode(ZCom_BitStream& stream, int count)
{
	assert(count > 0);
	return stream.getInt(bitsOf(count - 1));
}

struct VectorEncoding
{
	VectorEncoding();
	
	VectorEncoding(Rect area_, unsigned int subPixelAcc_ = 1);
	
	template<class T>
	std::pair<long, long> quantize(T const& v)
	{
		long y = static_cast<long>((v.y - area.y1) * subPixelAcc + 0.5);
		if(y < 0)
			y = 0;
		else if(y > height)
			y = height - 1;
			
		long x = static_cast<long>((v.x - area.x1) * subPixelAcc + 0.5);
		if(x < 0)
			x = 0;
		else if(x > width)
			x = width - 1;
			
		return std::make_pair(x, y);
	}
	
	template<class T>
	void encode(ZCom_BitStream& stream, T const& v)
	{
		long y = static_cast<long>((v.y - area.y1) * subPixelAcc + 0.5);
		if(y < 0)
			y = 0;
		else if(y > height)
			y = height - 1;
			
		long x = static_cast<long>((v.x - area.x1) * subPixelAcc + 0.5);
		if(x < 0)
			x = 0;
		else if(x > width)
			x = width - 1;
			
		stream.addInt(x, bitsX);
		stream.addInt(y, bitsY);
	}
	
	template<class T>
	T decode(ZCom_BitStream& stream)
	{
		typedef typename T::manip_t manip_t;
		
		long x = stream.getInt(bitsX);
		long y = stream.getInt(bitsY);
		
		return T(manip_t(x) / subPixelAcc + area.x1, manip_t(y) / subPixelAcc + area.y1);
	}
	
	unsigned long totalBits()
	{
		return bitsX + bitsY;
	}
	
	Rect area;
	unsigned long total;
	unsigned long width;
	unsigned long height;
	unsigned long bitsX;
	unsigned long bitsY;
	
	unsigned int subPixelAcc;
	double isubPixelAcc;
};

/*
void writeBitset(ZCom_BitStream& stream, dynamic_bitset<> b);

dynamic_bitset<> readBitset(ZCom_BitStream& stream, zU8 bits);
*/

}

#endif //GUSANOS_ENCODING_H
