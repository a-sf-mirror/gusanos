#ifndef PLAYER_OPTIONS_H
#define PLAYER_OPTIONS_H

#include <string>

struct PlayerOptions
{
	PlayerOptions();
	void registerInConsole(int index);
	
	float aimAcceleration;
	float aimFriction;
	float aimMaxSpeed;
	float viewportFollowFactor;
	float ropeAdjustSpeed;
	int colour;
	std::string name;
};

#endif  // _PLAYER_OPTIONS_H_
