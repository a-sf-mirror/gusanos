#ifndef BASE_ANIMATOR_H
#define BASE_ANIMATOR_H
	
class BaseAnimator
{
	public:

	//virtual ~BaseAnimator() = 0;
	
	virtual int getFrame() = 0;
	virtual void tick() = 0;
};

#endif // _BASE_ANIMATOR_

