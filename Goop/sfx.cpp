#include "sfx.h"
//#include "gconsole.h"
#include "base_object.h"

#include <vector>
#include <fmod/fmod.h>

using namespace std;

Sfx sfx;

Sfx::Sfx()
{
}

Sfx::~Sfx()
{
}

void Sfx::init()
{	
	FSOUND_Init(44100, 32, 0);
	for ( int i = 0; i < FSOUND_GetMaxChannels(); i++ )
	{
		chanObject.push_back( (BaseObject*) NULL );
	}
	
}

void Sfx::shutDown()
{
	FSOUND_Close();
}

void Sfx::registerInConsole()
{
}

void Sfx::updateChanPositions()
{
	int i;
	for ( i = 0; i < FSOUND_GetMaxChannels(); i++ )
	{
		if ( chanObject[i] )
		{
			float pos[3] = { chanObject[i]->getPos().x, chanObject[i]->getPos().y, 0 };
			FSOUND_3D_SetAttributes(i, pos, NULL);
		}
	}
}

void Sfx::checkForDeletedObjects()
{
	int i;
	for ( i = 0; i < FSOUND_GetMaxChannels(); i++ )
	{
		if ( chanObject[i] && chanObject[i]->deleteMe ) // If the object is going to be deleted
		{
			chanObject[i] = NULL; // Remove it from this list so it doesnt get updated anymore 
		}
	}
}

void Sfx::setChanObject(int chan, BaseObject* object)
{
	chanObject[chan] = object;
}


