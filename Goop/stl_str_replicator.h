#ifndef STL_STR_REPLICATOR_H
#define STL_STR_REPLICATOR_H

#include <zoidcom.h>
#include <string>

class STLStringReplicator : public ZCom_ReplicatorBasic
{
	private:
	
		std::string*	m_ptr;
		std::string	m_cmp;
	
	public:

		STLStringReplicator(ZCom_ReplicatorSetup *_setup, std::string *_data);
	
	// TODO: Implement this for safeness sake
		ZCom_Replicator* Duplicate(ZCom_Replicator *_dest) { return NULL; } 
	
		bool checkState();
	
		bool checkInitialState() { return true; }
	
		zU32 packData(ZCom_BitStream *_stream);
	
		zU32 unpackData(ZCom_BitStream *_stream, bool _store, zU32 _estimated_time_sent);
	
		void Process(eZCom_NodeRole _localrole, zU32 _simulation_time_passed) {};
	
		void* peekData();
	
		void clearPeekData();
};

#endif