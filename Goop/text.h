#ifndef text_h
#define text_h

#include <sstream>
//#include <string>

using namespace std;

template<typename T1, typename T2> 
T1 cast(const T2 &t2)
{
	stringstream strStream;
	
	strStream << t2;
	T1 t1;
	strStream >> t1;
	return t1;
}

void separate_by(char ch, string &left, string &right);

#endif  // _text_h_
