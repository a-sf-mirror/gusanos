#ifndef PLAYER_OPTIONS_H
#define PLAYER_OPTIONS_H

#include <string>
#include <list>
#include "angle.h"
#include <allegro.h>

// TODO: Move these to blitters/<somewhere>
inline int universalColor(int r, int g, int b)
{
	return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

inline int universalToLocalColor(int c)
{
	return makecol(
		(c >> 16) & 0xFF,
		(c >> 8) & 0xFF,
		(c) & 0xFF);
}

struct PlayerOptions
{
	PlayerOptions::PlayerOptions(std::string const& name_ = "GusPlayer");
	void registerInConsole(int index);
	
	AngleDiff aimAcceleration;
	float aimFriction;
	AngleDiff aimMaxSpeed;
	float viewportFollowFactor;
	float ropeAdjustSpeed;
	int colour;
	std::string name;
	
	std::string setColour(std::list<std::string> const& args);
	
	void clearChangeFlags();
	
	bool nameChanged(); // Returns true when the name option has been changed
	bool colorChanged();
	
	void changeName(std::string const& name_);
	
private:
	void nameChange(); // for internal use only

	bool m_nameChanged;
	bool m_colorChanged;
};

#endif  // _PLAYER_OPTIONS_H_
