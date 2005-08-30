#ifndef TEXT_H
#define TEXT_H

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

void separate_str_by(char ch, const std::string &src, std::string &left, std::string &right);

std::list< std::list<std::string> > text2Tree(const std::string &text);

inline bool iStrCmp( const std::string &a, const std::string &b )
{
	std::string::const_iterator itA, itB;
	
	for (itA = a.begin(), itB = b.begin();
		    itA != a.end() && itB != b.end();
		    ++itA, ++itB)
	{
		char ca = std::toupper(*itA);
		char cb = std::toupper(*itB);
		if(ca != cb)
			return false;
	}
	return true;
}

struct IStrCompare
{
	template<class StringT>
	bool operator()(StringT const& a, StringT const& b) 
	{
	    typename StringT::const_iterator itA, itB;
	    
	    for (itA = a.begin(), itB = b.begin();
	         itA != a.end() && itB != b.end();
	         ++itA, ++itB)
	    {
	    	char ca = std::toupper(*itA);
	    	char cb = std::toupper(*itB);
	        if(ca != cb)
	            return ca < cb;
	    }
	    
	    return a.size() < b.size();
	}
};

template<class InputT>
bool portable_getline(InputT& str, std::string& s)
{
	std::string l;
	
	for(int len = 0; ; ++len)
	{
		int c = str.get();
		if(c == InputT::traits_type::eof())
		{
			if(len == 0)
				return false;
			s = l;
			return true;
		}
		
		if(c == '\r' || c == '\n')
		{
			if(len == 0) // Skip zero length lines
			{
				--len;
			}
			else
			{
				s = l;
				return true;
			}
		}
		else
			l += (char)c;
	}
}

#endif  // _TEXT_H_
