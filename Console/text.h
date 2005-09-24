#ifndef TEXT_H
#define TEXT_H

#include <sstream>
#include <list>
#include <string>
#include <iostream>

using std::cerr;
using std::endl;

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

inline bool iStrCmp(
	std::string::const_iterator a,
	std::string::const_iterator ae,
	std::string::const_iterator b,
	std::string::const_iterator be
)
{
	for (; a != ae && b != be;
		    ++a, ++b)
	{
		char ca = std::toupper(*a);
		char cb = std::toupper(*b);
		if(ca != cb)
			return false;
	}
	return true;
}

struct IStrCompare
{
	template<class StringT>
	bool operator()(StringT const& a, StringT const& b) const
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

template<class ContainerT, class GetText, class ShowAlternatives>
std::string shellComplete(
	ContainerT const& items,
	std::string::const_iterator b,
	std::string::const_iterator e,
	GetText getText,
	ShowAlternatives showAlternatives
	)
{
	size_t len = e - b;
	
	if ( len == 0 )
		return std::string(b, e);

	typename ContainerT::const_iterator item = items.lower_bound( std::string(b, e) );
	if( item == items.end() )
		return std::string(b, e);
		
	if ( ! iStrCmp( b, e, getText(item).begin(), getText(item).begin() + len) )
		return std::string(b, e);

	typename ContainerT::const_iterator firstMatch = item;
	typename ContainerT::const_iterator lastMatch;
		
	while ( item != items.end() && iStrCmp( b, e, getText(item).begin(), getText(item).begin() + len) )
	{
		lastMatch = item;
		item++;
	}
	
	if (lastMatch == firstMatch)
	{
		return getText(firstMatch) + ' ';
	}
	else
	{
		lastMatch++;
		bool differenceFound = false;
		int i = len;
		for(; ; ++i)
		{
			for (item = firstMatch; item != lastMatch; ++item)
			{
				if(getText(item).size() <= i
				|| tolower(getText(item)[i]) != tolower(getText(firstMatch)[i]))
				{
					differenceFound = true;
					break;
				}
			}
			
			if(differenceFound)
				break;
		}
		
		if(i == len)
		{
			showAlternatives(firstMatch, lastMatch, getText);
		}

		return std::string(getText(firstMatch).begin(), getText(firstMatch).begin() + i);
	}

	return std::string(b, e);
}

#endif  // _TEXT_H_
