#ifndef text_h
#define text_h

#include <sstream>
//#include <string>

template<typename T1, typename T2> 
T1 cast(const T2 &t2)
{
	std::stringstream strStream;
	
	strStream << t2;
	T1 t1;
	strStream >> t1;
	return t1;
}

void separate_str_by(char ch, const std::string &src, std::string &left, std::string &right);

#endif  // _text_h_
