#include "detect_event.h"

#include "events.h"
#include "base_object.h"
#include "game.h"

DetectEvent::DetectEvent( float range, bool detectOwner, int detectFilter)
	: m_detectFilter(detectFilter), m_range(range), m_detectOwner(detectOwner)
{
	m_event = new Event;
}

DetectEvent::~DetectEvent()
{
	delete m_event;
}

void DetectEvent::check( BaseObject* ownerObject )
{
	if ( m_detectFilter & 1 ) // 1 is the worm collision layer flag
	{
		ObjectsList::ColLayerIterator worm;
		for ( worm = game.objects.colLayerBegin(WORMS_COLLISION_LAYER); (bool)worm; ++worm)
		{
			if ( m_detectOwner || (*worm)->getOwner() != ownerObject->getOwner() )
				if ( (*worm)->isCollidingWith( ownerObject->pos, m_range) )
			{
				m_event->run( ownerObject, (*worm) );
			}
		}
	}
	// from CUSTOM_COL_LAYER_START to COLLISION_LAYERS_AMMOUNT its the particles collision layers
	for ( int customFilter = CUSTOM_COL_LAYER_START, filterFlag = 2; customFilter < COLLISION_LAYERS_AMMOUNT; ++customFilter, filterFlag*=2 )
	{
		if ( m_detectFilter & filterFlag )
		{
			ObjectsList::ColLayerIterator object;
			for ( object = game.objects.colLayerBegin(customFilter); (bool)object; ++object)
			{
				if ( (*object) != ownerObject )
				if ( !(*object)->deleteMe && (m_detectOwner || (*object)->getOwner() != ownerObject->getOwner() ) )
				if ( (*object)->isCollidingWith( ownerObject->pos, m_range) )
				{
					m_event->run( ownerObject,(*object) );
				}
			}
		}
	}
}