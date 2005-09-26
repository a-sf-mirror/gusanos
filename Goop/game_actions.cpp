#include "game_actions.h"

#include "game.h"
//#include "particle.h"
#include "part_type.h"
#include "explosion.h"
#include "exp_type.h"
#ifndef DEDSERV
#include "sound.h"
#include "sprite_set.h"
#endif
#include "text.h"
#include "base_object.h"
#include "weapon.h"
#include "worm.h"
#include "angle.h"
#include "level_effect.h"
#include "math_func.h"

#include "glua.h"
#include "script.h"

//#include <allegro.h>

//#include <boost/random.hpp>

using namespace std;

void registerGameActions()
{
	game.actionList["shoot_particles"] = shootParticles;
	game.actionList["uniform_shoot_particles"] = uniformShootParticles;
	game.actionList["create_explosion"] = createExplosion;
	game.actionList["remove"] = remove;
	game.actionList["play_sound"] = playSound;
	game.actionList["play_random_sound"] = playRandomSound;
	game.actionList["play_sound_static"] = playSoundStatic;
	game.actionList["delay_fire"] = delayFire;
	game.actionList["add_angle_speed"] = addAngleSpeed;
	game.actionList["push"] = push;
	game.actionList["damage"] = damage;
	game.actionList["set_alpha_fade"] = setAlphaFade;
	game.actionList["show_firecone"] = showFirecone;
	game.actionList["custom_event"] = runCustomEvent;
	game.actionList["run_script"] = runScript;
	game.actionList["repel"] = repel;
	game.actionList["damp"] = damp;
	game.actionList["apply_map_effect"] = applyMapEffect;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* shootParticles( const vector< string >& params )
{
	return new ShootParticles(params);
}

ShootParticles::ShootParticles( const vector< string >& params )
: distribution(360.0), angleOffset(0.0)
{
	type = NULL;
	amount = 0;
	amountVariation = 0;
	speed = 0;
	speedVariation = 0;
	motionInheritance = 0;
	distanceOffset = 0;
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
		//distribution = cast<float>(params[6]);
		distribution = cast<Angle>(params[6]);
	}
	if( params.size() >= 8 )
	{
		//angleOffset = cast<float>(params[7]);
		angleOffset = cast<AngleDiff>(params[7]);
	}
	if( params.size() >= 9 )
	{
		distanceOffset = cast<float>(params[8]);
	}
}

void ShootParticles::run( ActionParams const& params )
{
	if (type)
	{
/*
		Vec spd;
		Angle tmpAngle;
		char dir = params.object->getDir();
		//for ( int i = 0; i < amount + rnd()*amountVariation; i++) //WTF??
		int realAmount = amount + int(rnd()*amountVariation);
		for ( int i = 0; i < realAmount; i++)
		{
			tmpAngle = params.object->getAngle() + angleOffset * dir + distribution * midrnd();
			spd = Vec( tmpAngle, speed + midrnd()*speedVariation );
			spd += params.object->spd * motionInheritance;
			game.insertParticle( new Particle( type, params.object->pos + Vec( tmpAngle, (double)distanceOffset) , spd, params.object->getDir(), params.object->getOwner() ));
		}
*/
		int dir = params.object->getDir();
		Angle baseAngle(params.object->getAngle() + angleOffset * dir);
		
		int realAmount = amount + int(rnd()*amountVariation);
		for(int i = 0; i < realAmount; ++i)
		{
			Angle angle = baseAngle;
			if(distribution) angle += distribution * midrnd();
			Vec direction(angle);
			Vec spd(direction * (speed + midrnd()*speedVariation));
			if(motionInheritance)
			{
				spd += params.object->spd * motionInheritance;
				angle = spd.getAngle(); // Need to recompute angle
			}

			type->newParticle(type, params.object->pos + direction * distanceOffset, spd, params.object->getDir(), params.object->getOwner(), angle);
		}
	}
}

ShootParticles::~ShootParticles()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* uniformShootParticles( const vector< string >& params )
{
	return new UniformShootParticles(params);
}

UniformShootParticles::UniformShootParticles( const vector< string >& params )
: distribution(360.0), angleOffset(0.0)
{
	type = NULL;
	amount = 0;
	amountVariation = 0;
	speed = 0;
	speedVariation = 0;
	motionInheritance = 0;
	distanceOffset = 0;
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
		//distribution = cast<float>(params[6]);
		distribution = cast<Angle>(params[6]);
	}
	if( params.size() >= 8 )
	{
		//angleOffset = cast<float>(params[7]);
		angleOffset = cast<AngleDiff>(params[7]);
	}
	if( params.size() >= 9 )
	{
		distanceOffset = cast<float>(params[8]);
	}
}

void UniformShootParticles::run( ActionParams const& params )
{
	if (type)
	{
		int dir = params.object->getDir();
		Angle angle(params.object->getAngle() + (angleOffset * dir) - ( distribution * 0.5f ) );
		
		
		int realAmount = amount + int(rnd()*amountVariation);
		
		AngleDiff angleIncrease( distribution / realAmount );
				
		for(int i = 0; i < realAmount; ++i, angle += angleIncrease )
		{
			Vec direction(angle);
			Vec spd(direction * (speed + midrnd()*speedVariation));
			if(motionInheritance)
			{
				spd += params.object->spd * motionInheritance;
				angle = spd.getAngle(); // Need to recompute angle
			}

			type->newParticle(type, params.object->pos + direction * distanceOffset, spd, params.object->getDir(), params.object->getOwner(), angle);
		}
	}
}

UniformShootParticles::~UniformShootParticles()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* createExplosion( const vector< string >& params )
{
	return new CreateExplosion(params);
}

CreateExplosion::CreateExplosion( const vector< string >& params )
{
	type = NULL;
	if ( params.size() >= 1 )
	{
		type = expTypeList.load(params[0]);
	}
}

void CreateExplosion::run( ActionParams const& params )
{
	if (type != NULL)
	{
		game.insertExplosion( new Explosion( type, params.object->pos, params.object->getOwner() ) );
	}
}

CreateExplosion::~CreateExplosion()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*! push(factor)

	Pushes the second object in the direction the first object is moving.
*/
BaseAction* push( const vector< string >& params )
{
	return new Push(params);
}

Push::Push( const vector< string >& params )
{
	factor = 0;

	if ( params.size() >= 1 )
	{
		factor = cast<float>(params[0]);
	}
}

void Push::run( ActionParams const& params  )
{
	params.object2->spd += params.object->spd*factor;
}

Push::~Push()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* repel( const vector< string >& params )
{
	return new Repel(params);
}

Repel::Repel( const vector< string >& params )
{
	maxForce = 0;
	minForce = 0;
	maxDistance = 0;

	if ( params.size() > 0 )
	{
		maxForce = cast<float>(params[0]);
	}
	if ( params.size() > 1 )
	{
		maxDistance = cast<float>(params[1]);
	}
	if ( params.size() > 2 )
	{
		minForce = cast<float>(params[2]);
	}
	
	maxDistanceSqr = maxDistance*maxDistance;
	forceDiffScaled = (minForce - maxForce) / maxDistance;
}

void Repel::run( ActionParams const& params )
{
/*
	float distance = ( params.object2->pos - params.object->pos ).length();
	if ( ( distance > 0 ) && ( distance < maxDistance ) )
	{
		params.object2->spd += ( params.object2->pos - params.object->pos ).normal() * ( maxForce + distance * ( minForce - maxForce ) / maxDistance );
	}
*/
	Vec dir( params.object2->pos - params.object->pos );
	double distanceSqr = dir.lengthSqr();
	if ( distanceSqr > 0.f && distanceSqr < maxDistanceSqr )
	{
		double distance = sqrt(distanceSqr);
		dir /= distance; // Normalize
		params.object2->spd += dir * ( maxForce + distance * forceDiffScaled);
	}
}

Repel::~Repel()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* damp( const vector< string >& params )
{
	return new Damp(params);
}

Damp::Damp( const vector< string >& params )
{
	factor = 0;

	if ( params.size() > 0 )
	{
		factor = cast<float>(params[0]);
	}
}

void Damp::run( ActionParams const& params )
{
	params.object2->spd *= factor;
}

Damp::~Damp()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* damage( const vector< string >& params )
{
	return new Damage(params);
}

Damage::Damage( const vector< string >& params )
{
	m_damage = 0;
	m_damageVariation = 0;

	if ( params.size() > 0 )
	{
		m_damage = cast<float>(params[0]);
	}
	if ( params.size() > 1 )
	{
		m_damageVariation = cast<float>(params[1]);
	}
}

void Damage::run( ActionParams const& params )
{
	if ( params.object2 )
		params.object2->damage( m_damage + rnd() * m_damageVariation, params.object->getOwner() );
}

Damage::~Damage()
{
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

void Remove::run( ActionParams const& params )
{
	params.object->remove();
}

Remove::~Remove()
{
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
#ifndef DEDSERV
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
#endif
}

void PlaySound::run( ActionParams const& params )
{
#ifndef DEDSERV
	if (sound != NULL)
	{
		sound->play2D(params.object,loudness,pitch,pitchVariation);
	}
#endif
}

PlaySound::~PlaySound()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* playRandomSound( const vector< string >& params )
{
	return new PlayRandomSound(params);
}

PlayRandomSound::PlayRandomSound( const vector< string >& params )
{
#ifndef DEDSERV
	loudness = 100;
	pitch = 1;
	pitchVariation = 0;

	size_t i = 0;
	if( params.size() > i )
	{
		loudness = cast<float>(params[i]);
	}
	++i;
	if( params.size() > i )
	{
		pitch = cast<float>(params[i]);
	}
	++i;
	if( params.size() > i )
	{
		pitchVariation = cast<float>(params[i]);
	}
	++i;
	while ( params.size() > i )
	{
		sounds.push_back( soundList.load(params[i]) );
		++i;
	}
#endif
}

void PlayRandomSound::run( ActionParams const& params )
{
#ifndef DEDSERV
	if ( !sounds.empty() )
	{
		int sound = rnd() * sounds.size(); // TODO: Make an integer version of rnd()
		if ( sounds[sound] != NULL )
		{
			sounds[sound]->play2D(params.object,loudness,pitch,pitchVariation);
		}
	}
#endif
}

PlayRandomSound::~PlayRandomSound()
{
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
#ifndef DEDSERV
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
#endif
}

void PlaySoundStatic::run( ActionParams const& params )
{
#ifndef DEDSERV
	if (sound != NULL)
	{
		sound->play2D(params.object->pos,loudness,pitch,pitchVariation);
	}
#endif
}

PlaySoundStatic::~PlaySoundStatic()
{
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

void DelayFire::run( ActionParams const& params )
{
	if(params.weapon)
	{
		params.weapon->delay( static_cast<int>(delayTime + rnd()*delayTimeVariation) );
	}
}

DelayFire::~DelayFire()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* showFirecone( const vector< string >& params )
{
	return new ShowFirecone(params);
}

ShowFirecone::ShowFirecone( const vector< string >& params )
{
#ifndef DEDSERV
	sprite = NULL;
	frames = 0;
	drawDistance = 0;
	if ( params.size() > 0 )
	{
		sprite = spriteList.load(params[0]);
	}
	if( params.size() > 1 )
	{
		frames = cast<int>(params[1]);
	}
	if( params.size() > 2 )
	{
		drawDistance = cast<float>(params[2]);
	}
#endif
}

void ShowFirecone::run( ActionParams const& params )
{
#ifndef DEDSERV
	BaseWorm* w;
	if( w = dynamic_cast<BaseWorm*>(params.object) )
	{
		w->showFirecone( sprite, frames, drawDistance );
	}
#endif
}

ShowFirecone::~ShowFirecone()
{
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
		speed = cast<AngleDiff>(params[0]);
	}
	if( params.size() >= 2 )
	{
		speedVariation = cast<AngleDiff>(params[1]);
	}
}

void AddAngleSpeed::run( ActionParams const& params )
{
	if (params.object)
	{
		params.object->addAngleSpeed( speed*params.object->getDir() + speedVariation*rnd() );
	}
}

AddAngleSpeed::~AddAngleSpeed()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* setAlphaFade( const vector< string >& params )
{
	return new SetAlphaFade(params);
}

SetAlphaFade::SetAlphaFade( const vector< string >& params )
{
	frames = 0;
	dest = 0;
	if ( params.size() > 0 )
	{
		frames = cast<int>(params[0]);
	}
	if( params.size() > 1 )
	{
		dest = cast<int>(params[1]);
	}
}

void SetAlphaFade::run( ActionParams const& params )
{
#ifndef DEDSERV
	if (params.object)
	{
		params.object->setAlphaFade( frames, dest );
	}
#endif
}

SetAlphaFade::~SetAlphaFade()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* runCustomEvent( const vector< string >& params )
{
	return new RunCustomEvent(params);
}

RunCustomEvent::RunCustomEvent( const vector< string >& params )
{
	index = 0;
	if ( params.size() > 0 )
	{
		index = cast<int>(params[0]);
	}
}

void RunCustomEvent::run( ActionParams const& params )
{
	if (params.object2)
	{
		params.object2->customEvent( index );
	}
}

RunCustomEvent::~RunCustomEvent()
{
}


/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* runScript( vector< string > const& params )
{
	return new RunScript(params);
}

RunScript::RunScript( vector< string > const& params )
: function(0)
{
	if ( params.size() > 0 )
	{
		std::string const& scriptName = params[0];
		std::string::size_type p = scriptName.find('.');
		if(p != std::string::npos)
		{
			Script* s = scriptLocator.load(scriptName.substr(0, p));
			if(s)
			{
				function = s->createFunctionRef(scriptName.substr(p + 1, scriptName.size() - p - 1));
			}
		}
	}
}

void RunScript::run( ActionParams const& params )
{
	lua.pushReference(function);
	
	if(params.object)
		params.object->pushLuaReference();
	else
		lua_pushnil(lua);
		
	if(params.object2)
		params.object2->pushLuaReference();
	else
		lua_pushnil(lua);
		
	lua.call(2, 0);
}

RunScript::~RunScript()
{
	lua.destroyReference(function);
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

BaseAction* applyMapEffect( vector< string > const& params )
{
	return new ApplyMapEffect(params);
}

ApplyMapEffect::ApplyMapEffect( vector< string > const& params )
{
	effect = NULL;
	if ( params.size() > 0 )
	{
		effect = levelEffectList.load(params[0]);
	}
}

void ApplyMapEffect::run( ActionParams const& params )
{
	if ( effect )
		game.applyLevelEffect(effect, (int)params.object->pos.x, (int)params.object->pos.y);
}

ApplyMapEffect::~ApplyMapEffect()
{
}

