#ifndef GAME_ACTIONS_H
#define GAME_ACTIONS_H

#include "base_action.h"
#include <string>
#include <vector>

class PartType;
class Sound;
class BaseObject;

void registerGameActions();

BaseAction* shootParticles( const std::vector< std::string >& params );

class ShootParticles : public BaseAction
{
	public:

	ShootParticles( const std::vector< std::string >& params );
	~ShootParticles();

	void run( BaseObject *object );

	private:

	PartType *type;
	
	int amount;
	int amountVariation;
	
	float motionInheritance;
	float speed;
	float speedVariation;
	float distribution;
	float angleOffset;
};

BaseAction* remove( const std::vector< std::string >& params );

class Remove : public BaseAction
{
	public:

	Remove( const std::vector< std::string >& params );
	~Remove();

	void run( BaseObject *object );

	private:
	
};

BaseAction* playSound( const std::vector< std::string >& params );

class PlaySound : public BaseAction
{
	public:

	PlaySound( const std::vector< std::string >& params );
	~PlaySound();

	void run( BaseObject *object );

	private:
		
	Sound *sound;
	float volume;
	float pitch;
	float volumeVariation;
	float pitchVariation;
	float loudness;
	
};

#endif  // _GAME_ACTIONS_H_
