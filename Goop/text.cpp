#include "text.h"

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

