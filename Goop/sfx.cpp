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
	list< pair< int, BaseObject* > >::iterator i;
	for ( i = chanObject.begin(); i != chanObject.end(); i++ )
	{
		if ( (*i).second )
		{
			float pos[3] = { (*i).second->getPos().x, (*i).second->getPos().y, 0 };
			FSOUND_3D_SetAttributes((*i).first, pos, NULL);
		}
	}
}

void Sfx::checkForDeletedObjects()
{
	list< pair< int, BaseObject* > >::iterator i;
	for ( i = chanObject.begin(); i != chanObject.end(); )
	{
		if ( (*i).second && ( (*i).second->deleteMe || !FSOUND_IsPlaying( (*i).first ) ) )
		{
			list< pair< int, BaseObject* > >::iterator tmp;
			tmp = i;
			i++;
			chanObject.erase(tmp);
		}else i++;
	}
}

void Sfx::setChanObject(int chan, BaseObject* object)
{
	chanObject.push_back( pair< int, BaseObject* > ( chan, object ) );
}


