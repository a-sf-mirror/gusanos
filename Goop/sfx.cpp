#include "sfx.h"
#include "gconsole.h"
#include "base_object.h"
#include "text.h" //For cast<>

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
	// Select a driver
	
	//FSOUND_SetOutput(FSOUND_OUTPUT_ALSA); //TODO: Selection of this via console/command line
	FSOUND_SetDriver(0);
	
	/* We select default driver for now
	int numDrivers = FSOUND_GetNumDrivers();
	// TODO: Desired driver here: string desiredDriver(); 
	int selectedDriver = 0;
	
	for(int i = 0; i < numDrivers; ++i)
	{
		const char* driverName = FSOUND_GetDriverName(i);
		console.addLogMsg(string("* FMOD DRIVER ") + cast<string>(i) + string(": ") + driverName);
		if(string(driverName).find(desiredDriver) >= 0)
		{
			selectedDriver = i;
			break;
		}
	}
	
	FSOUND_SetDriver(selectedDriver);*/

	FSOUND_Init(44100, 32, 0);
	FSOUND_3D_SetDistanceFactor(20);
	FSOUND_3D_SetRolloffFactor(2);
	
	console.addLogMsg(string("* FMOD LIB INITIALIZED, USING DRIVER ") + FSOUND_GetDriverName(FSOUND_GetDriver()));
}

void Sfx::shutDown()
{
	FSOUND_Close();
}

void Sfx::registerInConsole()
{

}

void Sfx::think()
{
	FSOUND_Update();
	
	int i;
	for ( i = 0; i < listeners.size(); ++i )
	{
		FSOUND_3D_Listener_SetCurrent(i,listeners.size());
		float pos[3] = { listeners[i]->pos.x, listeners[i]->pos.y, -20 };
		FSOUND_3D_Listener_SetAttributes(pos,NULL,0,0,1,0,1,0);
	}
	
	//Update 3d channel that follow objects positions
	list< pair< int, BaseObject* > >::iterator obj;
	for ( obj = chanObject.begin(); obj != chanObject.end(); obj++ )
	{
		if ( (*obj).second )
		{
			float pos[3] = { (*obj).second->getPos().x, (*obj).second->getPos().y, 0 };
			FSOUND_3D_SetAttributes((*obj).first, pos, NULL);
		}
	}

	//Check for deleted objects
	for ( obj = chanObject.begin(); obj != chanObject.end(); )
	{
		if ( (*obj).second && ( (*obj).second->deleteMe || !FSOUND_IsPlaying( (*obj).first ) ) )
		{
			list< pair< int, BaseObject* > >::iterator tmp;
			tmp = obj;
			obj++;
			chanObject.erase(tmp);
		}else obj++;
	}

}

void Sfx::setChanObject(int chan, BaseObject* object)
{
	chanObject.push_back( pair< int, BaseObject* > ( chan, object ) );
}
	
void Sfx::clear()
{
	chanObject.clear();
}

Listener* Sfx::newListener()
{
	listeners.push_back( new Listener );
	return listeners.back();
}

void Sfx::freeListener(Listener* listener)
{
	vector<Listener*>::iterator i;
	for ( i = listeners.begin(); i != listeners.end(); ++i )
	{
		if ( listener == *i )
		{
			delete *i;
			listeners.erase(i);
			break;
		}
	}
}


