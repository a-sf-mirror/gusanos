#include "player_options.h"
#include "gconsole.h"
#include "text.h"

using namespace std;

PlayerOptions::PlayerOptions()
{
	aimAcceleration = 0.1;
	aimFriction = 0.05;
	aimMaxSpeed = 1;
	viewportFollowFactor = 1;
	colour = 0;
}

void PlayerOptions::registerInConsole(int index)
{
	console.registerFloatVariable("P" + cast<string>(index) +"_AIM_ACCEL", &aimAcceleration, 0.5);
	console.registerFloatVariable("P" + cast<string>(index) +"_AIM_FRICTION", &aimFriction, 0.25);
	console.registerFloatVariable("P" + cast<string>(index) +"_AIM_SPEED", &aimMaxSpeed, 2.5);
	console.registerFloatVariable("P" + cast<string>(index) +"_VIEWPORT_FOLLOW", &viewportFollowFactor, 0.1);
}

