#ifndef GUSANOS_LOADERS_LOSP_H
#define GUSANOS_LOADERS_LOSP_H

#include "../resource_locator.h"
#include "../font.h"


		
struct LOSPFontLoader : ResourceLocator<Font>::BaseLoader
{
	virtual bool canLoad(fs::path const& path, std::string& name);
	
	virtual bool load(Font*, fs::path const& path);
	
	virtual const char* getName();
	
	static LOSPFontLoader instance;
};

#endif //GUSANOS_LOADERS_LOSP_H