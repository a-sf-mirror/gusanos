#ifndef text_h
#define text_h

#include <sstream>
#include <list>
#include <string>

template<typename T1, typename T2> 
T1 cast(const T2 &t2)
{
	std::stringstream strStream;
	
	strStream << t2;
	T1 t1;
	strStream >> t1;
	return t1;
}

#endif  // _text_h_
