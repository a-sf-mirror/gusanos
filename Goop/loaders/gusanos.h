#ifndef GUSANOS_LOADERS_GUSANOS_H
#define GUSANOS_LOADERS_GUSANOS_H

#include "../resource_locator.h"
#include "../level.h"

struct GusanosLevelLoader : ResourceLocator<Level>::BaseLoader
{
	virtual bool canLoad(fs::path const& path, std::string& name);
	
	virtual bool load(Level*, fs::path const& path);
	
	virtual const char* getName();
	
	static GusanosLevelLoader instance;
};

#endif //GUSANOS_LOADERS_GUSANOS_H
