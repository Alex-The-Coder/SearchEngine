#include "UserSearchComponent.h"
using namespace std;

UserSearchComponent::UserSearchComponent(vector<string>& terms):
terms(terms) {}

void UserSearchComponent::fillDocumentSet(IndexInterface<string, unsigned int, unsigned int>* index, unordered_map<unsigned int, unordered_set<unsigned int>>& expected, unsigned int termIndex)
{
	if (termIndex >= terms.size())
	{
		return;
	}
	
	if (expected.empty() && termIndex == 0)
	{
		if (terms.size() > 1)
		{
			unordered_map<unsigned int, unordered_set<unsigned int>> seeking;
			for (auto& element : index->getValues(terms[termIndex]))
			{
				for (const unsigned int location : element.second)
				{
					seeking[element.first].emplace(location + 1);
				}
			}
            
			fillDocumentSet(index, seeking, termIndex + 1);
		}
		else
		{
			for (auto& element : index->getValues(terms[termIndex]))
			{
				results[element.first]++;
			}
		}
	}
	else if (termIndex == terms.size() - 1)
	{
		for (auto& element : expected)
		{
			unordered_set<unsigned int> locations = index->getValue(terms[termIndex], element.first);
            
			for (const unsigned int& location : locations)
			{
				if (element.second.count(location) > 0)
				{
					results[element.first]++;
				}
			}
		}
	}
	else
	{
		unordered_map<unsigned int, unordered_set<unsigned int>> seeking;
        
		for (auto& element : expected)
		{
			unordered_set<unsigned int> locations = index->getValue(terms[termIndex], element.first);
            
			for (const unsigned int& location : element.second)
			{
				if (locations.count(location) > 0)
				{
					seeking[element.first].emplace(location + 1);
				}
			}
		}
        
		fillDocumentSet(index, seeking, termIndex + 1);
	}
}

unordered_map<unsigned int, unsigned int>& UserSearchComponent::getResultSet(IndexInterface<string, unsigned int, unsigned int>* index)
{
	if (!computed)
	{
		unordered_map<unsigned int, unordered_set<unsigned int>> empty;
		fillDocumentSet(index, empty);
		computed = true;
	}
	
	return results;
}
