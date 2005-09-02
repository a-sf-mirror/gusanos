#include "player_options.h"
#include "gconsole.h"
#include <boost/bind.hpp>
#include "text.h"
#include <cmath>

using namespace std;

PlayerOptions::PlayerOptions()
{
	aimAcceleration = AngleDiff(0.1);
	//aimFriction = 0.05;
	aimFriction = pow(0.89, 0.7);
	aimMaxSpeed = AngleDiff(1);
	viewportFollowFactor = 1;
	ropeAdjustSpeed = 0.5;
	colour = 0;
	name = "GusPlayer";
	m_nameChanged = false;
}

void PlayerOptions::registerInConsole(int index)
{
	console.registerVariables()
		("P" + cast<string>(index) +"_AIM_ACCEL", &aimAcceleration, AngleDiff(0.17))
		("P" + cast<string>(index) +"_AIM_FRICTION", &aimFriction, pow(0.89, 0.7))
		("P" + cast<string>(index) +"_AIM_SPEED", &aimMaxSpeed, AngleDiff(1.7))
		("P" + cast<string>(index) +"_VIEWPORT_FOLLOW", &viewportFollowFactor, 0.1)
		("P" + cast<string>(index) +"_ROPE_ADJUST_SPEED", &ropeAdjustSpeed, 0.5)
		("P" + cast<string>(index) +"_NAME", &name, "GusPlayer",
		 boost::bind( &PlayerOptions::nameChange, this ) )
	;
}

void PlayerOptions::nameChange()
{
	m_nameChanged = true;
}

bool PlayerOptions::nameChanged()
{
	if ( m_nameChanged )
	{
		m_nameChanged = false;
		return true;
	}
	return false;
}