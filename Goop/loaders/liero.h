#ifndef GUSANOS_LOADERS_LIERO_H
#define GUSANOS_LOADERS_LIERO_H

#include "../resource_locator.h"
#include "../level.h"

struct LieroLevelLoader : ResourceLocator<Level>::BaseLoader
{
	virtual bool canLoad(fs::path const& path, std::string& name);
	
	virtual bool load(Level*, fs::path const& path);
	
	virtual const char* getName();
	
	static LieroLevelLoader instance;
};

#endif //GUSANOS_LOADERS_LIERO_H
