#ifndef BASE_ACTION_H
#define BASE_ACTION_H

class BaseObject;
class BaseWorm;
class Weapon;

struct ActionParams
{
	ActionParams( BaseObject *object_, BaseObject *object2_, BaseWorm *worm_, Weapon *weapon_ ) :
			object(object_), object2(object2_), worm(worm_), weapon(weapon_)
	{
	}
	BaseObject* object;
	BaseObject *object2;
	BaseWorm *worm;
	Weapon *weapon;
};

class BaseAction
{
public:
	enum Flags
	{
		RequiresObject = (1<<0),
		RequiresObject2 = (1<<1),
		RequiresWeapon = (1<<2),
	};
		
	BaseAction(int flags_);
	virtual ~BaseAction(); // <GLIP> Virtual dtor always needed for classes with virtual functions

	virtual void run( ActionParams const& params ) = 0;
	
	int flags;
};

#endif  // _BASE_ACTION_H_
