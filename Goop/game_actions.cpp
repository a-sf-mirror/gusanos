#include "game_actions.h"

#include "game.h"
//#include "particle.h"
#include "part_type.h"
#include "explosion.h"
#include "exp_type.h"
#include "sound.h"
#include "sprite_set.h"
#include "text.h"
#include "base_object.h"
#include "weapon.h"
#include "worm.h"
#include "angle.h"
#include "level_effect.h"

#include "glua.h"
#include "script.h"

#include <allegro.h>

#include <boost/random.hpp>

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

void ShootParticles::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	if (type)
	{
/*
		Vec spd;
		Angle tmpAngle;
		char dir = object->getDir();
		//for ( int i = 0; i < amount + rnd()*amountVariation; i++) //WTF??
		int realAmount = amount + int(rnd()*amountVariation);
		for ( int i = 0; i < realAmount; i++)
		{
			tmpAngle = object->getAngle() + angleOffset * dir + distribution * midrnd();
			spd = Vec( tmpAngle, speed + midrnd()*speedVariation );
			spd += object->spd * motionInheritance;
			game.insertParticle( new Particle( type, object->pos + Vec( tmpAngle, (double)distanceOffset) , spd, object->getDir(), object->getOwner() ));
		}
*/
		int dir = object->getDir();
		Angle baseAngle(object->getAngle() + angleOffset * dir);
		
		int realAmount = amount + int(rnd()*amountVariation);
		for(int i = 0; i < realAmount; ++i)
		{
			Angle angle = baseAngle;
			if(distribution) angle += distribution * midrnd();
			Vec direction(angle);
			Vec spd(direction * (speed + midrnd()*speedVariation));
			if(motionInheritance)
			{
				spd += object->spd * motionInheritance;
				angle = spd.getAngle(); // Need to recompute angle
			}

			type->newParticle(type, object->pos + direction * distanceOffset, spd, object->getDir(), object->getOwner(), angle);
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

void UniformShootParticles::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	if (type)
	{
		int dir = object->getDir();
		Angle angle(object->getAngle() + (angleOffset * dir) - ( distribution * 0.5f ) );
		
		
		int realAmount = amount + int(rnd()*amountVariation);
		
		AngleDiff angleIncrease( distribution / realAmount );
				
		for(int i = 0; i < realAmount; ++i, angle += angleIncrease )
		{
			Vec direction(angle);
			Vec spd(direction * (speed + midrnd()*speedVariation));
			if(motionInheritance)
			{
				spd += object->spd * motionInheritance;
				angle = spd.getAngle(); // Need to recompute angle
			}

			type->newParticle(type, object->pos + direction * distanceOffset, spd, object->getDir(), object->getOwner(), angle);
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

void CreateExplosion::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	if (type != NULL)
	{
		game.insertExplosion( new Explosion( type, object->pos, object->getOwner() ) );
	}
}

CreateExplosion::~CreateExplosion()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

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

void Push::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon  )
{
	object2->spd += object->spd*factor;
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

void Repel::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon  )
{
/*
	float distance = ( object2->pos - object->pos ).length();
	if ( ( distance > 0 ) && ( distance < maxDistance ) )
	{
		object2->spd += ( object2->pos - object->pos ).normal() * ( maxForce + distance * ( minForce - maxForce ) / maxDistance );
	}
*/
	Vec dir( object2->pos - object->pos );
	double distanceSqr = dir.lengthSqr();
	if ( distanceSqr > 0.f && distanceSqr < maxDistanceSqr )
	{
		double distance = sqrt(distanceSqr);
		dir /= distance; // Normalize
		object2->spd += dir * ( maxForce + distance * forceDiffScaled);
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

void Damp::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon  )
{
	object2->spd *= factor;
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

void Damage::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon  )
{
	if ( object2 )
		object2->damage( m_damage + rnd() * m_damageVariation, object->getOwner() );
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

void Remove::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon  )
{
	object->remove();
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

void PlaySound::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon  )
{
	if (sound != NULL)
	{
		sound->play2D(object,loudness,pitch,pitchVariation);
	}
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
}

void PlayRandomSound::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon  )
{
	if ( !sounds.empty() )
	{
		int sound = rnd() * sounds.size(); // TODO: Make an integer version of rnd()
		if ( sounds[sound] != NULL )
		{
			sounds[sound]->play2D(object,loudness,pitch,pitchVariation);
		}
	}
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

void PlaySoundStatic::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon  )
{
	if (sound != NULL)
	{
		sound->play2D(object->pos,loudness,pitch,pitchVariation);
	}
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

void DelayFire::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	if(weapon)
	{
		weapon->delay( static_cast<int>(delayTime + rnd()*delayTimeVariation) );
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
}

void ShowFirecone::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	BaseWorm* w;
	if( w = dynamic_cast<BaseWorm*>(object) )
	{
		w->showFirecone( sprite, frames, drawDistance );
	}
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

void AddAngleSpeed::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	if (object)
	{
		object->addAngleSpeed( speed*object->getDir() + speedVariation*rnd() );
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

void SetAlphaFade::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	if (object)
	{
		object->setAlphaFade( frames, dest );
	}
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

void RunCustomEvent::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	if (object2)
	{
		object2->customEvent( index );
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

void RunScript::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	lua.pushReference(function);
	
	if(object)
		object->pushLuaReference();
	else
		lua_pushnil(lua);
		
	if(object2)
		object2->pushLuaReference();
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

void ApplyMapEffect::run( BaseObject* object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	if ( effect )
		game.level.applyEffect(effect, (int)object->pos.x, (int)object->pos.y);
}

ApplyMapEffect::~ApplyMapEffect()
{
}

