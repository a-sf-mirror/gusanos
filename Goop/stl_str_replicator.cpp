#include "stl_str_replicator.h"

#include <zoidcom.h>
#include <string>

using namespace std;

STLStringReplicator::STLStringReplicator(ZCom_ReplicatorSetup *_setup, string *_data) : 
	ZCom_ReplicatorBasic(cZCom_RType_boolp, _setup),
	m_ptr(_data)
{
	m_flags |= ZCOM_REPLICATOR_INITIALIZED;
}

bool STLStringReplicator::checkState()
{
	bool s = ( m_cmp != *m_ptr );
	m_cmp = *m_ptr;
	return s;
}

zU32 STLStringReplicator::packData(ZCom_BitStream *_stream)
{
	_stream->addString( m_ptr->c_str() );
	return 1;
}

zU32 STLStringReplicator::unpackData(ZCom_BitStream *_stream, bool _store, zU32 _estimated_time_sent)
{
	if (_store)
	{
		*m_ptr = _stream->getStringStatic();
	}
	else 
	{
		_stream->getStringStatic();
	}
	return 1;
}

void* STLStringReplicator::peekData()
{
	assert(getPeekStream());
	string* retString = new string;
	*retString = getPeekStream()->getStringStatic();
	
	peekDataStore(retString);
	
	return (void*) retString;
}

void STLStringReplicator::clearPeekData()
{
	string *buf = (string*) peekDataRetrieve();
	if (buf) delete buf;
};

