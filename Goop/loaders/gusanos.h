#ifndef GUSANOS_LOADERS_GUSANOS_H
#define GUSANOS_LOADERS_GUSANOS_H

#include "../resource_locator.h"
#include "../level.h"
#include "../font.h"

struct GusanosLevelLoader : ResourceLocator<Level>::BaseLoader
{
	virtual bool canLoad(fs::path const& path, std::string& name);
	
	virtual bool load(Level*, fs::path const& path);
	
	virtual const char* getName();
	
	static GusanosLevelLoader instance;
};

struct GusanosFontLoader : ResourceLocator<Font>::BaseLoader
{
	virtual bool canLoad(fs::path const& path, std::string& name);
	
	virtual bool load(Font*, fs::path const& path);
	
	virtual const char* getName();
	
	static GusanosFontLoader instance;
};

#endif //GUSANOS_LOADERS_GUSANOS_H
