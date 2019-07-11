#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <cstdio>
#include <cstring>
#include <cwchar>
#include <stdexcept>
#include <sstream>
#include "AVLNode.h"
#include "IndexInterface.h"
using namespace std;

template<typename T, typename U, typename V, typename W = std::hash<U>, typename X = std::hash<V>>
class AVLTree : public IndexInterface<T, U, V, W, X>
{
private:
	unsigned int elements;
	AVLNode<T, U, V, W, X>* root;
	unordered_map<U, unordered_set<V, X>, W> empty;
public:
	AVLTree();
	AVLTree(const AVLTree<T, U, V, W, X>&);
	AVLTree<T, U, V, W, X>& operator=(const AVLTree<T, U, V, W, X>&);
	~AVLTree();
	
	bool isEmpty();
	unsigned int getElements();
	
	bool isDefaultValueSet(unordered_map<U, unordered_set<V, X>, W>&);
	
	void insert(const T&, const U&, const V&);
	
	unordered_map<U, unordered_set<V, X>, W>& getValues(const T&);
	void getMostCommonKeys(unsigned int, list<T>&);
	
	void saveToFile(string&);
	void loadFromFile(string&);
private:
	int height(AVLNode<T, U, V, W, X>*);
	int max(int, int);
	
	void rotateWithLeftChild(AVLNode<T, U, V, W, X>*&);
	void rotateWithRightChild(AVLNode<T, U, V, W, X>*&);
	void doubleWithLeftChild(AVLNode<T, U, V, W, X>*&);
	void doubleWithRightChild(AVLNode<T, U, V, W, X>*&);
	
	void clear(AVLNode<T, U, V, W, X>*&);
	
	bool insert(const T&, const U&, const V&, AVLNode<T, U, V, W, X>*&);
	
	unordered_map<U, unordered_set<V, X>, W>& getValues(const T&, AVLNode<T, U, V, W, X>*&);
	void getMostCommonKeys(unsigned int, list<T>&, AVLNode<T, U, V, W, X>*&, T&, unsigned long&);
	
	void saveToFile(ostringstream&, AVLNode<T, U, V, W, X>*&);
};

template<typename T, typename U, typename V, typename W, typename X>
AVLTree<T, U, V, W, X>::AVLTree()
{
	root = nullptr;
	elements = 0;
}

template<typename T, typename U, typename V, typename W, typename X>
AVLTree<T, U, V, W, X>::AVLTree(const AVLTree<T, U, V, W, X>& copy)
{
	root = copy.root == nullptr ? nullptr : copy.root->deepCopy();
	elements = copy.elements;
}

template<typename T, typename U, typename V, typename W, typename X>
AVLTree<T, U, V, W, X>& AVLTree<T, U, V, W, X>::operator=(const AVLTree<T, U, V, W, X>& copy)
{
	if (&copy != this)
	{
		clear(root);
		root = copy.root == nullptr ? nullptr : copy.root->deepCopy();
		elements = copy.elements;
	}
	
	return *this;
}

template<typename T, typename U, typename V, typename W, typename X>
AVLTree<T, U, V, W, X>::~AVLTree()
{
	clear(root);
	elements = 0;
}

template<typename T, typename U, typename V, typename W, typename X>
unsigned int AVLTree<T, U, V, W, X>::getElements()
{
	return elements;
}

template<typename T, typename U, typename V, typename W, typename X>
bool AVLTree<T, U, V, W, X>::isEmpty()
{
	return elements == 0;
}

template<typename T, typename U, typename V, typename W, typename X>
bool AVLTree<T, U, V, W, X>::isDefaultValueSet(unordered_map<U, unordered_set<V, X>, W>& check)
{
	return &check == &empty;
}

template<typename T, typename U, typename V, typename W, typename X>
void AVLTree<T, U, V, W, X>::clear(AVLNode<T, U, V, W, X>*& base)
{
	if (base == nullptr)
	{
		return;
	}
    
	if (base->getLeft() != nullptr)
	{
		clear(base->getLeft());
	}
    
	if (base->getRight() != nullptr)
	{
		clear(base->getRight());
	}
    
	delete base;
	base = nullptr;
	elements--;
}

template<typename T, typename U, typename V, typename W, typename X>
void AVLTree<T, U, V, W, X>::getMostCommonKeys(unsigned int count, list<T>& loadInto)
{
	count = std::min(count, elements);
    
	while (loadInto.size() < count)
	{
		T maxKey;
		unsigned long maxValue = 0;
		getMostCommonKeys(count, loadInto, root, maxKey, maxValue);
		
		loadInto.emplace_back(maxKey);
	}
}

template<typename T, typename U, typename V, typename W, typename X>
void AVLTree<T, U, V, W, X>::saveToFile(string& path)
{
	if (elements > 0)
	{
		ostringstream compute;
		compute << elements << '\n';
		saveToFile(compute, root);
		
		FILE* output = fopen(path.c_str(), "wb");
		fwrite(compute.str().c_str(), 1, ((long)compute.tellp()) + 1, output);
		fclose(output);
	}
}

template<typename T, typename U, typename V, typename W, typename X>
void AVLTree<T, U, V, W, X>::saveToFile(ostringstream& compute, AVLNode<T, U, V, W, X>*& node)
{
	if (node != nullptr)
	{
		saveToFile(compute, node->getLeft());
		compute << node->getKey() << '\n';
		compute << node->getValues().size() << '\n';
        
		for (auto& element : node->getValues())
		{
			compute << element.first << '\n';
			compute << element.second.size() << '\n';
            
			for (const V& value : element.second)
			{
				compute << value << '\n';
			}
		}
        
		saveToFile(compute, node->getRight());
	}
}

template<typename T, typename U, typename V, typename W, typename X>
void AVLTree<T, U, V, W, X>::loadFromFile(string& path)
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
		
		clear(root);
		
		processor >> elements;
		for (unsigned int i = 0; i < elements; i++)
		{
			T key;
			unsigned long size;
			processor >> key;
			processor >> size;
            
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
					
					insert(key, subKey, value, root);
				}
			}
		}
	}
}

template<typename T, typename U, typename V, typename W, typename X>
void AVLTree<T, U, V, W, X>::getMostCommonKeys(unsigned int count, list<T>& loadInto, AVLNode<T, U, V, W, X>*& node, T& maxKey, unsigned long& maxValue)
{
	if (node == nullptr)
	{
		return;
	}
	
	if (node->getValues().size() > maxValue)
	{
		const auto& pos = std::find(loadInto.begin(), loadInto.end(), node->getKey());
        
		if (pos == loadInto.end())
		{
			maxKey = node->getKey();
			maxValue = node->getValues().size();
		}
	}
	
	getMostCommonKeys(count, loadInto, node->getLeft(), maxKey, maxValue);
	getMostCommonKeys(count, loadInto, node->getRight(), maxKey, maxValue);
}

template<typename T, typename U, typename V, typename W, typename X>
void AVLTree<T, U, V, W, X>::insert(const T& key, const U& subKey, const V& value)
{
	if (insert(key, subKey, value, root))
	{
		elements++;
	}
}

template<typename T, typename U, typename V, typename W, typename X>
bool AVLTree<T, U, V, W, X>::insert(const T& key, const U& subKey, const V& value, AVLNode<T, U, V, W, X>*& node)
{
	bool increment = false;
    
	if (node == nullptr)
	{
		node = new AVLNode<T, U, V, W, X>(key, subKey, value, nullptr, nullptr);
		increment = true;
	}
	else if (key == node->getKey())
	{
		node->getValues()[subKey].emplace(value);
	}
	else if (key < node->getKey())
	{
		increment = insert(key, subKey, value, node->getLeft());
        
		if (height(node->getLeft()) - height(node->getRight()) == 2)
		{
			if (key < node->getLeft()->getKey())
			{
				rotateWithLeftChild(node);
			}
			else
			{
				doubleWithLeftChild(node);
			}
		}
	}
	else if (node->getKey() < key)
	{
		increment = insert(key, subKey, value, node->getRight());
        
		if (height(node->getRight()) - height(node->getLeft()) == 2)
		{
			if (node->getRight()->getKey() < key)
			{
				rotateWithRightChild(node);
			}
			else
			{
				doubleWithRightChild(node);
			}
		}
	}
	
	node->getHeight() = max(height(node->getLeft()), height(node->getRight())) + 1;
	return increment;
}

template<typename T, typename U, typename V, typename W, typename X>
unordered_map<U, unordered_set<V, X>, W>& AVLTree<T, U, V, W, X>::getValues(const T& key)
{
	return getValues(key, root);
}

template<typename T, typename U, typename V, typename W, typename X>
unordered_map<U, unordered_set<V, X>, W>& AVLTree<T, U, V, W, X>::getValues(const T& key, AVLNode<T, U, V, W, X>*& node)
{
	if (node != nullptr)
	{
		if (key == node->getKey())
		{
			return node->getValues();
		}
		else if (key < node->getKey())
		{
			return getValues(key, node->getLeft());
		}
		else if (node->getKey() < key)
		{
			return getValues(key, node->getRight());
		}
	}
	
	empty.clear();
	return empty;
}

template<typename T, typename U, typename V, typename W, typename X>
int AVLTree<T, U, V, W, X>::height(AVLNode<T, U, V, W, X>* node)
{
	if (node == nullptr)
	{
		return -1;
	}
    
	return node->getHeight();
}

template<typename T, typename U, typename V, typename W, typename X>
int AVLTree<T, U, V, W, X>::max(int lhs, int rhs)
{
	return lhs > rhs ? lhs : rhs;
}

template<typename T, typename U, typename V, typename W, typename X>
void AVLTree<T, U, V, W, X>::rotateWithLeftChild(AVLNode<T, U, V, W, X>*& alpha)
{
	AVLNode<T, U, V, W, X>* child = alpha->getLeft();
	alpha->getLeft() = child->getRight();
	child->getRight() = alpha;
	alpha->getHeight() = max(height(alpha->getLeft()), height(alpha->getRight())) + 1;
	child->getHeight() = max(height(child->getLeft()), alpha->getHeight()) + 1;
	alpha = child;
}

template<typename T, typename U, typename V, typename W, typename X>
void AVLTree<T, U, V, W, X>::rotateWithRightChild(AVLNode<T, U, V, W, X>*& alpha)
{
	AVLNode<T, U, V, W, X>* child = alpha->getRight();
	alpha->getRight() = child->getLeft();
	child->getLeft() = alpha;
	alpha->getHeight() = max(height(alpha->getLeft()), height(alpha->getRight())) + 1;
	child->getHeight() = max(height(child->getRight()), alpha->getHeight()) + 1;
	alpha = child;
}

template<typename T, typename U, typename V, typename W, typename X>
void AVLTree<T, U, V, W, X>::doubleWithLeftChild(AVLNode<T, U, V, W, X>*& alpha)
{
	rotateWithRightChild(alpha->getLeft());
	rotateWithLeftChild(alpha);
}

template<typename T, typename U, typename V, typename W, typename X>
void AVLTree<T, U, V, W, X>::doubleWithRightChild(AVLNode<T, U, V, W, X>*& alpha)
{
	rotateWithLeftChild(alpha->getRight());
	rotateWithRightChild(alpha);
}

#endif /* AVLTREE_H_ */
