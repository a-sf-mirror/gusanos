#ifndef RESOURCE_LIST_H
#define RESOURCE_LIST_H

#include <map>
#include <vector>
#include <string>
#include <list>
#include <iostream>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

using std::cerr;
using std::endl;

template<typename T1>
class ResourceList
{
public:

	typedef std::map<fs::path, T1*> MapT;
	
	ResourceList()
	: m_locked(false)
	{
		
	}
	
	void clear()
	{
		m_paths.clear();
		typename MapT::iterator item = m_resItems.begin();
		for (; item != m_resItems.end(); ++item)
		{
			delete item->second;
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
		
	T1* load( fs::path const& filename, bool suppressError = false )
	{
		if ( m_locked )
			return NULL;

		typename MapT::iterator item = m_resItems.find(filename);
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
				if(!suppressError)
					cerr << "ERROR: Could not load " << filename.native_file_string() << endl;
				return NULL;
			}
		}
	}
	
	void think()
	{
		typename MapT::iterator i = m_resItems.begin();
		for (; i != m_resItems.end(); ++i)
		{
			i->second->think();
		}
	}
	
	void indexate()
	{
		m_locked = true;
		typename MapT::iterator item = m_resItems.begin();
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
		}
		else
			return 0;
	}
	
private:
	
	bool m_locked;
	std::vector<T1*> m_resItemsIndex;
	MapT m_resItems;
	std::list<fs::path>     m_paths; // Paths to scan
};

#endif // _RESOURCE_LIST_H_
