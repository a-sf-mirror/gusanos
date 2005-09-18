#include "gfx.h"
#include "text.h"
#include "gconsole.h"
#include "blitters/blitters.h"
#include <boost/assign/list_inserter.hpp>
using namespace boost::assign;

#include <allegro.h>

#include <loadpng.h>

#include <string>
#include <algorithm>
#include <list>

using namespace std;

Gfx gfx;

string fullscreenCmd(const list<string> &args)
{
	return "";
}

void fullscreen( int oldValue )
{
	if(gfx) gfx.fullscreenChange();
}

void doubleRes( int oldValue )
{
	if(gfx) gfx.doubleResChange();
}

Gfx::Gfx()
: buffer(NULL), m_initialized(false), m_fullscreen(true), m_doubleRes(false)
, m_vwidth(320), m_vheight(240), m_bitdepth(32)
{
}

Gfx::~Gfx()
{
}

void Gfx::init()
{
	register_png_file_type();
	
	set_color_depth(m_bitdepth); //Ugh

	doubleResChange(); // This calls fullscreenChange() that sets the gfx mode

	loadpng_init();

	buffer = create_bitmap(320,240);
	
	m_initialized = true; // Tell console commands it's safe to manipulate gfx
}

void Gfx::shutDown()
{
	if ( buffer ) destroy_bitmap(buffer);
}

void Gfx::registerInConsole()
{
	console.registerCommands()
		("SCREENSHOT", screenShot)
	;
	
	console.registerVariables()
		("VID_FULLSCREEN", &m_fullscreen, 0, fullscreen)
		("VID_DOUBLERES", &m_doubleRes, 0, doubleRes)
		("VID_VSYNC", &m_vsync, 0)
		("VID_CLEAR_BUFFER", &m_clearBuffer, 0)
		("VID_BITDEPTH", &m_bitdepth, 32)
		("VID_DISTORTION_AA", &m_distortionAA, 1)
	;
	
	// NOTE: When/if adding a callback to gfx variables, make it do nothing if
	// gfx.operator bool() returns false.
	
	{
		EnumVariable::MapType videoFilters;

		insert(videoFilters) // These neat boost::assign functions actually make it smaller than!
			("NOFILTER", NO_FILTER)
			("SCANLINES", SCANLINES)
			("SCANLINES2", SCANLINES2)
			//("2XSAI", AA2XSAI) // To be included later.
		;

		console.registerVariable(new EnumVariable("VID_FILTER", &m_filter, NO_FILTER, videoFilters));
	}
	
	{ // TODO: Only include drivers relevant to the platform
		EnumVariable::MapType videoDrivers;
		
		insert(videoDrivers)
			("AUTO", GFX_AUTODETECT)
#ifdef WINDOWS
			("DIRECTX", GFX_DIRECTX)
#else //def LINUX   ..or?
			("XDGA", GFX_XDGA)
			("XDGA2", GFX_XDGA2)
			("XWINDOWS", GFX_XWINDOWS)
#endif
		;

		console.registerVariable(new EnumVariable("VID_DRIVER", &m_driver, GFX_AUTODETECT, videoDrivers));
	}
}

void Gfx::updateScreen()
{
	if ( m_vsync ) vsync();
	if ( !m_doubleRes )
	{
		blit(buffer,screen,0,0,0,0,m_vwidth,m_vheight);
	}
	else
	{
		bool blitFromBuffer = true;
		switch ( (Filters)m_filter )
		{
			case NO_FILTER: 
				switch(bitmap_color_depth(screen))
				{
					case 32:

						acquire_screen();

						bmp_select(screen);
						
						for(int y = 0; y < 239; ++y)
						{
							Pixel32* src = (Pixel32 *)buffer->line[y];

							unsigned long dest1 = bmp_write_line(screen, y*2);
							unsigned long dest2 = bmp_write_line(screen, y*2 + 1);
										
							for(int x = 0; x < 319; ++x)
							{
								Pixel p = *src++;
								
								bmp_write32(dest1, p); dest1 += sizeof(Pixel32);
								bmp_write32(dest1, p); dest1 += sizeof(Pixel32);
								bmp_write32(dest2, p); dest2 += sizeof(Pixel32);
								bmp_write32(dest2, p); dest2 += sizeof(Pixel32);
							}
						}
						/*
						unsigned long lastRow[640];
						
						Pixel32* src = (Pixel32 *)buffer->line[0];
						
						unsigned long dest1 = bmp_write_line(screen, 0);
						
						Pixel32 p = *src++;
														
						for(int x = 0; x < 319; ++x)
						{
							Pixel next = *src++;
							
							Pixel ur = Blitters::blendColorsHalfCrude_32(p, next);

							bmp_write32(dest1, p); dest1 += sizeof(Pixel32);
							bmp_write32(dest1, ur); dest1 += sizeof(Pixel32);
							lastRow[x*2] = p;
							lastRow[x*2 + 1] = ur;
							
							p = next;
						}
						
						src = (Pixel32 *)buffer->line[1];
						
						dest1 = bmp_write_line(screen, 1);
						unsigned long dest2 = bmp_write_line(screen, 1);
						
						p = *src++;
														
						for(int x = 0; x < 319; ++x)
						{
							Pixel next = *src++;
							
							Pixel ll = Blitters::blendColorsHalfCrude_32(lastRow[x*2], p);
							Pixel ur = Blitters::blendColorsHalfCrude_32(p, next);
							Pixel lr = Blitters::blendColorsHalfCrude_32(lastRow[x*2 + 1], ur);

							bmp_write32(dest1, ll); dest1 += sizeof(Pixel32);
							bmp_write32(dest1, lr); dest1 += sizeof(Pixel32);
							bmp_write32(dest2, p);  dest2 += sizeof(Pixel32);
							bmp_write32(dest2, ur); dest2 += sizeof(Pixel32);
							lastRow[x*2] = p;
							lastRow[x*2 + 1] = ur;
							
							p = next;
						}

						for(int y = 0; y < 239; ++y)
						{
							Pixel32* src = (Pixel32 *)buffer->line[y];

							unsigned long dest1 = bmp_write_line(screen, y*2);
							unsigned long dest2 = bmp_write_line(screen, y*2 + 1);
							
							p = *src++;
														
							for(int x = 0; x < 319; ++x)
							{
								Pixel next = *src++;
								
								Pixel ll = Blitters::blendColorsHalfCrude_32(lastRow[x*2], p);
								Pixel ur = Blitters::blendColorsHalfCrude_32(p, next);
								Pixel lr = Blitters::blendColorsHalfCrude_32(lastRow[x*2 + 1], ur);
								
	
								bmp_write32(dest1, ll); dest1 += sizeof(Pixel32);
								bmp_write32(dest1, lr); dest1 += sizeof(Pixel32);
								bmp_write32(dest2, p);  dest2 += sizeof(Pixel32);
								bmp_write32(dest2, ur); dest2 += sizeof(Pixel32);
								lastRow[x*2] = p;
								lastRow[x*2 + 1] = ur;
								
								p = next;
							}
						}
						*/
						bmp_unwrite_line(screen);
						
						release_screen();
					break;
					
					case 16:
						acquire_screen();

						bmp_select(screen);

						for(int y = 0; y < 240; ++y)
						{
							// This is done in two loops to avoid shearing
							Pixel16* src = (Pixel16 *)buffer->line[y];
							
							unsigned long dest1 = bmp_write_line(screen, y*2);
														
							for(int x = 0; x < 320; ++x)
							{
								Pixel32 p = (Pixel32)*src++;
								
								p = p | (p << 16);

								bmp_write32(dest1, p); dest1 += sizeof(Pixel32);
							}
							
							src = (Pixel16 *)buffer->line[y];
							
							dest1 = bmp_write_line(screen, y*2 + 1);
							
							for(int x = 0; x < 320; ++x)
							{
								Pixel32 p = (Pixel32)*src++;
								
								p = p | (p << 16);

								bmp_write32(dest1, p); dest1 += sizeof(Pixel32);
							}
						}
						
						bmp_unwrite_line(screen);
						
						release_screen();
					break;
					
					default:
						stretch_blit(buffer, screen, 0, 0, buffer->w, buffer->h, 0, 0, screen->w, screen->h);
					break;
				}

				blitFromBuffer = false;
			break;
			
			case SCANLINES: 
			/*
				acquire_screen();
				for ( int i = 0; i < buffer->h; ++i )
				{
					stretch_blit(buffer, screen, 0, i, buffer->w, 1, 0, i*2, screen->w, 1);
					hline(screen, 0, i*2+1, screen->w, 0);
				}
				release_screen();*/
				switch(bitmap_color_depth(screen))
				{
					case 32:
						acquire_screen();

						bmp_select(screen);

						for(int y = 0; y < 240; ++y)
						{
							unsigned long* src = (unsigned long *)buffer->line[y];

							unsigned long dest1 = bmp_write_line(screen, y*2);
							unsigned long dest2 = bmp_write_line(screen, y*2 + 1);
														
							for(int x = 0; x < 320; ++x)
							{
								unsigned long p = *src++;

								bmp_write32(dest1, p); dest1 += sizeof(unsigned long);
								bmp_write32(dest1, p); dest1 += sizeof(unsigned long);
								bmp_write32(dest2, 0); dest2 += sizeof(unsigned long);
								bmp_write32(dest2, 0); dest2 += sizeof(unsigned long);
							}
						}
						
						bmp_unwrite_line(screen);
						
						release_screen();
					break;
					
					default:
						acquire_screen();
						for ( int i = 0; i < buffer->h; ++i )
						{
							stretch_blit(buffer, screen, 0, i, buffer->w, 1, 0, i*2, screen->w, 1);
							hline(screen, 0, i*2+1, screen->w, 0);
						}
						release_screen();
					break;
				}
				blitFromBuffer = false;
			break;
			
			case SCANLINES2: 
				//blit(buffer, m_doubleResBuffer, 0, 0, 0, 0, buffer->w, buffer->h);
				acquire_screen();
				for ( int i = 0; i < buffer->h; ++i )
				{
					stretch_blit(buffer, screen, 0, i, buffer->w, 1, 0, i*2, screen->w, 1);
					//stretch_blit(m_doubleResBuffer, screen, 0, i+1, buffer->w, 1, 0, i*2+1, screen->w, 1);
				}
				drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
				set_trans_blender(0, 0, 0, 128);
				rectfill( buffer, 0, 0, buffer->w, buffer->h, 0 );
				solid_mode();
				//acquire_screen();
				for ( int i = 0; i < buffer->h; ++i )
				{
					//stretch_blit(buffer, screen, 0, i, buffer->w, 1, 0, i*2, screen->w, 1);
					stretch_blit(buffer, screen, 0, i, buffer->w, 1, 0, i*2+1, screen->w, 1);
				}
				release_screen();
				blitFromBuffer = false;
			break;
		}
		if(blitFromBuffer)
			blit(m_doubleResBuffer, screen, 0, 0, 0, 0, m_vwidth, m_vheight);
	}
	if ( m_clearBuffer ) clear_bitmap(buffer);
}

int Gfx::getGraphicsDriver()
{
	int driverSelected = GFX_AUTODETECT;
	if ( m_fullscreen )
	{
		driverSelected = GFX_AUTODETECT_FULLSCREEN;
		switch ( m_driver )
		{
			case GFX_AUTODETECT: driverSelected = GFX_AUTODETECT_FULLSCREEN; break;
#ifdef WINDOWS
			case GFX_DIRECTX: driverSelected = GFX_DIRECTX; break;
#else //def LINUX   ..or?
			case GFX_XDGA: driverSelected = GFX_XDGA_FULLSCREEN; break;
			case GFX_XDGA2: driverSelected = GFX_XDGA2; break;
			case GFX_XWINDOWS: driverSelected = GFX_XWINDOWS_FULLSCREEN; break;
#endif

		}
	}
	else
	{
		driverSelected = GFX_AUTODETECT_WINDOWED;
		switch ( m_driver )
		{
			case GFX_AUTODETECT: driverSelected = GFX_AUTODETECT_WINDOWED; break;
#ifdef WINDOWS
			case GFX_DIRECTX: driverSelected = GFX_DIRECTX_WIN; break;
#else //ifdef LINUX   ..or?
			case GFX_XDGA: driverSelected = GFX_XDGA; break;
			case GFX_XDGA2: driverSelected = GFX_AUTODETECT_WINDOWED; break; //XDGA2 only works in fullscreen
			case GFX_XWINDOWS: driverSelected = GFX_XWINDOWS; break;
#endif
			// TODO: DirectX overlay support (GFX_DIRECTX_OVL)?
		}
	}
	
	return driverSelected;
}

void Gfx::fullscreenChange()
{
	set_color_depth(m_bitdepth);
	
	// TODO: I suppose that changing graphics driver will clear out bitmaps and such

	int result = set_gfx_mode(getGraphicsDriver(), m_vwidth, m_vheight, 0, 0);
	if(result < 0)
	{
		// TODO: Print error to cerr
		// We hit some error
		if(m_driver != GFX_AUTODETECT)
		{
			// If the driver wasn't at autodetect, revert to it and try again
			m_driver = GFX_AUTODETECT;
			result = set_gfx_mode(getGraphicsDriver(), m_vwidth, m_vheight, 0, 0);
			// TODO: if result returns a negative value, we should close down the shop
		}
	}
	
	if(set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
		set_display_switch_mode(SWITCH_BACKGROUND);
}

void Gfx::doubleResChange()
{
	destroy_bitmap( m_doubleResBuffer );
	if ( m_doubleRes )
	{
		m_vwidth = 640;
		m_vheight = 480;
		m_doubleResBuffer = create_bitmap(m_vwidth, m_vheight );
	}else
	{
		m_vwidth = 320;
		m_vheight = 240;
		m_doubleResBuffer = NULL;
	}
	fullscreenChange();
}

BITMAP* Gfx::loadBitmap( const string& filename, RGB* palette )
{
	BITMAP* returnValue = NULL;
	
#ifndef COLORCONV_KEEP_ALPHA
	const int COLORCONV_KEEP_ALPHA = COLORCONV_EXPAND_256 |
			COLORCONV_15_TO_8 |
			COLORCONV_16_TO_8 |
			COLORCONV_24_TO_8 |
			COLORCONV_32_TO_8 |
			COLORCONV_EXPAND_15_TO_16 |
			COLORCONV_REDUCE_16_TO_15 |
			COLORCONV_EXPAND_HI_TO_TRUE |
			COLORCONV_REDUCE_TRUE_TO_HI |
			COLORCONV_24_EQUALS_32;
#endif
	
	
	int old = get_color_conversion();
	set_color_conversion(COLORCONV_KEEP_ALPHA | COLORCONV_DITHER);
	
	if ( exists( filename.c_str() ) )
	{
		returnValue = load_bitmap(filename.c_str(), palette);
	}else
	{
		string tmp = filename;
		tmp += ".png";
		if ( exists( tmp.c_str() ) )
		{
			returnValue = load_png( tmp.c_str() , palette );
		}
		else
		{
			tmp = filename;
			tmp += ".bmp";
			if ( exists( tmp.c_str() ))
			{
				returnValue = load_bmp( tmp.c_str() , palette );
			}
		}
	}
	
	set_color_conversion(old);
	
	return returnValue;
}

bool Gfx::saveBitmap( const string &filename,BITMAP* image, RGB* palette )
{
	bool returnValue = false;
	
	if ( !save_bitmap(filename.c_str(), image, palette) ) returnValue = true;
	
	return returnValue;
}

string screenShot(const list<string> &args)
{
	int nameIndex = 0;
	
	string filename;
	do
	{
		filename = "screenshots/ss" + cast<string>(nameIndex) + ".png";
		++nameIndex;
	} while( exists( filename.c_str() ) );
	
	BITMAP * tmpbitmap = create_bitmap_ex(16,screen->w,screen->h);
	blit(screen,tmpbitmap,0,0,0,0,screen->w,screen->h);
	bool success = gfx.saveBitmap( filename.c_str(),tmpbitmap,0);
	destroy_bitmap(tmpbitmap);
	
	if ( success )
		return "SCREENSHOT SAVED AS: " + filename;
	else 
		return "UNABLE TO SAVE SCREENSHOT";
}
