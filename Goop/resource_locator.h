#ifndef GUSANOS_RESOURCE_LOCATOR_H
#define GUSANOS_RESOURCE_LOCATOR_H

#include <map>
#include <string>
#include <list>
#include <set>
#include <iostream>
#include <console.h> //For IStrCompare
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
namespace fs = boost::filesystem;

template<class T>
struct ResourceLocator
{
	ResourceLocator()
	{
	}
	
	struct BaseLoader
	{
		// Should return true and set name to the resource name 
		// if the file/folder specified by path can be loaded.
		// Otherwise, it should return false.
		virtual bool canLoad(fs::path const& path, std::string& name) = 0;
		
		// Should load the resource located at path
		virtual bool load(T*, fs::path const& path) = 0;
		
		virtual const char* getName() = 0;
		
		virtual ~BaseLoader() { }
	};
	
	// Information about a resource found via refresh()
	struct ResourceInfo
	{
		ResourceInfo() {}
		
		ResourceInfo(fs::path const& path_, BaseLoader* loader_)
		: path(path_), loader(loader_)
		{
		}
		
		fs::path path; // Path to load from
		BaseLoader* loader;   // Loader to use
	};
	
	// Refreshes the internal resource list by
	// scanning the path list after resources that can
	// be loaded by any of the registered loaders.
	// Folders are tried as well. If a loader is found for
	// a folder, all the subfolders/files are ignored.
	void refresh();
	
	// Clears the path list
	void clearPaths()
	{
		m_paths.clear();
	}
	
	// Adds a path to the path list
	void addPath(fs::path const& path)
	{
		m_paths.insert(path);
	}
	
	// Loads the named resource
	bool load(T*, std::string const& name);

	// Registers a new loader for this resource type
	void registerLoader(BaseLoader* loader)
	{
		m_loaders.push_back(loader);
	}
	
private:
	void refresh(fs::path const& path);
	
	typedef typename std::map<std::string, ResourceInfo, IStrCompare> NamedResourceMap;
	
	NamedResourceMap m_namedResources; //The resource list
	
	std::list<BaseLoader *> m_loaders; // Registered loaders
	std::set<fs::path>      m_paths; // Paths to scan
};

template<class T>
void ResourceLocator<T>::refresh(fs::path const& path)
{
	std::cout << "Scanning: " << path.native_file_string() << std::endl;
	try
	{
		fs::directory_iterator i(path), e;
		
		for(; i != e; ++i)
		{
			std::string name;
			BaseLoader* loader = 0;
			
			// Try loaders until a working one is found
			
			for(typename std::list<BaseLoader *>::iterator l = m_loaders.begin();
			    l != m_loaders.end();
			    ++l)
			{
				if((*l)->canLoad(*i, name))
				{
					loader = *l;
					break;
				}
			}
			
			if(loader)
			{
				// We found a loader
				std::cout << "Found resource: " << name << ", loader: " << loader->getName() << std::endl;
				m_namedResources[name] = ResourceInfo(*i, loader);
			}
			else if(fs::is_directory(*i))
			{
				// If no loader was found and this is a directory, scan it
				refresh(*i);
			}
		}
	}
	catch(fs::filesystem_error& err)
	{
		std::cout << err.what() << std::endl;
	}
}

template<class T>
void ResourceLocator<T>::refresh()
{
	m_namedResources.clear(); // Remove all old resources
	
	for(std::set<fs::path>::const_iterator p = m_paths.begin();
	    p != m_paths.end();
	    ++p)
	{
		refresh(*p);
	}
}

template<class T>
bool ResourceLocator<T>::load(T* dest, std::string const& name)
{
	typename NamedResourceMap::iterator i = m_namedResources.find(name);
	if(i == m_namedResources.end())
		return false;

	return i->second.loader->load(dest, i->second.path);
}

#endif //GUSANOS_RESOURCE_LOCATOR_H
