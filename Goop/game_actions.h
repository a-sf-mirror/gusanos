#ifndef GAME_ACTIONS_H
#define GAME_ACTIONS_H

#include "base_action.h"
#include <string>
#include <vector>

class PartType;
class ExpType;
class Sound;
class SpriteSet;
class BaseObject;

void registerGameActions();

BaseAction* shootParticles( const std::vector< std::string >& params );

class ShootParticles : public BaseAction
{
	public:

	ShootParticles( const std::vector< std::string >& params );
	~ShootParticles();

	void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

	private:

	PartType *type;
	
	int amount;
	int amountVariation;
	
	float motionInheritance;
	float speed;
	float speedVariation;
	float distribution;
	float angleOffset;
	float distanceOffset;
};

BaseAction* createExplosion( const std::vector< std::string >& params );

class CreateExplosion : public BaseAction
{
	public:

		CreateExplosion( const std::vector< std::string >& params );
		~CreateExplosion();

		void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

	private:

		ExpType *type;
};

BaseAction* push( const std::vector< std::string >& params );

class Push : public BaseAction
{
	public:

		Push( const std::vector< std::string >& params );
		~Push();

		void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

	private:
		float factor;
};

BaseAction* damage( const std::vector< std::string >& params );

class Damage : public BaseAction
{
	public:

		Damage( const std::vector< std::string >& params );
		~Damage();

		void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

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

	void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

	private:
	
};

BaseAction* playSound( const std::vector< std::string >& params );

class PlaySound : public BaseAction
{
	public:

	PlaySound( const std::vector< std::string >& params );
	~PlaySound();

	void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

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

		void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

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

	void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

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

	void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

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

		void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

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

	void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

	private:
	
	float speed;
	float speedVariation;
};

BaseAction* setAlphaFade( const std::vector< std::string >& params );

class SetAlphaFade : public BaseAction
{
	public:

		SetAlphaFade( const std::vector< std::string >& params );
		~SetAlphaFade();

		void run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon );

	private:
	
		int frames;
		int dest;
};

#endif  // _GAME_ACTIONS_H_
