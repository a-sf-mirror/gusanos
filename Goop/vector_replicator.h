#ifndef VECTOR_REPLICATOR_H
#define VECTOR_REPLICATOR_H

#include <zoidcom.h>

#include "vec.h"
#include "encoding.h"
#include <utility>


//#define COMPACT_FLOATS

class VectorReplicator : public ZCom_ReplicatorBasic
{
private:
	
	Vec*	m_ptr;
#ifdef COMPACT_FLOATS
	std::pair<long, long> m_old;
#else
	Vec	m_cmp;
#endif
	
public:

	VectorReplicator(ZCom_ReplicatorSetup *_setup, Vec *_data, Encoding::VectorEncoding& encoding_);
	
	// TODO: Implement this for safeness sake
	ZCom_Replicator* Duplicate(ZCom_Replicator *_dest) { return NULL; } 
	
	bool checkState();
	
	bool checkInitialState() { return true; }
	
	zU32 packData(ZCom_BitStream *_stream);
	
	zU32 unpackData(ZCom_BitStream *_stream, bool _store, zU32 _estimated_time_sent);
	
	void Process(eZCom_NodeRole _localrole, zU32 _simulation_time_passed) {}
	
	void* peekData();
	
	void clearPeekData();
	
	//const Vec& getLastUpdate();
	
private:
	Encoding::VectorEncoding& encoding;
};

#endif
