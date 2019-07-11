#ifndef AVLNODE_H_
#define AVLNODE_H_

#include <unordered_map>
#include <unordered_set>
using namespace std;

template<typename T, typename U, typename V, typename W = std::hash<U>, typename X = std::hash<V>>
class AVLNode
{
private:
	AVLNode<T, U, V, W, X>* left;
	AVLNode<T, U, V, W, X>* right;
	int height;
	T key;
	unordered_map<U, unordered_set<V, X>, W> values;
public:
	AVLNode(const T& key, const U& subKey, const V& value, AVLNode<T, U, V, W, X>* left, AVLNode<T, U, V, W, X>* right, int height = 0);
	
	T& getKey();
	unordered_map<U, unordered_set<V, X>, W>& getValues();
	int& getHeight();
	AVLNode<T, U, V, W, X>*& getLeft();
	AVLNode<T, U, V, W, X>*& getRight();
	
	AVLNode<T, U, V, W, X>* deepCopy();
private:
	AVLNode(T& key, unordered_map<U, unordered_set<V, X>, W>& values, AVLNode<T, U, V, W, X>* left, AVLNode<T, U, V, W, X>* right, int height = 0);
};

template<typename T, typename U, typename V, typename W, typename X>
AVLNode<T, U, V, W, X>::AVLNode(const T& key, const U& subKey, const V& value, AVLNode<T, U, V, W, X>* left, AVLNode<T, U, V, W, X>* right, int height):
	key(key), left(left), right(right), height(height)
{
	values[subKey].emplace(value);
}

template<typename T, typename U, typename V, typename W, typename X>
AVLNode<T, U, V, W, X>::AVLNode(T& key, unordered_map<U, unordered_set<V, X>, W>& values, AVLNode<T, U, V, W, X>* left, AVLNode<T, U, V, W, X>* right, int height):
	key(key), values(values), left(left), right(right), height(height) {}

template<typename T, typename U, typename V, typename W, typename X>
T& AVLNode<T, U, V, W, X>::getKey()
{
	return key;
}

template<typename T, typename U, typename V, typename W, typename X>
unordered_map<U, unordered_set<V, X>, W>& AVLNode<T, U, V, W, X>::getValues()
{
	return values;
}

template<typename T, typename U, typename V, typename W, typename X>
AVLNode<T, U, V, W, X>*& AVLNode<T, U, V, W, X>::getLeft()
{
	return left;
}

template<typename T, typename U, typename V, typename W, typename X>
AVLNode<T, U, V, W, X>*& AVLNode<T, U, V, W, X>::getRight()
{
	return right;
}

template<typename T, typename U, typename V, typename W, typename X>
int& AVLNode<T, U, V, W, X>::getHeight()
{
	return height;
}

template<typename T, typename U, typename V, typename W, typename X>
AVLNode<T, U, V, W, X>* AVLNode<T, U, V, W, X>::deepCopy()
{
	AVLNode<T, U, V, W, X>* leftDeep = left == nullptr ? nullptr : left->deepCopy();
	AVLNode<T, U, V, W, X>* rightDeep = right == nullptr ? nullptr : right->deepCopy();
	
	AVLNode<T, U, V, W, X>* deep = new AVLNode<T, U, V, W, X>(key, values, leftDeep, rightDeep);
	
	return deep;
}

#endif /* AVLNODE_H_ */
