#include "vector_replicator.h"

#include <zoidcom.h>
#include "vec.h"

VectorReplicator::VectorReplicator(ZCom_ReplicatorSetup *_setup, Vec *_data) : 
	ZCom_ReplicatorBasic(cZCom_RType_boolp, _setup),
	m_ptr(_data)
{
	m_flags |= ZCOM_REPLICATOR_INITIALIZED;
}

bool VectorReplicator::checkState()
{
	bool s = (( m_cmp.x != m_ptr->x ) || ( m_cmp.y != m_ptr->y));
	m_cmp = *m_ptr;
	return s;
}

zU32 VectorReplicator::packData(ZCom_BitStream *_stream)
{
	_stream->addFloat(m_ptr->x,32);
	_stream->addFloat(m_ptr->y,32);
	return 1;
}

zU32 VectorReplicator::unpackData(ZCom_BitStream *_stream, bool _store, zU32 _estimated_time_sent)
{
	if (_store)
	{
		m_ptr->x = _stream->getFloat(32);
		m_ptr->y = _stream->getFloat(32);
	}
	else 
	{
		_stream->getFloat(32);
		_stream->getFloat(32);
	}
	return 1;
}

void* VectorReplicator::peekData()
{
	assert(getPeekStream());
	Vec *retVec = new Vec;
	retVec->x = getPeekStream()->getFloat(32);
	retVec->y = getPeekStream()->getFloat(32);
	
	peekDataStore(retVec);
	
	return (void*) retVec;
}

void VectorReplicator::clearPeekData()
{
	Vec *buf = (Vec*) peekDataRetrieve();
	if (buf) delete buf;
};

/*Vec VectorReplicator::getLastUpdate()
{
	return m_cmp;
}*/