#ifndef INDEXINTERFACE_H
#define INDEXINTERFACE_H

#include <unordered_map>
#include <unordered_set>
#include <list>
using namespace std;

template<typename T, typename U, typename V, typename W = std::hash<U>, typename X = std::hash<V>>
class IndexInterface
{
public:
	virtual ~IndexInterface() {}
	virtual unsigned int getElements() = 0;
	virtual bool isEmpty() = 0;
	virtual void insert(const T&, const U&, const V&) = 0;
	virtual unordered_map<U, unordered_set<V, X>, W>& getValues(const T&) = 0;
	virtual unordered_set<V, X>& getValue(const T&, const U&);
	virtual bool isDefaultValueSet(unordered_map<U, unordered_set<V, X>, W>&) = 0;
	virtual void getMostCommonKeys(unsigned int, list<T>&) = 0;
	virtual void loadFromFile(string&) = 0;
	virtual void saveToFile(string&) = 0;
};

template<typename T, typename U, typename V, typename W, typename X>
unordered_set<V, X>& IndexInterface<T, U, V, W, X>::getValue(const T& key, const U& subKey)
{
	return getValues(key)[subKey];
}

#endif // INDEXINTERFACE_H
