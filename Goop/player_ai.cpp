#include "player_ai.h"
#include "player_options.h"
#include "base_player.h"
#include "worm.h"
#include "game.h"
#include "weapon.h"
#include <vector>
#include <list>
#include <cmath>

PlayerAI::PlayerAI()
	: m_pathSteps(100), m_thinkTime(0)
{
	m_options = new PlayerOptions;
	m_target = NULL;
	
	m_movingRight = false;
	m_movingLeft = false;
	m_shooting = false;
}

PlayerAI::~PlayerAI()
{
	delete m_options;
}


// getTarget assumes that a m_worm is a valid pointer, please dont call this if m_worm is null or sth
void PlayerAI::getTarget()
{
	m_target = NULL;
	float tmpDist = -1;
	ObjectsList::ColLayerIterator worm;
	for ( worm = game.objects.colLayerBegin(WORMS_COLLISION_LAYER); (bool)worm; ++worm)
	{
		BaseWorm *tmpWorm;
		if ( (*worm)->getOwner() != this )
		if ( ( tmpWorm = dynamic_cast<BaseWorm*>(*worm) ) && tmpWorm->isActive() )
		if ( ( m_worm->getPos() - (*worm)->getPos() ).length() < tmpDist || tmpDist < 0 )
		{
			m_target = *worm;
			tmpDist = ( m_worm->getPos() - (*worm)->getPos() ).length();
		}
	}
}

void PlayerAI::getPath()
{
	Vec pos = m_worm->getPos();		//AI position
	Vec target = m_worm->getPos();		//Target position
	
	//create "nodes" array
	for (int y = 0; y < 128; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			m_nodes[y][x] = 0;	//0 - unwalkable
			m_nodes[y][x] = 1;	//1 - walkable (uncalculated)
		}
	}
}

void PlayerAI::subThink()
{
	if ( m_thinkTime > 0) --m_thinkTime;
	else if ( m_worm )
	{
		m_thinkTime = thinkDelay;
		
		if ( !m_worm->isActive() )
			baseActionStart(RESPAWN);
		
		getTarget();
		if (!m_target)
			return;
		
		Vec pos = m_worm->getPos();		//AI position
		Vec target = m_target->getPos();	//Target position
		
		if ( m_worm->isActive() )
		if ( pos.x < target.x )
		{
			if ( m_movingLeft ) baseActionStop( LEFT );
			if ( !m_movingRight ) baseActionStart( RIGHT );
			m_movingLeft = false;
			m_movingRight = true;
		}else
		{
			if ( m_movingRight ) baseActionStop( RIGHT );
			if ( !m_movingLeft ) baseActionStart( LEFT );
			m_movingRight = false;
			m_movingLeft = true;
		}
		
		randomError = maxAimErrorOffset * midrnd();
		
		Vec tmpVec = ( target - pos );
		if ( tmpVec.x < 0 ) tmpVec.x *= -1;
		float angle2Target = tmpVec.getAngle();
		float wormAimAngle = m_worm->aimAngle + randomError;
		
		
		if ( wormAimAngle - maxInaccuracy < angle2Target && wormAimAngle + maxInaccuracy > angle2Target )
		{
			baseActionStart(FIRE);
		} else
			baseActionStop(FIRE);
	
	
		if ( ( m_worm->getCurrentWeapon()->reloading && ( rand() % 8 == 0 ) ) || rand() % 15 == 0)
		{
			m_worm->changeWeaponTo(m_worm->getWeaponIndexOffset( rand() % 50 ) );
		}
	
		// TODO: Make decent behaviour
		//jump
		if (rand() % 6 == 0)
			baseActionStart(JUMP);
	
		//rope
		if (rand() % 10 == 0)
			baseActionStart(NINJAROPE);
		else if (rand() % 96 == 0)
			baseActionStop(NINJAROPE);

	}
	if ( m_worm )
	if ( m_target )
	{
		Vec pos = m_worm->getPos();		//AI position
		Vec target = m_target->getPos();	//Target position
		
		Vec tmpVec = ( target - pos );
		if ( tmpVec.x < 0 ) tmpVec.x *= -1;
		float angle2Target = tmpVec.getAngle();
		float wormAimAngle = m_worm->aimAngle + randomError;
		
		if ( wormAimAngle < angle2Target ) m_worm->aimSpeed = 0.7;
		if ( wormAimAngle > angle2Target ) m_worm->aimSpeed = -0.7;
	}else
	{
		m_worm->aimSpeed = 0;
	}
}

