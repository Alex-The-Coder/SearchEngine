#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <cstdio>
#include <cstring>
#include <cwchar>
#include <stdexcept>
#include <sstream>
#include "IndexInterface.h"
#include "HashTableNode.h"
using namespace std;

struct LongTuple
{
    long first;
    long second;
};

template<typename T, typename U, typename V, typename W = std::hash<U>, typename X = std::hash<V>, typename Y = std::hash<T>>
class HashTable: public IndexInterface<T, U, V, W, X>
{
private:
	unsigned int totalBuckets;
	unsigned int count;
	HashTableNode<T, U, V, W, X>** buckets;
    unordered_map<U, unordered_set<V, X>, W> empty;
public:
	HashTable();
	HashTable(const HashTable<T, U, V, W, X, Y>&);
	HashTable<T, U, V, W, X, Y>& operator=(const HashTable<T, U, V, W, X, Y>&);
	~HashTable();

	unsigned int getElements();
	bool isEmpty();
    bool isDefaultValueSet(unordered_map<U, unordered_set<V, X>, W>&);

	void insert(const T&, const U&, const V&);
    unordered_map<U, unordered_set<V, X>, W>& getValues(const T&);
    void getMostCommonKeys(unsigned int, list<T>&);
	void saveToFile(string&);
	void loadFromFile(string&);
private:
	HashTableNode<T, U, V, W, X>** createBuckets(unsigned int);
	void resizeTable();
    bool insert(const long&, const T&, const U&, const V&, HashTableNode<T, U, V, W, X>*&);
	void insert(const long&, const T&, const unordered_map<U, unordered_set<V, X>, W>&, HashTableNode<T, U, V, W, X>*&);
	void clear(HashTableNode<T, U, V, W, X>*&);
    LongTuple computeBucket(const T&);
};

template<typename T, typename U, typename V, typename W, typename X, typename Y>
HashTable<T, U, V, W, X, Y>::HashTable()
{
	totalBuckets = 256;
	count = 0;
	buckets = createBuckets(totalBuckets);
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
HashTable<T, U, V, W, X, Y>::HashTable(const HashTable<T, U, V, W, X, Y>& copy)
{
	totalBuckets = copy.totalBuckets;
	count = copy.count;
	buckets = createBuckets(totalBuckets);
    
	for (unsigned int i = 0; i < totalBuckets; i++)
	{
		buckets[i] = copy.buckets[i]->deepCopy();
	}
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
HashTable<T, U, V, W, X, Y>& HashTable<T, U, V, W, X, Y>::operator=(const HashTable<T, U, V, W, X, Y>& copy)
{
	if (&copy != this)
	{
		for (unsigned int i = 0; i < totalBuckets; i++)
		{
			clear(buckets[i]);
		}
        
		delete[] buckets;

		totalBuckets = copy.totalBuckets;
		count = copy.count;
		buckets = createBuckets(totalBuckets);
        
		for (unsigned int i = 0; i < totalBuckets; i++)
		{
			buckets[i] = copy.buckets[i]->deepCopy();
		}
	}

	return *this;
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
HashTable<T, U, V, W, X, Y>::~HashTable()
{
	for (unsigned int i = 0; i < totalBuckets; i++)
	{
		clear(buckets[i]);
	}
    
	delete[] buckets;
	count = 0;
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
unsigned int HashTable<T, U, V, W, X, Y>::getElements()
{
	return count;
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
bool HashTable<T, U, V, W, X, Y>::isEmpty()
{
	return count == 0;
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
bool HashTable<T, U, V, W, X, Y>::isDefaultValueSet(unordered_map<U, unordered_set<V, X>, W>& check)
{
    return &check == &empty;
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
inline LongTuple HashTable<T, U, V, W, X, Y>::computeBucket(const T& key)
{
    LongTuple lt;
    lt.first = Y{}(key);
    lt.second = lt.first & (totalBuckets - 1);
    
    return lt;
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
void HashTable<T, U, V, W, X, Y>::clear(HashTableNode<T, U, V, W, X>*& bucket)
{
	if (bucket == nullptr)
	{
		return;
	}
    
	clear(bucket->getNext());
	delete bucket;
	bucket = nullptr;
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
HashTableNode<T, U, V, W, X>** HashTable<T, U, V, W, X, Y>::createBuckets(unsigned int total)
{
	HashTableNode<T, U, V, W, X>** created = new HashTableNode<T, U, V, W, X>*[total];
    
	for (unsigned int i = 0; i < total; i++)
	{
		created[i] = nullptr;
	}

	return created;
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
void HashTable<T, U, V, W, X, Y>::resizeTable()
{
	unsigned int resized = totalBuckets * 8;//* 2 * 2;
	HashTableNode<T, U, V, W, X>** created = createBuckets(resized);
    
	for (unsigned int i = 0; i < totalBuckets; i++)
	{
		HashTableNode<T, U, V, W, X>* node = buckets[i];
        
		while (node != nullptr)
		{
			HashTableNode<T, U, V, W, X>*& newBucket = created[node->getHash() & (resized - 1)];
			insert(node->getHash(), node->getKey(), node->getValues(), newBucket);
			node = node->getNext();
		}
        
		clear(buckets[i]);
	}
    
	delete[] buckets;

	totalBuckets = resized;
	buckets = created;
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
unordered_map<U, unordered_set<V, X>, W>& HashTable<T, U, V, W, X, Y>::getValues(const T& key)
{
	HashTableNode<T, U, V, W, X>* bucket = buckets[computeBucket(key).second];

	while (bucket != nullptr)
	{
		if (bucket->getKey() == key)
		{
			return bucket->getValues();
		}
		else
		{
			bucket = bucket->getNext();
		}
	}
    
    empty.clear();
	return empty;
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
void HashTable<T, U, V, W, X, Y>::getMostCommonKeys(unsigned int count, list<T>& loadInto)
{
    count = std::min(count, this->count);
    
    while (loadInto.size() < count)
    {
        T maxKey;
        unsigned long maxValue = 0;
        
        for (unsigned int i = 0; i < totalBuckets; i++)
        {
            for (HashTableNode<T, U, V, W, X>* bucket = buckets[i]; bucket != nullptr; bucket = bucket->getNext())
            {
                if (maxValue == 0 || bucket->getValues().size() > maxValue)
                {
                    const auto& pos = std::find(loadInto.begin(), loadInto.end(), bucket->getKey());
                    
                    if (pos == loadInto.end())
                    {
                        maxKey = bucket->getKey();
                        maxValue = bucket->getValues().size();
                    }
                }
            }
        }
        
        loadInto.emplace_back(maxKey);
    }
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
void HashTable<T, U, V, W, X, Y>::saveToFile(string& path)
{
	if (count > 0)
	{
		ostringstream compute;
		compute << count << '\n';
        
		for (unsigned int i = 0; i < totalBuckets; i++)
		{
			for (HashTableNode<T, U, V, W, X>* bucket = buckets[i]; bucket != nullptr; bucket = bucket->getNext())
			{
				compute << bucket->getKey() << '\n';
				compute << bucket->getValues().size() << '\n';
                
				for (auto& element : bucket->getValues())
				{
					compute << element.first << '\n';
					compute << element.second.size() << '\n';
                    
					for (const V& value : element.second)
					{
						compute << value << '\n';
					}
				}
			}
		}
		
		FILE* output = fopen(path.c_str(), "wb");
		fwrite(compute.str().c_str(), 1, ((long)compute.tellp()) + 1, output);
		fclose(output);
	}
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
void HashTable<T, U, V, W, X, Y>::loadFromFile(string& path)
{
	FILE* input = fopen(path.c_str(), "rb");
	
	if (input)
	{
		char* buffer = nullptr;
		long fileLength = 0;
		fseek(input, 0, SEEK_END);
		fileLength = ftell(input);
		
		fseek(input, 0, SEEK_SET);
		buffer = (char*) malloc(fileLength * sizeof(char));
		fread(buffer, sizeof(char), fileLength, input);
		fclose(input);
		
		istringstream processor(buffer);
		free(buffer);
		
		for (unsigned int i = 0; i < totalBuckets; i++)
		{
			clear(buckets[i]);
		}
        
		delete[] buckets;
		
		processor >> count;
		totalBuckets = 256;
        
		while (count >= (totalBuckets * 0.8))
		{
			totalBuckets *= 8;
		}
        
		buckets = createBuckets(totalBuckets);
        
		for (unsigned int i = 0; i < count; i++)
		{
			T key;
			unsigned long size;
			processor >> key;
			processor >> size;
			LongTuple computed = computeBucket(key);
			HashTableNode<T, U, V, W, X>*& bucket = buckets[computed.second];
            
			for (unsigned long ii = 0; ii < size; ii++)
			{
				U subKey;
				unsigned long subSize;
				processor >> subKey;
				processor >> subSize;
                
				for (unsigned long iii = 0; iii < subSize; iii++)
				{
					V value;
					processor >> value;
					
					insert(computed.first, key, subKey, value, bucket);
				}
			}
		}
	}
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
void HashTable<T, U, V, W, X, Y>::insert(const T& key, const U& subKey, const V& value)
{
    LongTuple computed = computeBucket(key);
	HashTableNode<T, U, V, W, X>*& bucket = buckets[computed.second];
    
    if (insert(computed.first, key, subKey, value, bucket))
    {
        count++;
    }

	if (count >= (totalBuckets * 0.8))
	{
		resizeTable();
	}
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
bool HashTable<T, U, V, W, X, Y>::insert(const long& hash, const T& key, const U& subKey, const V& value, HashTableNode<T, U, V, W, X>*& bucketStart)
{
	if (bucketStart == nullptr)
	{
		bucketStart = new HashTableNode<T, U, V, W, X>(hash, key, subKey, value, nullptr);
        return true;
	}
	else if (bucketStart->getKey() == key)
	{
		bucketStart->getValues()[subKey].emplace(value);
        return false;
	}
	else
	{
        return insert(hash, key, subKey, value, bucketStart->getNext());
	}
}

template<typename T, typename U, typename V, typename W, typename X, typename Y>
void HashTable<T, U, V, W, X, Y>::insert(const long& hash, const T& key, const unordered_map<U, unordered_set<V, X>, W>& values, HashTableNode<T, U, V, W, X>*& bucketStart)
{
	if (bucketStart == nullptr)
	{
		bucketStart = new HashTableNode<T, U, V, W, X>(hash, key, values, nullptr);
	}
	else if (bucketStart->getKey() == key)
	{
        for (auto& entry : values)
        {
            bucketStart->getValues()[entry.first].insert(entry.second.begin(), entry.second.end());
        }
	}
	else
	{
		insert(hash, key, values, bucketStart->getNext());
	}
}

#endif /* HASHTABLE_H_ */
