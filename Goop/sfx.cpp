#include "sfx.h"
#include "gconsole.h"
#include "base_object.h"
#include "text.h" //For cast<>

#include <vector>
#include <fmod/fmod.h>

using namespace std;

Sfx sfx;

Sfx::Sfx()
: m_initialized(false), m_outputMode(-1)
{
}

Sfx::~Sfx()
{
}

void Sfx::init()
{
	// Select a driver
	
	FSOUND_SetOutput(m_outputMode);
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
	
	m_initialized = true;
}

void Sfx::shutDown()
{
	FSOUND_Close();
}

void Sfx::registerInConsole()
{
	{ // TODO: Only add modes relevant to the platform
		map<string, int> outputModes;
		outputModes["NOSFX"] = FSOUND_OUTPUT_NOSOUND;
		outputModes["WINMM"] = FSOUND_OUTPUT_WINMM;
		outputModes["DSOUND"] = FSOUND_OUTPUT_DSOUND;
		outputModes["A3D"] = FSOUND_OUTPUT_A3D;
		outputModes["OSS"] = FSOUND_OUTPUT_OSS;
		outputModes["ESD"] = FSOUND_OUTPUT_ESD;
		outputModes["ALSA"] = FSOUND_OUTPUT_ALSA;
		outputModes["ASIO"] = FSOUND_OUTPUT_ASIO;
		outputModes["XBOX"] = FSOUND_OUTPUT_XBOX;
		outputModes["PS2"] = FSOUND_OUTPUT_PS2;
		outputModes["MAC"] = FSOUND_OUTPUT_MAC;
		outputModes["GC"] = FSOUND_OUTPUT_GC;
		outputModes["DEFAULT"] = -1;
		
		console.registerEnumVariable("SFX_OUTPUT_MODE", &m_outputMode, -1, outputModes);
		// NOTE: When/if adding a callback to sfx variables, make it do nothing if
		// sfx.operator bool() returns false.
	}
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


