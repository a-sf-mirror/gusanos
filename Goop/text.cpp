#include "text.h"
#include <list>


using namespace std;


/********************** separate_str_by ****************************************** 
*  separates the string in 2 chunks the first chunk is from index 0 to the first *
*  occurence of the given char, the second chunk is the rest of the src string   *
*********************************************************************************/
// should be changed to std::pair<string,string> acording to Gliptitc
void separate_str_by(char ch, const string &src, string &left, string &right)
{
	int leftIndex = 0;
	int rightIndex = 0;
	
	if (!src.empty())
	{
		leftIndex = src.find_first_not_of(ch);
		if (leftIndex != string::npos)
		{
			rightIndex = src.find_first_of(ch, leftIndex);
			if (rightIndex != string::npos)
			{
				left = src.substr(leftIndex, rightIndex - leftIndex);
				
				leftIndex = src.find_first_not_of(ch,rightIndex);
				if (leftIndex != string::npos)
				{
					right = src.substr(leftIndex);
				}
			}else
			{
				left = src.substr(leftIndex);
				right.clear();
			}
		}
	}
}

list<string> tokenize(const string &text)
{
	int left = 0;
	int right = 0;

	char lastChar = ' ';
	
	list<string> stringList;
	
	while (right != string::npos)
	{
		left = text.find_first_not_of(lastChar, right);
		
		if (left != string::npos)
		{
			if ( text[left] == '"' )
			{
				left++;
				right = text.find_first_of('"',left);
			}else
				right = text.find_first_of("; ",left);
			if (right != string::npos)
			{
				lastChar = text[right];
				
				if (right > left)
				stringList.push_back( text.substr(left, right - left) );
				
				if ( lastChar == ';' )
					stringList.push_back( ";" );
			}else
			stringList.push_back( text.substr(left) );
		}
		else
			right = string::npos;
	}
	
	return stringList;
}


list< list<string> > text2Tree(const string &text)
{
	int left = 0;
	int right = 0;
	
	list< list<string> > argTree;
	
	list<string> stringList = tokenize(text);
	
	if (!stringList.empty())
	{
		argTree.push_back();
		list<string>::iterator tokensIter = stringList.begin();
		while (tokensIter != stringList.end())
		{
			if ( (*tokensIter) == ";" )
			{
				list<string> tmplist;
				argTree.push_back(tmplist);
			}else
			{
				list< list<string> >::iterator iter = argTree.end();
				iter--;
				(*iter).push_back( (*tokensIter) );
			}
			tokensIter++;
		}
	}
	return argTree;
}



