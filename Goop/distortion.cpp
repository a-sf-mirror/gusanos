#include "distortion.h"

#include <allegro.h>
#include <vector>
#include "vec.h"

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
			putpixel(buffer, x,y, getpixel(where,x+_x+offset.x,y+_y+offset.y));
		}
		++y;
	}
	blit(buffer, where, 0, 0, _x, _y, buffer->w, buffer->h);
}

