#ifndef GUSANOS_HASH_TABLE_H
#define GUSANOS_HASH_TABLE_H

#include <utility>
#include <string>
#include <cstddef>

typedef unsigned long Hash;

template<class T>
struct hash
{
	Hash operator()(T const& key) const
	{
		return Hash(key);
	}
};

template<>
struct hash<unsigned long>
{
	Hash operator()(unsigned long key) const
	{
		return key;
	}
};

template<>
struct hash<std::string>
{
	Hash operator()(std::string const& key) const
	{
		size_t left = key.size();
		char const* d = key.data();
		Hash h = 0;
		for(; left > sizeof(Hash);
			left -= sizeof(Hash),
			d += sizeof(Hash))
		{
			h ^= *(Hash *)d;
		}
		
		// Do the rest byte by byte
		Hash rest = 0;
		for(; left > 0; --left, ++d)
		{
			rest = (rest << 8) | *d;
		}
		
		h ^= rest;
		return h;
	}
};

template<class KeyT, class ValueT>
struct HashIndex : public std::pair<const KeyT, ValueT>
{
	HashIndex(KeyT const& key, ValueT const& value)
	: std::pair<const KeyT, ValueT>(key, value), next(0)
	{
	}
	
	HashIndex(KeyT const& key)
	: std::pair<const KeyT, ValueT>(key, ValueT()), next(0)
	{
	}
	
	HashIndex* next;
};

extern unsigned long tableSizes[18];

template<class KeyT, class ValueT, class HashFunc = hash<KeyT> >
class HashTable
{
public:
	typedef HashIndex<KeyT, ValueT> IndexT;
	
	friend struct iterator;
	
	// TODO: iterator where you can actually iterate
	struct iterator
	{
		friend class HashTable;
		
		std::pair<const KeyT, ValueT>* operator->()
		{
			return slot;
		}
		
		bool operator==(iterator const& b)
		{
			return slot == b.slot;
		}
		
		bool operator!=(iterator const& b)
		{
			return slot != b.slot;
		}
		
	private:
		iterator(IndexT* slot_)
		: slot(slot_)
		{
		}
		
		IndexT* slot;
	};
	
	HashTable(size_t initialSizeOrder = 3)
	: count(0), sizeOrder(initialSizeOrder)
	{
		if(sizeOrder > 17)
			sizeOrder = 17;

		indexCount = tableSizes[sizeOrder];
		index = new IndexT*[indexCount];
		
		for(size_t i = 0; i < indexCount; ++i)
			index[i] = 0;
	}
	
	~HashTable()
	{
		for(size_t i = 0; i < indexCount; ++i)
		{
			for(IndexT* slot = index[i]; slot;)
			{
				IndexT* next = slot->next;
				delete slot;
				slot = next;
			}
		}
		delete [] index;
	}
	
	iterator find(KeyT const& key)
	{
		IndexT* index = getIndex(key);
		if(!index)
			return end();

		do
		{
			if(index->first == key)
				return iterator(index);
			index = index->next;
		} while(index);
		
		return end();
	}

	ValueT& operator[](KeyT const& key)
	{
		IndexT* index = getIndex(key);
		if(!index)
			return insert(key)->second;

		do
		{
			if(index->first == key)
				return index->second;
			index = index->next;
		} while(index);
		
		return insert(key)->second;
	}
	
	iterator insert(KeyT const& key)
	{
		if(++count > indexCount)
			enlarge();
			
		size_t idx = hashFunc(key) % indexCount;
		IndexT* slot = index[idx];
		
		// Check if key already exists
		for(IndexT* i = slot; i; i = i->next)
		{
			if(i->first == key)
				return iterator(i); // Return iterator to old key
		}
		
		// Key didn't exist, add it
		IndexT* newIndex = new IndexT(key);
		newIndex->next = slot;
		index[idx] = newIndex;
		return iterator(newIndex);
	}
	
	iterator end()
	{
		return iterator(0);
	}
	
private:
	void enlarge()
	{
		if(sizeOrder >= 17)
			return; // Max size already
			
		IndexT** oldTable = index;
		Hash     oldCount = indexCount;
		
		indexCount = tableSizes[++sizeOrder];
		index = new IndexT*[indexCount];
		
		for(size_t i = 0; i < indexCount; ++i)
			index[i] = 0;
		
		// Move old entries to the new table
		for(size_t i = 0; i < oldCount; ++i)
		{
			for(IndexT* slot = oldTable[i]; slot;)
			{
				IndexT* next = slot->next;
				size_t idx = hashFunc(slot->first) % indexCount;
				IndexT* dest = index[idx];
				slot->next = dest;
				index[idx] = slot;
				
				slot = next;
			}
		}
		
		delete [] oldTable;
	}
	
	IndexT* getIndex(KeyT const& key) const
	{
		return index[hashFunc(key) % indexCount];
	}
	
	IndexT**   index;
	size_t     count;
	Hash       indexCount;
	size_t     sizeOrder;
	hash<KeyT> hashFunc;
};

#endif //GUSANOS_HASH_TABLE_H
