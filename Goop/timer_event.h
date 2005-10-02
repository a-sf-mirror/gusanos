#ifndef TIMER_EVENT_H
#define TIMER_EVENT_H

#include "events.h"
#include "math_func.h"

struct TimerEvent : public Event
{
	
	struct State
	{
		State(TimerEvent* event_, int count_) : count(count_), triggerCount(0), event(event_)
		{}
		
		bool tick()
		{
			if ( event->triggerTimes <= 0 || triggerCount < event->triggerTimes )
			{
				--count;
				if ( count < 0 )
				{
					reset();
					++triggerCount;
					return true;
				}
			}
			return false;
		}
	
		void reset()
		{
			count = event->delay + rndInt(event->delayVariation+1);
		}
		
		int count;
		int triggerCount;
		TimerEvent* event;
	};
	
	State* createState()
	{
		return new State( this, delay + rndInt(delayVariation+1) );
	}
	
	TimerEvent(int _delay, int _delayVariation, int _triggerTimes) : 
	delay(_delay), delayVariation(_delayVariation), triggerTimes(_triggerTimes)
	{}
	
	~TimerEvent() {};
	
	int delay;
	int delayVariation;
	int triggerTimes;
};

#endif
