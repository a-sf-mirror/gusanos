#include "distortion.h"

#include <allegro.h>
#include <vector>
#include "vec.h"

#include <string>

using namespace std;

DistortionMap* lensMap(int radius)
{
	DistortionMap* lens = new DistortionMap;

	for ( int y = 0; y < radius*2; ++y )
	for ( int x = 0; x < radius*2; ++x )
	{
		Vec delta = Vec( x - radius, y - radius );
		
		if ( delta.length() <= radius )
		{
			if (delta.length() != 0)
			{
				float newLength = ( radius - sqrt( radius * radius - delta.length() * delta.length() )) - delta.length();
				lens->map.push_back( delta.normal()*newLength );
			}else
				lens->map.push_back( Vec(0,0) );
		}else
			lens->map.push_back( Vec (0,0) );
	}
	lens->width = radius*2;
	return lens;
}

DistortionMap* swirlMap(int radius)
{
	DistortionMap* swirl = new DistortionMap;

	for ( int y = 0; y < radius*2; ++y )
	for ( int x = 0; x < radius*2; ++x )
	{
		Vec delta = Vec( x - radius, y - radius );
		Vec deltaPerp = delta.perp();
		
		float perpFactor;
		float normFactor;
		
		if ( delta.length() <= radius )
		{
			if (delta.length() != 0)
			{
				normFactor = delta.length() / radius;
				perpFactor = 1 - normFactor;
				
				Vec newPos = delta*normFactor + deltaPerp*perpFactor;
				
				swirl->map.push_back( newPos - delta );
			}else
				swirl->map.push_back( Vec(0,0) );
		}else
			swirl->map.push_back( Vec (0,0) );
	}
	swirl->width = radius*2;
	return swirl;
}

DistortionMap* spinMap(int radius)
{
	DistortionMap* spin = new DistortionMap;

	for ( int y = 0; y < radius*2; ++y )
	for ( int x = 0; x < radius*2; ++x )
	{
		Vec delta = Vec( x - radius, y - radius );
		
		float factor;
		
		if ( delta.length() <= radius )
		{
			if (delta.length() != 0)
			{
				factor = 1 - delta.length() / radius;
				
				Vec newPos = angleVec( delta.getAngle() + 180 * factor, delta.length());
				
				spin->map.push_back( newPos - delta );
			}else
				spin->map.push_back( Vec(0,0) );
		}else
			spin->map.push_back( Vec (0,0) );
	}
	spin->width = radius*2;
	return spin;
}

DistortionMap* rippleMap(int radius, int frequency)
{
	DistortionMap* ripple = new DistortionMap;

	for ( int y = 0; y < radius*2; ++y )
	for ( int x = 0; x < radius*2; ++x )
	{
		Vec delta = Vec( x - radius, y - radius );
		
		float rippleFactor;
		
		if ( delta.length() <= radius )
		{
			if (delta.length() != 0)
			{
				rippleFactor = -cos(delta.length() * 2 * frequency * PI / radius) + 1;
				
				ripple->map.push_back( delta.normal() * rippleFactor * radius / frequency);
			}else
				ripple->map.push_back( Vec(0,0) );
		}else
			ripple->map.push_back( Vec (0,0) );
	}
	ripple->width = radius * 2;
	return ripple;
}

DistortionMap* randomMap(int radius)
{
	DistortionMap* lens = new DistortionMap;

	for ( int y = 0; y < radius*2; ++y )
	for ( int x = 0; x < radius*2; ++x )
	{
		lens->map.push_back( angleVec(rnd()*360, rnd() * 2 ) );
	}
	lens->width = radius*2;
	return lens;
}

DistortionMap* bitmapMap(const string &filename)
{
	int currVdepth = get_color_depth();
	
	set_color_depth(32);
	
	DistortionMap* lens = new DistortionMap;
	
	BITMAP* heightMap = load_bitmap(filename.c_str(),0);

	if ( heightMap )
	{
		for ( int y = 0; y < heightMap->h; ++y )
		for ( int x = 0; x < heightMap->w; ++x )
		{
			Vec distort;
			float value;
			float s,h;
			int c = getpixel(heightMap,x-1,y);
			rgb_to_hsv(getr(c), getg(c), getb(c), &h, &s, &value);
			distort+= Vec(-1,0)*value;
			c = getpixel(heightMap,x+1,y);
			rgb_to_hsv(getr(c), getg(c), getb(c), &h, &s, &value);
			distort+= Vec(1,0)*value;
			c = getpixel(heightMap,x,y-1);
			rgb_to_hsv(getr(c), getg(c), getb(c), &h, &s, &value);
			distort+= Vec(0,-1)*value;
			c = getpixel(heightMap,x,y+1);
			rgb_to_hsv(getr(c), getg(c), getb(c), &h, &s, &value);
			distort+= Vec(0,1)*value;
			lens->map.push_back( distort*10 );
		}
		lens->width = heightMap->w;
		destroy_bitmap(heightMap);
	}else
	{
		lens->width = 0;
	}
	
	set_color_depth(currVdepth);
	
	return lens;
}

Distortion::Distortion(DistortionMap* map)
{
	m_map = map;
	//m_map = lensMap(40);
	buffer = create_bitmap( m_map->width, m_map->map.size() / m_map->width);
	width = m_map->width;
	height = m_map->map.size() / m_map->width;
}

Distortion::~Distortion()
{
	if ( buffer ) destroy_bitmap( buffer );
	if ( m_map ) delete m_map;
}

void Distortion::apply( BITMAP* where, int _x, int _y, float multiply = 1)
{
	_x -= width / 2;
	_y -= height / 2;
	int y = 0;
	Vec offset;
	while ( y * m_map->width < m_map->map.size() )
	{
		for ( int x = 0; x < m_map->width; ++x )
		{
			offset = m_map->map[x+y*m_map->width] * multiply;
			putpixel(buffer, x,y, getpixel(where, static_cast<int>(x+_x+offset.x), static_cast<int>(y+_y+offset.y) ));
		}
		++y;
	}
	blit(buffer, where, 0, 0, _x, _y, buffer->w, buffer->h);
}

