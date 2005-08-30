#include "sprite_set.h"

#include "resource_list.h"
#include "gfx.h"
#include "sprite.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <iostream> //TEMP
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

using namespace std;

ResourceList<SpriteSet> spriteList("sprites/");

SpriteSet::SpriteSet()
{
	
}

SpriteSet::~SpriteSet()
{
	vector< vector<Sprite*> >::iterator frameY;
	vector< Sprite* >::iterator frameX;
	for (frameY = m_frame.begin(); frameY != m_frame.end(); frameY++)
		for (frameX = (*frameY).begin(); frameX != (*frameY).end(); frameX++)
	{
		if (*frameX) delete *frameX;
	}
}

bool SpriteSet::load(fs::path const& filename)
{	
	bool returnValue = false;
	
	//cerr << "Loading sprite set: " << filename.native_file_string() << endl;
	
	BITMAP *tempBitmap = gfx.loadBitmap(filename.native_file_string().c_str(),0);
	
	if (tempBitmap)
	{
		if ( (tempBitmap->w > 1) && (tempBitmap->h > 1) )
		{
			int lastY = 1;
			int pivotY = -1;
			
			for (int y = 1; y < tempBitmap->h; ++y)
			{
				
				if( getpixel(tempBitmap,0,y) == makecol(255,0,0) ) // Red pixel marks the pivot of the sprite
				{
					pivotY = y-lastY;
				}
				else if( getpixel(tempBitmap,0,y) == 0 || y == tempBitmap->h - 1 )
				{
					m_frame.push_back( vector<Sprite*>() );

					int lastX = 1;
					int pivotX = -1;
					
					for (int x = 1; x < tempBitmap->w; ++x)
					{
						if( getpixel(tempBitmap,x,0) == makecol(255,0,0) ) // Pivot again but for X axis
						{
							pivotX = x-lastX;
						}
						else if(getpixel(tempBitmap,x,0) == 0 || x == tempBitmap->w - 1 )
						{
							BITMAP* spriteFrame = create_bitmap(x-lastX+1,y-lastY+1);
							blit(tempBitmap,spriteFrame,lastX,lastY,0,0,spriteFrame->w,spriteFrame->h);
							m_frame.back().push_back(new Sprite( spriteFrame,pivotX,pivotY ) );
							
							pivotX = -1;
							
							lastX = x + 1;
						}
					}

					pivotY = -1;
					lastY = y + 1;
				}
			}
			
			// Fill the other 180º with the sprites but mirrored.
			
			
			
			returnValue = true;
		}
		destroy_bitmap(tempBitmap);
	}
	return returnValue;
}

Sprite* SpriteSet::getSprite( int frame )
{
	if ( frame > static_cast<int>( m_frame[0].size() ) )
	{
		frame = 0;
	}
	return m_frame[0][frame];
}

Sprite* SpriteSet::getSprite( int frame, float angle )
{
	if ( frame > static_cast<int>(m_frame[0].size()) )
	{
		frame = 0;
	}
	float angleDivisionSize = 180 / m_frame.size();
	size_t angleFrame = static_cast<size_t>((angle + angleDivisionSize / 2) * (m_frame.size()-1) / 180);
	if ( angleFrame > m_frame.size() )
	{
		angleFrame = m_frame.size()-1;
	}
	return m_frame[angleFrame][frame];
}

