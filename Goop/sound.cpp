#include "sound.h"

#include "resource_list.h"
#include "vec.h"

#include <string>

#include <fmod/fmod.h>

using namespace std;

ResourceList<Sound> soundList("sounds/");

Sound::Sound()
{
	m_sound = NULL;
}

Sound::~Sound()
{
	if ( m_sound )  FSOUND_Sample_Free( m_sound );
}

bool Sound::load(const string &filename)
{	
	m_sound = FSOUND_Sample_Load( FSOUND_FREE, filename.c_str(), 0, 0, 0 );
	if ( m_sound )
	{
		return true;
	}
	return false;
}

void Sound::play(float volume,float pitch, float volumeVariation, float pitchVariation)
{
	if( m_sound ) 
	{
		int chan = FSOUND_PlaySound(FSOUND_FREE,m_sound);
		if ( chan != -1 )
		{
			float rndPitch = pitch + rnd()*pitchVariation - pitchVariation / 2;
			FSOUND_SetFrequency(chan, FSOUND_GetFrequency(chan) * rndPitch );
			
			float rndVolume = pitch + rnd()*volumeVariation - volumeVariation / 2;
			FSOUND_SetVolume(chan,FSOUND_GetVolume(chan)*rndVolume);
		}
	}
}

