#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "base_object.h"
#include "exp_type.h"
#include "vec.h"
#include <vector>

class Sprite;
class BaseAnimator;
class BasePlayer;

class Explosion : public BaseObject
{
	public:
		
		Explosion(ExpType* type, const Vec& _pos = Vec(0,0), BasePlayer* owner = NULL);

		void draw(BITMAP* where,int xOff, int yOff);
		void think();
	
	private:
	
		ExpType* m_type;
		
		int m_time;
		int m_timeout;
		
		float m_fadeSpeed;
		float m_alpha;
		SpriteSet* m_sprite;
		BaseAnimator* m_animator;
	
		bool justCreated;
};

#endif  // _EXPLOSION_H_
