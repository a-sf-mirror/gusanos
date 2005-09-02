#ifndef PLAYER_OPTIONS_H
#define PLAYER_OPTIONS_H

#include <string>
#include "angle.h"

struct PlayerOptions
{
	PlayerOptions();
	void registerInConsole(int index);
	
	AngleDiff aimAcceleration;
	float aimFriction;
	AngleDiff aimMaxSpeed;
	float viewportFollowFactor;
	float ropeAdjustSpeed;
	int colour;
	std::string name;
	
	void nameChange(); // for internal use only
	
	bool nameChanged(); // Returns true when the name option has been changed
	
	private:

	bool m_nameChanged;
};

#endif  // _PLAYER_OPTIONS_H_
