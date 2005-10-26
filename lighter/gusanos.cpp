#include <allegro.h>

#include "level.h"
#include "loaders/gusanos.h"
#include "omfgutil_macros.h"
#include "omfgutil_math.h"
#include "omfgutil_text.h"
#include "gfx.h"

#ifdef WINDOWS
	#include <winalleg.h>
#endif

#include <boost/progress.hpp>

#include <string>
#include <vector>
#include <list>

#ifdef POSIX
#include <unistd.h>
#endif

using namespace std;


int main(int argc, char **argv)
{

	int iters = 20;

	for(int i = 0; i < argc; ++i)
	{
		const char* arg = argv[i];
		if(arg[0] == '-')
		{
			switch(arg[1])
			{
				case 'i':
					if(++i >= argc)
						break;
						
					iters = cast<int>(argv[i]);
				break;
			}
		}
	}
	allegro_init();
	install_keyboard();
	
	gfx.init();
	
	//set_gfx_mode (GFX_AUTODETECT, 320, 240, 0, 0);
	
	Level level;
	
	BITMAP* lightmap;
	
	GusanosLevelLoader::instance.load( &level, "./" );
	if ( level.isLoaded() )
	{
		vector<IVec> lightSources;
	
		BITMAP* lightsource = gfx.loadBitmap("lightsource", 0);
		
		for ( int x = 0; x < lightsource->w ; ++x )
		for ( int y = 0; y < lightsource->h ; ++y )
		{
			if ( getpixel( lightsource, x,y ) != 0 )
			{
				lightSources.push_back(IVec(x,y));
			}
		}
	
		Level::ParticleBlockPredicate pred;
		
		lightmap = create_bitmap_ex(24,level.material->w, level.material->h);
		
		float xCoord;
		float yCoord;
		
		boost::progress_display show_progress( lightmap->h );
		
		for ( int y = 0; y < lightmap->h; ++y )
		{
			for ( int x = 0; x < lightmap->w; ++x )
			{
				int color = 0;
				for ( int n = 0; n < lightSources.size() ; ++n )
				{
					if ( !level.preciseTrace( lightSources[n].x+0.5f, lightSources[n].x + 0.5f, x, y, pred ) )
					{
						float fade = 1;
						color += 255 * fade;
					}
				}
				/*for ( int n = 0; n < iters; ++n )
				{
					xCoord = 100.f + ( n / 2.f );
					yCoord = 121.f;
					float fade;
				
					if ( !level.preciseTrace( xCoord, yCoord, x, y, pred ) )
					{
						float distance = Vec( x-xCoord, y-yCoord ).length();
						if ( distance == 0 ) fade = 1;
						else fade = 1;
						
						if ( fade > 1 ) fade = 1;
						
						color += 255 * fade;
					}
				}*/
				color /= lightSources.size();
				if ( color > 255 ) color = 255;
				putpixel( lightmap,x,y,makecol(color,color,color) );
			}
			++show_progress;
		}
	}

	bool success = gfx.saveBitmap( "newLightmap.png",lightmap,0);
	
	allegro_message("done");

	allegro_exit();

	return(0);
}
END_OF_MAIN();

