#ifndef BASE_ACTION_H
#define BASE_ACTION_H

class BaseObject;

class BaseAction
{
	public:
		
	BaseAction();

	virtual void run( BaseObject* obj ) = 0;
};

#endif  // _BASE_ACTION_H_
