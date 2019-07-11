#ifndef USERSEARCH_H
#define USERSEARCH_H

#include <sstream>
#include "SearchEngine.h"
#include "UserSearchComponent.h"
using namespace std;

class UserSearch
{
private:
	enum ComponentOperationType { AND, OR, NOT, NO_OP };
	
	SearchEngine engine;
	vector<UserSearchComponent> components;
	vector<ComponentOperationType> operations;
	unordered_map<unsigned int, unsigned int> results;
	bool computed = false;
public:
	UserSearch(SearchEngine&);
	
	bool buildSearch(string&);
	unordered_map<unsigned int, unsigned int>& getResultSet();
private:
	ComponentOperationType getType(string&);
	string getTypeName(ComponentOperationType);
	bool processInput(istringstream&);
	void buildPhrase(istringstream&, vector<string>&);
	void performOperation(const unordered_map<unsigned int, unsigned int>&, ComponentOperationType);
	void autoCorrect(string&);
};

#endif /* USERSEARCH_h */
