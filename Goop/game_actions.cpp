#include "game_actions.h"

#include "game.h"
#include "particle.h"
#include "part_type.h"
#include "sound.h"
#include "text.h"
#include "base_object.h"
#include "weapon.h"
#include "worm.h"

#include <allegro.h>

#include <boost/random.hpp>

using namespace std;

void registerGameActions()
{
	game.actionList["shoot_particles"] = shootParticles;
	game.actionList["remove"] = remove;
	game.actionList["play_sound"] = playSound;
	game.actionList["play_sound_static"] = playSoundStatic;
	game.actionList["delay_fire"] = delayFire;
	game.actionList["add_angle_speed"] = addAngleSpeed;
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
		motionInheritance = cast<float>(params[4]);
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

void ShootParticles::run( BaseObject* object, BaseObject *object2, Worm *worm, Weapon *weapon )
{
	if (type != NULL)
	{
		if(!weapon)
		{
			Vec spd;
			for ( int i = 0; i < amount; i++)
			{
				spd = angleVec( object->getAngle() + angleOffset + midrnd()*distribution, speed + midrnd()*speedVariation );
				spd += object->getSpd() * motionInheritance;
				game.objects.push_back( new Particle( type, object->getPos(), spd ));
			}
		}else
		{
			Vec spd;
			char dir = weapon->getOwner()->getDir();
			for ( int i = 0; i < amount; i++)
			{
				spd = angleVec( object->getAngle() + angleOffset * dir + midrnd()*distribution, speed + midrnd()*speedVariation );
				spd += object->getSpd() * motionInheritance;
				game.objects.push_back( new Particle( type, object->getPos(), spd ));
			}
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

void Remove::run( BaseObject* object, BaseObject *object2, Worm *worm, Weapon *weapon  )
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
	loudness = 100;
	pitch = 1;
	pitchVariation = 0;

	if ( params.size() >= 1 )
	{
		sound = soundList.load(params[0]);
	}
	if( params.size() >= 2 )
	{
		loudness = cast<float>(params[1]);
	}
	if( params.size() >= 3 )
	{
		pitch = cast<float>(params[2]);
	}
	if( params.size() >= 4 )
	{
		pitchVariation = cast<float>(params[3]);
	}
}

void PlaySound::run( BaseObject* object, BaseObject *object2, Worm *worm, Weapon *weapon  )
{
	if (sound != NULL)
	{
		sound->play2D(object,loudness,pitch,pitchVariation);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* playSoundStatic( const vector< string >& params )
{
	return new PlaySoundStatic(params);
}

PlaySoundStatic::PlaySoundStatic( const vector< string >& params )
{
	sound = NULL;
	loudness = 100;
	pitch = 1;
	pitchVariation = 0;

	if ( params.size() >= 1 )
	{
		sound = soundList.load(params[0]);
	}
	if( params.size() >= 2 )
	{
		loudness = cast<float>(params[1]);
	}
	if( params.size() >= 3 )
	{
		pitch = cast<float>(params[2]);
	}
	if( params.size() >= 4 )
	{
		pitchVariation = cast<float>(params[3]);
	}
}

void PlaySoundStatic::run( BaseObject* object, BaseObject *object2, Worm *worm, Weapon *weapon  )
{
	if (sound != NULL)
	{
		sound->play2D(object->getPos(),loudness,pitch,pitchVariation);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* delayFire( const vector< string >& params )
{
	return new DelayFire(params);
}

DelayFire::DelayFire( const vector< string >& params )
{
	delayTime = 0;
	delayTimeVariation = 0;
	if ( params.size() >= 1 )
	{
		delayTime = cast<int>(params[0]);
	}
	if( params.size() >= 2 )
	{
		delayTimeVariation = cast<int>(params[1]);
	}
}

void DelayFire::run( BaseObject* object, BaseObject *object2, Worm *worm, Weapon *weapon )
{
	if(weapon)
	{
		weapon->delay( delayTime + rnd()*delayTimeVariation );
	}
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* addAngleSpeed( const vector< string >& params )
{
	return new AddAngleSpeed(params);
}

AddAngleSpeed::AddAngleSpeed( const vector< string >& params )
{
	speed = 0;
	speedVariation = 0;
	if ( params.size() >= 1 )
	{
		speed = cast<float>(params[0]);
	}
	if( params.size() >= 2 )
	{
		speedVariation = cast<float>(params[1]);
	}
}

void AddAngleSpeed::run( BaseObject* object, BaseObject *object2, Worm *worm, Weapon *weapon )
{
	if (object)
	{
		object->addAngleSpeed( speed + rnd()*speedVariation );
	}
}

