#ifndef BASE_ANIMATOR_H
#define BASE_ANIMATOR_H

//#include <boost/variant.hpp>
	
class BaseAnimator
{
public:
	BaseAnimator(int initFrame = 0);
	virtual ~BaseAnimator(); // <GLIP> Virtual dtor always needed for classes with virtual functions

	int getFrame() const
	{
		return m_frame;
	}
	
	virtual void tick() = 0;
	virtual void reset() = 0;
	void freeze(int ticks);
	
protected:
	
	int freezeTicks;
	int m_frame;
};

/*
struct AnimatorTick
    : public boost::static_visitor<>
{
public:

    void operator()(int & i) const
    {
		return;
    }

	template<class T>
    void operator()(T& anim) const
    {
		anim.tick();
    }

};

struct AnimatorFreeze
    : public boost::static_visitor<>
{
public:

	AnimatorFreeze(int f_)
	: f(f_)
	{
	}

    void operator()(int & i) const
    {
		return;
    }

	template<class T>
    void operator()(T& anim) const
    {
		anim.freeze(f);
    }

	int f;
};


struct AnimatorGetFrame
    : public boost::static_visitor<int>
{
public:

    int operator()(int & i) const
    {
		return 0;
    }

	template<class T>
    int operator()(T const& anim) const
    {
		return anim.getFrame();
    }

};*/

#endif // _BASE_ANIMATOR_

