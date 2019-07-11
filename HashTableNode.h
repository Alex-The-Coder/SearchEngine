#ifndef HASHTABLENODE_H_
#define HASHTABLENODE_H_

#include <unordered_map>
#include <unordered_set>
using namespace std;

template<typename T, typename U, typename V, typename W = std::hash<U>, typename X = std::hash<V>>
class HashTableNode
{
private:
	long hash;
	T key;
	unordered_map<U, unordered_set<V, X>, W> values;
	HashTableNode<T, U, V, W, X>* next;
public:
	HashTableNode(const long& hash, const T& key, const U& subKey, const V& value, HashTableNode<T, U, V, W, X>* next);
	HashTableNode(const long& hash, const T& key, const unordered_map<U, unordered_set<V, X>, W>& values, HashTableNode<T, U, V, W, X>* next);

	long& getHash();
	T& getKey();
	unordered_map<U, unordered_set<V, X>, W>& getValues();
	HashTableNode<T, U, V, W, X>*& getNext();

	HashTableNode<T, U, V, W, X>* deepCopy();
};

template<typename T, typename U, typename V, typename W, typename X>
HashTableNode<T, U, V, W, X>::HashTableNode(const long& hash, const T& key, const U& subKey, const V& value, HashTableNode<T, U, V, W, X>* next):
	hash(hash), key(key), next(next)
{
    values[subKey].emplace(value);
}

template<typename T, typename U, typename V, typename W, typename X>
HashTableNode<T, U, V, W, X>::HashTableNode(const long& hash, const T& key, const unordered_map<U, unordered_set<V, X>, W>& values, HashTableNode<T, U, V, W, X>* next):
	hash(hash), key(key), values(values), next(next) {}

template<typename T, typename U, typename V, typename W, typename X>
long& HashTableNode<T, U, V, W, X>::getHash()
{
	return hash;
}

template<typename T, typename U, typename V, typename W, typename X>
T& HashTableNode<T, U, V, W, X>::getKey()
{
	return key;
}

template<typename T, typename U, typename V, typename W, typename X>
unordered_map<U, unordered_set<V, X>, W>& HashTableNode<T, U, V, W, X>::getValues()
{
	return values;
}

template<typename T, typename U, typename V, typename W, typename X>
HashTableNode<T, U, V, W, X>*& HashTableNode<T, U, V, W, X>::getNext()
{
	return next;
}

template<typename T, typename U, typename V, typename W, typename X>
HashTableNode<T, U, V, W, X>* HashTableNode<T, U, V, W, X>::deepCopy()
{
	HashTableNode<T, U, V, W, X>* deepNext = next == nullptr ? nullptr : next->deepCopy();

	return new HashTableNode<T, U, V, W, X>(hash, key, values, deepNext);
}

#endif /* HASHTABLENODE_H_ */
