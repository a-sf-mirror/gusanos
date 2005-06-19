#ifndef LEVEL_H
#define LEVEL_H

#include "material.h"
#include "resource_locator.h"
#include "vec.h"

#include <allegro.h>
#include <string>
#include <vector>

class Level
{
	public:
		
	Level();
	~Level();
	
	bool load(const std::string &name);
	bool loadLiero(const std::string &name);
	void unload();
	bool isLoaded();
	void draw(BITMAP* where, int x, int y);
	int width();
	int height();
	
	const std::string &getPath();
	const std::string &getName();
	void setName(const std::string &_name);
	/*
	const Material& getMaterial(int x, int y);*/
	
	Vec getSpawnLocation();
	
	Material const& getMaterial(unsigned int x, unsigned int y)
	{
		if(x < material->w && y < material->h)
			return m_materialList[material->line[y][x] + 1];
		else
			return m_materialList[0];
	}
	
	Material const& unsafeGetMaterial(unsigned int x, unsigned int y)
	{
		return m_materialList[material->line[y][x] + 1];
	}
	
	//private:
		
	bool loaded;
	
	BITMAP* image;
	BITMAP* material;
	BITMAP* background;
	std::string name;
	std::string path;
	std::vector<Material> m_materialList;
	
};

extern ResourceLocator<Level> levelLocator;

#endif // _LEVEL_H_
