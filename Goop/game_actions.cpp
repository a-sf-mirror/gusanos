#include "game_actions.h"

#include "game.h"
#include "particle.h"
#include "part_type.h"
#include "sound.h"
#include "text.h"
#include "base_object.h"

#include <allegro.h>

#include <boost/random.hpp>

using namespace std;

void registerGameActions()
{
	game.actionList["shoot_particles"] = shootParticles;
	game.actionList["remove"] = remove;
	game.actionList["play_sound"] = playSound;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* shootParticles( const vector< string >& params )
{
	return new ShootParticles(params);
}

ShootParticles::ShootParticles( const vector< string >& params )
{
	type = NULL;
	amount = 0;
	amountVariation = 0;
	speed = 0;
	speedVariation = 0;
	motionInheritance = 0;
	distribution = 360;
	angleOffset = 0;
	if ( params.size() >= 1 )
	{
		type = partTypeList.load(params[0]);
	}
	if( params.size() >= 2 )
	{
		amount = cast<int>(params[1]);
	}
	if( params.size() >= 3 )
	{
		speed = cast<float>(params[2]);
	}
	if( params.size() >= 4 )
	{
		speedVariation = cast<float>(params[3]);
	}
	if( params.size() >= 5 )
	{
		motionInheritance = cast<int>(params[4]);
	}
	if( params.size() >= 6 )
	{
		amountVariation = cast<int>(params[5]);
	}
	if( params.size() >= 7 )
	{
		distribution = cast<int>(params[6]);
	}
	if( params.size() >= 8 )
	{
		angleOffset = cast<int>(params[7]);
	}
}

void ShootParticles::run( BaseObject* object )
{
	if (type != NULL)
	{
		Vec spd;
		for ( int i = 0; i < amount; i++)
		{
			spd = angleVec( object->getAngle() + angleOffset + midrnd()*distribution, speed + midrnd()*speedVariation );
			spd += object->getSpd() * motionInheritance;
			game.objects.push_back( new Particle( type, object->getPos(), spd ));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* remove( const vector< string >& params )
{
	return new Remove(params);
}

Remove::Remove( const vector< string >& params )
{
}

void Remove::run( BaseObject* object )
{
	object->deleteMe = true;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* playSound( const vector< string >& params )
{
	return new PlaySound(params);
}

PlaySound::PlaySound( const vector< string >& params )
{
	sound = NULL;
	volume = 1;
	pitch = 1;
	volumeVariation = 0;
	pitchVariation = 0;
	loudness = 100;
	if ( params.size() >= 1 )
	{
		sound = soundList.load(params[0]);
	}
	if( params.size() >= 2 )
	{
		volume = cast<float>(params[1]);
	}
	if( params.size() >= 3 )
	{
		pitch = cast<float>(params[2]);
	}
	if( params.size() >= 4 )
	{
		volumeVariation = cast<float>(params[3]);
	}
	if( params.size() >= 5 )
	{
		pitchVariation = cast<float>(params[4]);
	}
	if( params.size() >= 6 )
	{
		loudness = cast<float>(params[5]);
	}
}

void PlaySound::run( BaseObject* object )
{
	if (sound != NULL)
	{
		sound->play2D(object,loudness,volume,pitch,volumeVariation,pitchVariation);
	}
}









