#ifndef GUSANOS_LOADERS_GUSANOS_H
#define GUSANOS_LOADERS_GUSANOS_H

#include "../resource_locator.h"
#include "../level.h"
#include "../font.h"
#include "../menu.h"

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

struct XMLLoader : ResourceLocator<XMLFile, false, false>::BaseLoader
{
	virtual bool canLoad(fs::path const& path, std::string& name);
	
	virtual bool load(XMLFile*, fs::path const& path);
	
	virtual const char* getName();
	
	static XMLLoader instance;
};

struct GSSLoader : ResourceLocator<GSSFile, false, false>::BaseLoader
{
	virtual bool canLoad(fs::path const& path, std::string& name);
	
	virtual bool load(GSSFile*, fs::path const& path);
	
	virtual const char* getName();
	
	static GSSLoader instance;
};

#endif //GUSANOS_LOADERS_GUSANOS_H
