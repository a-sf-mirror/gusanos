#ifndef PART_EVENTS_H
#define PART_EVENTS_H

#include "base_action.h"

#include <string>
#include <vector>

struct PartEvent
{
	public:
		
	PartEvent();
	~PartEvent();

	void addAction( const std::string& name, const std::vector<std::string>& params );
	void run( BaseObject *object );
	
	private:
	
	std::vector<BaseAction*> actions;
};

#endif  // _PART_EVENTS_H_
