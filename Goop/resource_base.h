#ifndef RESOURCE_BASE_H
#define RESOURCE_BASE_H

class ResourceBase
{

public:
	
	ResourceBase() { m_index = 0; }
	
	size_t getIndex()
	{
		return m_index;
	}
	
	void setIndex( size_t index )
	{
		m_index = index;
	}
	
	void think()
	{}
	
private:
	
	size_t m_index;

};

#endif