#ifndef TEXT_H
#define TEXT_H

#include <sstream>
#include <list>
#include <set>
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

inline bool istrCmp( const std::string &a, const std::string &b )
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
	
	return (itA == a.end() && itB == b.end());
}

inline bool iisPrefixOfOther(
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

	//typename ContainerT::const_iterator item = );
	typename ContainerT::const_iterator i = items.begin();
	
	typedef std::set<std::string, IStrCompare> SortedListT;
	SortedListT sortedList;
	
	std::string s(b, e);
	
	for(; i != items.end(); ++i)
		sortedList.insert(getText(i));
		
	typename SortedListT::const_iterator item = sortedList.lower_bound( s );
	
	if( item == sortedList.end() )
		return s;
		
	if ( ! iisPrefixOfOther( b, e, item->begin(), item->end()) )
		return s;

	typename SortedListT::const_iterator firstMatch = item;
	typename SortedListT::const_iterator lastMatch;
		
	while ( item != sortedList.end() && iisPrefixOfOther( b, e, item->begin(), item->end()) )
	{
		lastMatch = item;
		item++;
	}
	
	if (lastMatch == firstMatch)
	{
		return *firstMatch + ' ';
	}
	else
	{
		lastMatch++;
		bool differenceFound = false;
		size_t i = len;
		for(; ; ++i)
		{
			for (item = firstMatch; item != lastMatch; ++item)
			{
				if(item->size() <= i
				|| tolower((*item)[i]) != tolower((*firstMatch)[i]))
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
			showAlternatives(firstMatch, lastMatch);
		}

		return std::string(firstMatch->begin(), firstMatch->begin() + i);
	}

	return s;
}

#endif  // _TEXT_H_
