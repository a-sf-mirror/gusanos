#include "encoding.h"
#include <iostream>
using std::cerr;
using std::endl;

namespace Encoding
{
	
unsigned int eliasCodedBits = 0;
unsigned int eliasInvokations = 0;
	
VectorEncoding::VectorEncoding()
: area(0, 0, 0, 0), total(0), width(0), height(0), bitsX(0), bitsY(0), subPixelAcc(1), isubPixelAcc(0.0)
{
}

VectorEncoding::VectorEncoding(Rect area_, unsigned int subPixelAcc_)
: area(area_), subPixelAcc(subPixelAcc_), isubPixelAcc(1.0 / subPixelAcc_)
{
	width = area.getWidth() * subPixelAcc;
	height = area.getHeight() * subPixelAcc;
	total = width * height;
	bitsX = bitsOf(width - 1);
	bitsY = bitsOf(height - 1);

}

DiffVectorEncoding::DiffVectorEncoding(unsigned int subPixelAcc_)
: subPixelAcc(subPixelAcc_)
{

}

/*
void writeBitset(ZCom_BitStream& stream, dynamic_bitset<> b)
{
	stream.addInt(b.to_ulong(), b.size());
}

dynamic_bitset<> readBitset(ZCom_BitStream& stream, zU8 bits)
{
	return dynamic_bitset<>((dynamic_bitset<>::size_type)bits, stream.getInt(bits));
}
*/
}
