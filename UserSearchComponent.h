#ifndef USERSEARCHCOMPONENT_H
#define USERSEARCHCOMPONENT_H

#include <string>
#include <vector>
#include "IndexInterface.h"
using namespace std;

class UserSearchComponent
{
private:
	vector<string> terms;
	unordered_map<unsigned int, unsigned int> results;
	bool computed = false;
public:
	UserSearchComponent(vector<string>&);
	
	void fillDocumentSet(IndexInterface<string, unsigned int, unsigned int>*, unordered_map<unsigned int, unordered_set<unsigned int>>&, unsigned int = 0);
	
	unordered_map<unsigned int, unsigned int>& getResultSet(IndexInterface<string, unsigned int, unsigned int>*);
};

#endif /* USERSEARCHCOMPONENT_H */
