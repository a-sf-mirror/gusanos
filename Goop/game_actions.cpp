#include "game_actions.h"

#include "game.h"
//#include "particle.h"
#include "part_type.h"
#include "explosion.h"
#include "exp_type.h"
#ifndef DEDSERV
#include "sound.h"
#include "sound1d.h"
#include "sprite_set.h"
#endif
#include "util/text.h"
#include "util/angle.h"
#include "util/vec.h"
#include "util/macros.h"
#include "base_object.h"
#include "weapon.h"
#include "worm.h"
#include "level_effect.h"

#include "glua.h"
#include "script.h"

#include "omfg_script.h"

//#include <allegro.h>

//#include <boost/random.hpp>

using namespace std;

OmfgScript::ActionFactory gameActions;

template<class T>
BaseAction* newAction(vector<OmfgScript::TokenBase*> const& params )
{
	return new T(params);
}

template<class T>
BaseAction* oldAction(vector<string> const& params )
{
	return new T(params);
}

void registerGameActions()
{
	game.actionList["shoot_particles"] = oldAction<ShootParticles>;
	game.actionList["uniform_shoot_particles"] = oldAction<UniformShootParticles>;
	game.actionList["remove"] = oldAction<Remove>;
	
	game.actionList["create_explosion"] = oldAction<CreateExplosion>;
	game.actionList["play_sound"] = oldAction<PlaySound>;
	game.actionList["play_random_sound"] = oldAction<PlayRandomSound>;
	game.actionList["play_sound_static"] = oldAction<PlaySoundStatic>;
	game.actionList["play_global_sound"] = oldAction<PlayGlobalSound>;
	game.actionList["delay_fire"] = oldAction<DelayFire>;
	game.actionList["add_angle_speed"] = oldAction<AddAngleSpeed>;
	game.actionList["push"] = oldAction<Push>;
	game.actionList["damage"] = oldAction<Damage>;
	game.actionList["set_alpha_fade"] = oldAction<SetAlphaFade>;
	game.actionList["show_firecone"] = oldAction<ShowFirecone>;
	game.actionList["custom_event"] = oldAction<RunCustomEvent>;
	game.actionList["run_script"] = oldAction<RunScript>;
	game.actionList["repel"] = oldAction<Repel>;
	game.actionList["damp"] = oldAction<Damp>;
	game.actionList["apply_map_effect"] = oldAction<ApplyMapEffect>;
	game.actionList["put_particle"] = oldAction<PutParticle>;
	
	gameActions.add("shoot_particles", newAction<ShootParticles>)
		("type", false)
		("amount")
		("speed")
		("speed_var")
		("motion_inheritance")
		("amount_var")
		("distribution")
		("angle_offs")
		("distance_offs")
	;
	
	gameActions.add("uniform_shoot_particles", newAction<UniformShootParticles>)
		("type", false)
		("amount")
		("speed")
		("speed_var")
		("motion_inheritance")
		("amount_var")
		("distribution")
		("angle_offs")
		("distance_offs")
	;
	
	gameActions.add("remove", newAction<Remove>);
	
	gameActions.add("put_particle", newAction<PutParticle>)
		("type", false)
		("x", false)
		("y", false)
		("xspd")
		("yspd")
	;
	
	gameActions.add("create_explosion", newAction<CreateExplosion>)
		("type", false)
	;
	gameActions.add("play_sound", newAction<PlaySound>)
		("sound", false)	
		("loudness")
		("pitch")
		("pitch_var")
	;
	gameActions.add("play_random_sound", newAction<PlayRandomSound>)
		("loudness")
		("pitch")
		("pitch_var")
		("sounds", false)
	;
	gameActions.add("play_sound_static", newAction<PlaySoundStatic>)
		("sound", false)	
		("loudness")
		("pitch")
		("pitch_var")
	;
	gameActions.add("play_global_sound", newAction<CreateExplosion>)
		("sound", false)
		("volume")
		("volume_var")
		("pitch")
		("pitch_var")
	;
	gameActions.add("delay_fire", newAction<DelayFire>)
		("time")
		("time_var")
	;
	gameActions.add("add_angle_speed", newAction<AddAngleSpeed>)
		("amount")
		("amount_var")
	;
	gameActions.add("push", newAction<Push>)
		("factor")
	;
	gameActions.add("damage", newAction<Damage>)
		("amount")
		("amount_var")
	;
	gameActions.add("set_alpha_fade", newAction<SetAlphaFade>)
		("frames")
		("dest")
	;
	gameActions.add("show_firecone", newAction<ShowFirecone>)
		("sprite", false)
		("frames")
		("draw_distance")
	;
	gameActions.add("custom_event", newAction<RunCustomEvent>)
		("index", false)
	;
	gameActions.add("run_script", newAction<RunScript>)
		("name", false)
	;
	gameActions.add("repel", newAction<Repel>)
		("max_force")
		("max_distance")
		("min_force")
	;
	gameActions.add("damp", newAction<Damp>)
		("factor")
	;
	gameActions.add("apply_map_effect", newAction<ApplyMapEffect>)
		("effect", false)
	;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

ShootParticles::ShootParticles( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject)
, distribution(360.0), angleOffset(0.0)
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

ShootParticles::ShootParticles( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject)
{
	type = partTypeList.load(params[0]->toString());
	amount = params[1]->toInt(1);
	speed = params[2]->toDouble(0.0);
	speedVariation = params[3]->toDouble(0.0);
	motionInheritance = params[4]->toDouble(0.0);
	amountVariation = params[5]->toInt(0);
	distribution = Angle(params[6]->toDouble(360.0));
	angleOffset = AngleDiff(params[7]->toDouble(0.0));
	distanceOffset = params[7]->toDouble(0.0);
}

void ShootParticles::run( ActionParams const& params )
{
	if (type)
	{
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

UniformShootParticles::UniformShootParticles( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject)
, distribution(360.0), angleOffset(0.0)
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

UniformShootParticles::UniformShootParticles( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject)
{
	type = partTypeList.load(params[0]->toString());
	amount = params[1]->toInt(1);
	speed = params[2]->toDouble(0.0);
	speedVariation = params[3]->toDouble(0.0);
	motionInheritance = params[4]->toDouble(0.0);
	amountVariation = params[5]->toInt(0);
	distribution = Angle(params[6]->toDouble(360.0));
	angleOffset = AngleDiff(params[7]->toDouble(0.0));
	distanceOffset = params[7]->toDouble(0.0);
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

PutParticle::PutParticle( const vector< string >& params )
: BaseAction(0)
{
	type = NULL;
	x = 0;
	y = 0;
	xspd = 0;
	yspd = 0;
	if ( params.size() >= 1 )
	{
		type = partTypeList.load(params[0]);
	}
	if( params.size() >= 2 )
	{
		x = cast<float>(params[1]);
	}
	if( params.size() >= 3 )
	{
		y = cast<float>(params[2]);
	}
	if( params.size() >= 4 )
	{
		xspd = cast<float>(params[3]);
	}
	if( params.size() >= 5 )
	{
		yspd = cast<float>(params[4]);
	}
	if( params.size() >= 6 )
	{
		angle = Angle( cast<float>(params[5]) );
	}
}

PutParticle::PutParticle( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(0)
{
	type = partTypeList.load(params[0]->toString());
	x = params[1]->toDouble();
	y = params[2]->toDouble();
	xspd = params[3]->toDouble(0.0);
	yspd = params[4]->toDouble(0.0);
	angle = Angle(params[5]->toDouble(0.0));
}

void PutParticle::run( ActionParams const& params )
{
	if (type)
	{
		type->newParticle(type, Vec(x,y), Vec(xspd,yspd), 1, 0, angle);
	}
}

PutParticle::~PutParticle()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

CreateExplosion::CreateExplosion( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject)
{
	type = NULL;
	if ( params.size() >= 1 )
	{
		type = expTypeList.load(params[0]);
	}
}

CreateExplosion::CreateExplosion( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject)
{
	type = expTypeList.load(params[0]->toString());
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

Push::Push( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject | BaseAction::RequiresObject2)
{
	factor = 0;

	if ( params.size() >= 1 )
	{
		factor = cast<float>(params[0]);
	}
}

Push::Push( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject | BaseAction::RequiresObject2)
{
	factor = params[0]->toDouble(0.0);
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

Repel::Repel( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject | BaseAction::RequiresObject2)
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

Repel::Repel( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject | BaseAction::RequiresObject2)
{
	// Sensible defaults?
	maxForce = params[0]->toDouble(0.0);
	maxDistance = params[1]->toDouble(0.0);
	minForce = params[2]->toDouble(0.0);

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

Damp::Damp( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject2)
{
	factor = 0;

	if ( params.size() > 0 )
	{
		factor = cast<float>(params[0]);
	}
}

Damp::Damp( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject2)
{
	factor = params[0]->toDouble(0.0);
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

Damage::Damage( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject | BaseAction::RequiresObject2)
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

Damage::Damage( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject | BaseAction::RequiresObject2)
{
	m_damage = params[0]->toDouble(0.0);
	m_damageVariation = params[1]->toDouble(0.0);
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

Remove::Remove( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject)
{
}

Remove::Remove( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject)
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

PlaySound::PlaySound( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject)
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

PlaySound::PlaySound( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject)
{
#ifndef DEDSERV
	sound = soundList.load(params[0]->toString());
	loudness = params[1]->toDouble(100.0);
	pitch = params[2]->toDouble(1.0);
	pitchVariation = params[3]->toDouble(0.0);
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

PlayRandomSound::PlayRandomSound( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject)
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

PlayRandomSound::PlayRandomSound( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject)
{
#ifndef DEDSERV
	loudness = params[0]->toDouble(100.0);
	pitch = params[1]->toDouble(1.0);
	pitchVariation = params[2]->toDouble(0.0);

	OmfgScript::TokenBase* l = params[3];
	if(l->assertList())
	{
		const_foreach(s, l->toList())
		{
			if((*s)->assertString())
				sounds.push_back(soundList.load((*s)->toString()));
		}
	}
#endif
}

void PlayRandomSound::run( ActionParams const& params )
{
#ifndef DEDSERV
	if ( !sounds.empty() )
	{
		int sound = rndInt(sounds.size()); // TODO: Make an integer version of rnd()
		if ( sounds[sound] )
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

PlaySoundStatic::PlaySoundStatic( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject)
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

PlaySoundStatic::PlaySoundStatic( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject)
{
#ifndef DEDSERV
	sound = soundList.load(params[0]->toString());
	loudness = params[1]->toDouble(100.0);
	pitch = params[2]->toDouble(1.0);
	pitchVariation = params[3]->toDouble(0.0);
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

PlayGlobalSound::PlayGlobalSound( const vector< string >& params )
: BaseAction(0)
{
#ifndef DEDSERV
	sound = NULL;
	volume = 1;
	volumeVariation = 0;
	pitch = 1;
	pitchVariation = 0;

	if ( params.size() > 0 )
	{
		sound = sound1DList.load(params[0]);
	}
	if( params.size() > 1 )
	{
		volume = cast<float>(params[1]);
	}
	if( params.size() > 2 )
	{
		volumeVariation = cast<float>(params[2]);
	}
	if( params.size() > 3 )
	{
		pitch = cast<float>(params[3]);
	}
	if( params.size() > 4 )
	{
		pitchVariation = cast<float>(params[4]);
	}
#endif
}

PlayGlobalSound::PlayGlobalSound( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(0)
{
#ifndef DEDSERV
	sound = sound1DList.load(params[0]->toString());
	volume = params[1]->toDouble(1.0);
	volumeVariation = params[2]->toDouble(0.0);
	pitch = params[3]->toDouble(1.0);
	pitchVariation = params[4]->toDouble(0.0);
#endif
}

void PlayGlobalSound::run( ActionParams const& params )
{
#ifndef DEDSERV
	if (sound != NULL)
	{
		//allegro_message("moo");
		sound->play( volume, pitch, volumeVariation, pitchVariation );
	}
#endif
}

PlayGlobalSound::~PlayGlobalSound()
{
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

DelayFire::DelayFire( const vector< string >& params )
: BaseAction(BaseAction::RequiresWeapon)
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

DelayFire::DelayFire( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresWeapon)
{
	delayTime = params[0]->toInt(0);
	delayTimeVariation = params[1]->toInt(0);
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

ShowFirecone::ShowFirecone( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject)
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

ShowFirecone::ShowFirecone( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject)
{
#ifndef DEDSERV
	sprite = spriteList.load(params[0]->toString());
	frames = params[1]->toInt(0);
	drawDistance = params[2]->toDouble(0);
#endif
}

void ShowFirecone::run( ActionParams const& params )
{
#ifndef DEDSERV
	if( BaseWorm* w = dynamic_cast<BaseWorm*>(params.object) )
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

AddAngleSpeed::AddAngleSpeed( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject)
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

AddAngleSpeed::AddAngleSpeed( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject)
{
	speed = AngleDiff(params[0]->toDouble(0.0));
	speedVariation = AngleDiff(params[1]->toDouble(0.0));
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

SetAlphaFade::SetAlphaFade( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject)
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

SetAlphaFade::SetAlphaFade( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject)
{
	frames = params[0]->toInt(0);
	dest = params[1]->toInt(0);
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

RunCustomEvent::RunCustomEvent( const vector< string >& params )
: BaseAction(BaseAction::RequiresObject2)
{
	index = 0;
	if ( params.size() > 0 )
	{
		index = cast<int>(params[0]);
	}
}

RunCustomEvent::RunCustomEvent( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject2)
{
	index = params[0]->toInt(0);
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

RunScript::RunScript( vector< string > const& params )
: BaseAction(0)
, function(0)
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

RunScript::RunScript( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(0)
, function(0)
{
	std::string const& scriptName = params[0]->toString();
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

ApplyMapEffect::ApplyMapEffect( vector< string > const& params )
: BaseAction(BaseAction::RequiresObject)
{
	effect = NULL;
	if ( params.size() > 0 )
	{
		effect = levelEffectList.load(params[0]);
	}
}

ApplyMapEffect::ApplyMapEffect( vector<OmfgScript::TokenBase*> const& params )
: BaseAction(BaseAction::RequiresObject)
{
	effect = levelEffectList.load(params[0]->toString());
}

void ApplyMapEffect::run( ActionParams const& params )
{
	if ( effect )
		game.applyLevelEffect(effect, (int)params.object->pos.x, (int)params.object->pos.y);
}

ApplyMapEffect::~ApplyMapEffect()
{
}

