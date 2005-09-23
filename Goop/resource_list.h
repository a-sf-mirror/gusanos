#ifndef RESOURCE_LIST_H
#define RESOURCE_LIST_H

#include <map>
#include <vector>
#include <string>
#include <list>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

template<typename T1>
class ResourceList
{
public:

	ResourceList( fs::path const& subFolder)
	{
		m_subFolder = subFolder;
		m_locked = false;
	}
	
	void clear()
	{
		m_paths.clear();
		typename std::map<fs::path, T1*>::iterator item = m_resItems.begin();
		while (item != m_resItems.end())
		{
			delete item->second;
			++item;
		}
		m_resItems.clear();
		m_locked = false;
		m_resItemsIndex.clear();
	}
	
	void addPath(fs::path const& path)
	{
		if(std::find(m_paths.begin(), m_paths.end(), path) == m_paths.end())
			m_paths.push_back(path);
	}
	
	bool load(fs::path const& name, T1& resource)
	{
		std::list<fs::path>::iterator i = m_paths.begin();
		for(; i != m_paths.end(); ++i)
		{
			if(resource.load(*i / name))
				return true;
		}
		return false;
	}
		
	T1* load( fs::path const& filename )
	{
		if ( m_locked )
			return NULL;

		typename std::map<fs::path, T1*>::iterator item = m_resItems.find(filename);
		if (item != m_resItems.end())
		{
			return item->second;
		}
		else
		{
			T1 *i = new T1;
			m_resItems[filename] = i;

			if(load(filename, *i))
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
	}
	
	void indexate()
	{
		m_locked = true;
		typename std::map<fs::path, T1*>::iterator item = m_resItems.begin();
		for( size_t i = 0; item != m_resItems.end() ; ++item, ++i )
		{
			m_resItemsIndex.push_back( item->second );
			item->second->setIndex(i);
		}
	}
	
	size_t size()
	{
		return m_resItemsIndex.size();
	}
	
	T1* operator[]( size_t index ) const
	{
		if( index < m_resItemsIndex.size() )
		{
			return m_resItemsIndex[index];
		}else
			return NULL;
	}
	
private:
	
	bool m_locked;
	fs::path m_subFolder;
	std::vector<T1*> m_resItemsIndex;
	std::map<fs::path, T1*> m_resItems;
	std::list<fs::path>     m_paths; // Paths to scan
};

#endif // _RESOURCE_LIST_H_
