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
	ropeAdjustSpeed = 0.5;
	colour = 0;
}

void PlayerOptions::registerInConsole(int index)
{
	console.registerFloatVariable("P" + cast<string>(index) +"_AIM_ACCEL", &aimAcceleration, 0.17);
	console.registerFloatVariable("P" + cast<string>(index) +"_AIM_FRICTION", &aimFriction, 0.08);
	console.registerFloatVariable("P" + cast<string>(index) +"_AIM_SPEED", &aimMaxSpeed, 1.7);
	console.registerFloatVariable("P" + cast<string>(index) +"_VIEWPORT_FOLLOW", &viewportFollowFactor, 0.1);
	console.registerFloatVariable("P" + cast<string>(index) +"_ROPE_ADJUST_SPEED", &ropeAdjustSpeed, 0.5);
}

