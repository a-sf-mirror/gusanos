#ifndef GAME_ACTIONS_H
#define GAME_ACTIONS_H

#include "base_action.h"
#include "angle.h"
#include "lua/types.h"
#include <string>
#include <vector>

class PartType;
class ExpType;
class Sound;
class SpriteSet;
class BaseObject;
class LevelEffect;

void registerGameActions();

BaseAction* shootParticles( const std::vector< std::string >& params );

class ShootParticles : public BaseAction
{
	public:

	ShootParticles( const std::vector< std::string >& params );
	~ShootParticles();

	void run( ActionParams const& params );

	private:

	PartType *type;
	
	int amount;
	int amountVariation;
	
	float motionInheritance;
	float speed;
	float speedVariation;
	Angle distribution;
	AngleDiff angleOffset;
	float distanceOffset;
};

BaseAction* uniformShootParticles( const std::vector< std::string >& params );

class UniformShootParticles : public BaseAction
{
	public:

		UniformShootParticles( const std::vector< std::string >& params );
		~UniformShootParticles();

		void run( ActionParams const& params );

	private:

		PartType *type;
	
		int amount;
		int amountVariation;
	
		float motionInheritance;
		float speed;
		float speedVariation;
		Angle distribution;
		AngleDiff angleOffset;
		float distanceOffset;
};

BaseAction* createExplosion( const std::vector< std::string >& params );

class CreateExplosion : public BaseAction
{
	public:

		CreateExplosion( const std::vector< std::string >& params );
		~CreateExplosion();

		void run( ActionParams const& params );

	private:

		ExpType *type;
};

BaseAction* push( const std::vector< std::string >& params );

class Push : public BaseAction
{
	public:

		Push( const std::vector< std::string >& params );
		~Push();

		void run( ActionParams const& params );

	private:
		float factor;
};

BaseAction* repel( const std::vector< std::string >& params );

class Repel : public BaseAction
{
	public:

		Repel( const std::vector< std::string >& params );
		~Repel();

		void run( ActionParams const& params );

	private:
		float maxForce;
		float minForce;
		float maxDistance;
		
		// Precomputed constants
		float maxDistanceSqr;
		float forceDiffScaled;
};

BaseAction* damp( const std::vector< std::string >& params );

class Damp : public BaseAction
{
	public:

		Damp( const std::vector< std::string >& params );
		~Damp();

		void run( ActionParams const& params );

	private:
		float factor;
};

BaseAction* damage( const std::vector< std::string >& params );

class Damage : public BaseAction
{
	public:

		Damage( const std::vector< std::string >& params );
		~Damage();

		void run( ActionParams const& params );

	private:
		float m_damage;
		float m_damageVariation;
};

BaseAction* remove( const std::vector< std::string >& params );

class Remove : public BaseAction
{
	public:

	Remove( const std::vector< std::string >& params );
	~Remove();

	void run( ActionParams const& params );

	private:
	
};

BaseAction* playSound( const std::vector< std::string >& params );

class PlaySound : public BaseAction
{
	public:

	PlaySound( const std::vector< std::string >& params );
	~PlaySound();

	void run( ActionParams const& params );

	private:
		
	Sound *sound;
	float pitch;
	float pitchVariation;
	float loudness;
	
};

BaseAction* playRandomSound( const std::vector< std::string >& params );

class PlayRandomSound : public BaseAction
{
	public:

		PlayRandomSound( const std::vector< std::string >& params );
		~PlayRandomSound();

		void run( ActionParams const& params );

	private:
		
		std::vector<Sound*> sounds;
		float pitch;
		float pitchVariation;
		float loudness;
	
};

BaseAction* playSoundStatic( const std::vector< std::string >& params );

class PlaySoundStatic : public BaseAction
{
	public:

	PlaySoundStatic( const std::vector< std::string >& params );
	~PlaySoundStatic();

	void run( ActionParams const& params );

	private:
		
	Sound *sound;
	float pitch;
	float pitchVariation;
	float loudness;
	
};

BaseAction* delayFire( const std::vector< std::string >& params );

class DelayFire : public BaseAction
{
	public:

	DelayFire( const std::vector< std::string >& params );
	~DelayFire();

	void run( ActionParams const& params );

	private:
	
	int delayTime;
	int delayTimeVariation;
};

BaseAction* showFirecone( const std::vector< std::string >& params );

class ShowFirecone : public BaseAction
{
	public:

		ShowFirecone( const std::vector< std::string >& params );
		~ShowFirecone();

		void run( ActionParams const& params );

	private:
	
		int frames;
		float drawDistance;
		SpriteSet* sprite;
};

BaseAction* addAngleSpeed( const std::vector< std::string >& params );

class AddAngleSpeed : public BaseAction
{
	public:

	AddAngleSpeed( const std::vector< std::string >& params );
	~AddAngleSpeed();

	void run( ActionParams const& params );

	private:
	
	AngleDiff speed;
	AngleDiff speedVariation;
};

BaseAction* setAlphaFade( const std::vector< std::string >& params );

class SetAlphaFade : public BaseAction
{
	public:

		SetAlphaFade( const std::vector< std::string >& params );
		~SetAlphaFade();

		void run( ActionParams const& params );

	private:
	
		int frames;
		int dest;
};

BaseAction* runCustomEvent( const std::vector< std::string >& params );

class RunCustomEvent : public BaseAction
{
	public:

		RunCustomEvent( const std::vector< std::string >& params );
		~RunCustomEvent();

		void run( ActionParams const& params );

	private:
	
		size_t index;
};

BaseAction* runScript( std::vector< std::string > const& params );

class RunScript : public BaseAction
{
public:

	RunScript( std::vector< std::string > const& params );
	~RunScript();

	void run( ActionParams const& params );

private:

	LuaReference function;
};

BaseAction* applyMapEffect( const std::vector< std::string >& params );

class ApplyMapEffect : public BaseAction
{
	public:

		ApplyMapEffect( const std::vector< std::string >& params );
		~ApplyMapEffect();

		void run( ActionParams const& params );

	private:
	
		LevelEffect* effect;
};


#endif  // _GAME_ACTIONS_H_
