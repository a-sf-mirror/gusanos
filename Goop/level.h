#ifndef LEVEL_H
#define LEVEL_H

#include "material.h"

#include <allegro.h>
#include <string>
#include <vector>

class Level
{
	public:
		
	Level();
	~Level();
	
	bool load(const std::string &name);
	void unload();
	void draw(BITMAP* where, int x, int y);
	int width();
	int height();
	
	const Material& getMaterial(int x, int y);
	
	private:
	
	BITMAP* image;
	BITMAP* material;
	BITMAP* background;
	std::string name;
	std::string path;
	std::vector<Material> m_materialList;
	
};

#endif // _LEVEL_H_
