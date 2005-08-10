#include "lierox.h"
#include "../gfx.h"
#include "zlib.h"
#include <string>
#include <cstring>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;

LieroXLevelLoader LieroXLevelLoader::instance;

bool LieroXLevelLoader::canLoad(fs::path const& path, std::string& name)
{
	if(fs::extension(path) == ".lxl")
	{
		name = basename(path);
		return true;
	}
	return false;
}

bool LieroXLevelLoader::load(Level* level, fs::path const& path)
{
	fs::ifstream f(path, std::ios::binary);
	if(!f)
		return false;
		
	static const char magic[] = "LieroX Level";
	char check[sizeof(magic)];
	f.read(check, sizeof(check));
	if(memcmp(check, magic, sizeof(magic)))
		return false; //File magic check failed
		
	long width = 0, height = 0; //long has to be at least 32-bit, static_assert?
	f.seekg(0x64); //Level size position
	f.read((char *)&width, 4);
	f.read((char *)&height, 4);
	
	//TODO: Read level name
	
	f.seekg(0x9c); //Level data position
	
	const size_t ChunkBits = 14;
	const size_t ChunkSize = 1 << ChunkBits;
	
	size_t have;
	z_stream strm;
	unsigned char in[ChunkSize];

	// allocate inflate state
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	int ret = inflateInit(&strm);
	if (ret != Z_OK)
		return false;

	size_t totalDataSize = (2 * 3 + 1) * (width * height);
	
	// Add an extra chunk size to be sure not to overflow
	totalDataSize = totalDataSize + ChunkSize;

	std::vector<unsigned char> data(totalDataSize);
	unsigned char *p = &data[0];
	
	do
	{
		f.read((char *)in, ChunkSize);
		strm.avail_in = f.gcount();

		if (strm.avail_in == 0)
		    break;
		strm.next_in = (Bytef *)in;
		
		// run inflate() on input until output buffer not full
		do
		{
			if(totalDataSize < ChunkSize)
				return false; // We got too much data from the stream
			strm.avail_out = ChunkSize;
		    strm.next_out = (Bytef *)p;
		    ret = inflate(&strm, Z_NO_FLUSH);

		    switch (ret)
		    {
				case Z_NEED_DICT:
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
				case Z_STREAM_ERROR:
					inflateEnd(&strm);
					return false;
		    }
		    
		    have = ChunkSize - strm.avail_out;
		    totalDataSize -= have;
		    p += have;
		} while (strm.avail_out == 0);

	} while (ret != Z_STREAM_END);
	
	inflateEnd(&strm); // Clean up
		
	int vdepth = get_color_depth();
	
	set_color_depth(8);
	level->material = create_bitmap_ex(8, width, height);
	set_color_depth(vdepth);
	level->image = create_bitmap_ex(vdepth, width, height);
	level->background = create_bitmap_ex(vdepth, width, height);

	unsigned char *pbackground = &data[0];
	unsigned char *pimage = pbackground + width*height*3;
	unsigned char *pmaterial = pimage + width*height*3;
	
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			
			int backgroundc = makecol(pbackground[0], pbackground[1], pbackground[2]);
			
			int m = pmaterial[0];

			if(m == 1)
				putpixel(level->image, x, y, backgroundc);
			else
			{
				int imagec = makecol(pimage[0], pimage[1], pimage[2]);
				putpixel(level->image, x, y, imagec);
			}
				
			putpixel(level->background, x, y, backgroundc);
			putpixel(level->material, x, y, m);
			
			pimage += 3;
			pbackground += 3;
			++pmaterial;
		}
	}
	
	//TODO
	level->m_materialList[2].worm_pass = true;
	level->m_materialList[2].particle_pass = true;
	
	level->m_materialList[3].worm_pass = false;
	level->m_materialList[3].particle_pass = false;
	
	level->m_materialList[5].worm_pass = false;
	level->m_materialList[5].particle_pass = false;
	
	level->loaderSucceeded();
	return true;
}

const char* LieroXLevelLoader::getName()
{
	return "LieroX level loader";
}
