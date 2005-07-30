#ifndef RESOURCE_LIST_H
#define RESOURCE_LIST_H

#include "game.h"

#include <map>
#include <string>


template<typename T1>
class ResourceList
{
	public:

	ResourceList( const std::string& subFolder)
	{
		m_subFolder = subFolder;
	};
	
	void clear()
	{
		typename std::map<std::string, T1*>::iterator item = m_resItems.begin();
		while (item != m_resItems.end())
		{
			delete item->second;
			++item;
		}
		m_resItems.clear();
	};
		
	T1* load( std::string filename )
	{
		typename std::map<std::string, T1*>::iterator item = m_resItems.find(filename);
		if (item != m_resItems.end())
		{
			return item->second;
		}
		else
		{
			T1 *i = new T1;
			m_resItems[filename] = i;

			if ( game.specialLoad(m_subFolder + filename, *i) )
			{
				return i;
			}
			else
			{
				delete i;
				item = m_resItems.find(filename);
				m_resItems.erase(item);
				return NULL;
			}
		}
	};
	
	private:
	
	std::string m_subFolder;
	std::map<std::string, T1*> m_resItems;
};

#endif // _RESOURCE_LIST_H_
