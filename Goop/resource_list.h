#ifndef RESOURCE_LIST_H
#define RESOURCE_LIST_H

#include "game.h"

#include <map>
#include <string>


template<typename T1>
class ResourceList
{
	public:
		
	ResourceList()
	{
	};
	
	ResourceList( const std::string& subFolder)
	{
		m_subFolder = subFolder;
	};
	
	~ResourceList()
	{
		typename std::map<std::string, T1*>::iterator item = m_resItems.begin();
		while (item != m_resItems.end())
		{
			delete item->second;
			++item;
		}
	};
		
	T1* load( std::string filename )
	{
		typename std::map<std::string, T1*>::iterator item = m_resItems.find(filename);
		if (item != m_resItems.end())
		{
			return item->second;
		}else
		{
			m_resItems[filename] = new T1;
			item = m_resItems.find(filename);
			if ( gameLoad(m_subFolder + filename,*item->second))
			{
				return item->second;
			}else
			{
				delete item->second;
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