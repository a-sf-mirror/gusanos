#include "sound.h"

#include "sfx.h"
#include "resource_list.h"
#include "vec.h"
#include "base_object.h"

#include <string>

#include <allegro.h>
#include "text.h"

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
	m_sound = FSOUND_Sample_Load( FSOUND_FREE, filename.c_str(), FSOUND_HW3D | FSOUND_FORCEMONO, 0, 0 );
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

void Sound::play2D(const Vec& pos, float loudness, float pitch, float pitchVariation)
{
	if( m_sound ) 
	{
		int chan = FSOUND_PlaySound(FSOUND_FREE,m_sound);
		if ( chan != -1 )
		{
			float _pos[3] = { pos.x, pos.y, 0 };
			FSOUND_3D_SetAttributes(chan, _pos, NULL);
			
			float rndPitch = pitch + rnd()*pitchVariation - pitchVariation / 2;
			FSOUND_SetFrequency(chan, FSOUND_GetFrequency(chan) * rndPitch );
			
			FSOUND_3D_SetMinMaxDistance(chan, loudness, 10000.0f);
		}
	}
}

void Sound::play2D(BaseObject* obj, float loudness, float pitch, float pitchVariation)
{
	if( m_sound ) 
	{
		int chan = FSOUND_PlaySound(FSOUND_FREE,m_sound);
		if ( chan != -1 )
		{
			float pos[3] = { obj->getPos().x, obj->getPos().y, 0 };
			FSOUND_3D_SetAttributes(chan, pos, NULL);
			
			sfx.setChanObject( chan, obj );
			
			float rndPitch = pitch + rnd()*pitchVariation - pitchVariation / 2;
			FSOUND_SetFrequency(chan, FSOUND_GetFrequency(chan) * rndPitch );
			
			FSOUND_3D_SetMinMaxDistance(chan, loudness, 10000.0f);
		}
	}
}
