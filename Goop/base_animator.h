#ifndef BASE_ANIMATOR_H
#define BASE_ANIMATOR_H
	
class BaseAnimator
{
	public:

	virtual ~BaseAnimator(); // <GLIP> Virtual dtor always needed for classes with virtual functions
	
	virtual int getFrame() = 0;
	virtual void tick() = 0;
};

#endif // _BASE_ANIMATOR_

