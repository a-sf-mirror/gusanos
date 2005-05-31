#ifndef BASE_ACTION_H
#define BASE_ACTION_H

class BaseObject;
class BaseWorm;
class Weapon;

class BaseAction
{
	public:
		
	BaseAction();
	virtual ~BaseAction(); // <GLIP> Virtual dtor always needed for classes with virtual functions

	virtual void run( BaseObject* obj, BaseObject *object2, BaseWorm *worm, Weapon *weapon ) = 0;
};

#endif  // _BASE_ACTION_H_
