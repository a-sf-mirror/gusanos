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
	bool isLoaded();
	void draw(BITMAP* where, int x, int y);
	int width();
	int height();
	
	const std::string &getPath();
	
	const Material& getMaterial(int x, int y);
	/*inline const Material& getMaterial(int x, int y);
	{
		return m_materialList[getpixel(material,x,y)+1];
	}*/
	
	private:
		
	bool loaded;
	
	BITMAP* image;
	BITMAP* material;
	BITMAP* background;
	std::string name;
	std::string path;
	std::vector<Material> m_materialList;
	
};

#endif // _LEVEL_H_
