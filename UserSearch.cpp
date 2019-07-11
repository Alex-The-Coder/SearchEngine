#include <algorithm>
#include <cctype>
#include <iostream>
#include "UserSearch.h"
#include "InputProcessor.h"
using namespace std;

UserSearch::UserSearch(SearchEngine& engine):
	engine(engine) {}

UserSearch::ComponentOperationType UserSearch::getType(string& str)
{
	string serial = str;
	InputProcessor::getInstance().lowerCase(serial);
    
	if (serial == "and")
	{
		return AND;
	}
	else if (serial == "or")
	{
		return OR;
	}
	else if (serial == "not")
	{
		return NOT;
	}
	else
	{
		return NO_OP;
	}
}

string UserSearch::getTypeName(UserSearch::ComponentOperationType type)
{
	switch (type)
	{
		case AND:
			return "AND";
		case OR:
			return "OR";
		case NOT:
			return "NOT";
		default:
			return "NO_OP";
	}
}

bool UserSearch::buildSearch(string& input)
{
	istringstream inputStream(input);
    
	return processInput(inputStream);
}

void UserSearch::buildPhrase(istringstream& inputStream, vector<string>& container)
{
	string read;
	int iterations = -1;
	
	while (inputStream >> read)
	{
		if (read.size() == 0)
		{
			continue;
		}
        
		iterations++;
		
		bool completeSearch = false;
		string parsed;
        
		if (read.size() == 1)
		{
			if (iterations < 1)
			{
				continue;
			}
			
			break;
		}
		else if (read[0] == '"' && read[read.size() - 1] == '"')
		{
			parsed = InputProcessor::getInstance().parse(read.substr(1, read.size() - 2));
			completeSearch = true;
		}
		else if (read[0] == '"')
		{
			parsed = InputProcessor::getInstance().parse(read.substr(1, read.size()));
		}
		else if (read[read.size() - 1] == '"')
		{
			parsed = InputProcessor::getInstance().parse(read.substr(0, read.size() - 1));
			completeSearch = true;
		}
		else
		{
			parsed = InputProcessor::getInstance().parse(read);
			completeSearch = iterations == 0;
		}
		
		if (parsed.size() > 0)
		{
			autoCorrect(parsed);
			container.emplace_back(parsed);
		}
		
		if (completeSearch)
		{
			break;
		}
	}
}

bool UserSearch::processInput(istringstream& inputStream)
{
	vector<string> terms;
	string read;
	
	buildPhrase(inputStream, terms);
    
	if (!terms.empty())
	{
		components.emplace_back(UserSearchComponent(terms));
		terms.clear();
		operations.emplace_back(OR);
	}
	else
	{
		cout << "INVALID SEARCH TERMS" << endl;
        
		return false;
	}
	
	while (inputStream.good())
	{
		buildPhrase(inputStream, terms);
        
		if (!terms.empty())
		{
			components.emplace_back(UserSearchComponent(terms));
			terms.clear();
		}
		else
		{
			break;
		}
		
		inputStream >> read;
		UserSearch::ComponentOperationType operation = getType(read);
        
		if (operation == NO_OP)
		{
			cout << "INCORRECT SEARCH TERMS: AFTER TWO SEARCH PHRASES THERE MUST BE A MERGING COMMAND (AND/OR/NOT)" << endl;
			
			components.clear();
			operations.clear();
			
			return false;
		}
		else
		{
			operations.emplace_back(operation);
		}
	}
	
	if (operations.size() != components.size())
	{
		cout << "INCORRECT SEARCH TERMS: AFTER TWO SEARCH PHRASES THERE MUST BE A MERGING COMMAND (AND/OR/NOT)" << endl;
		
		components.clear();
		operations.clear();
		
		return false;
	}
	
	return true;
}

void UserSearch::performOperation(const unordered_map<unsigned int, unsigned int>& right, UserSearch::ComponentOperationType operation)
{
	switch (operation)
	{
		case AND:
			for (auto iter = results.begin(); iter != results.end();)
			{
				if (right.count((*iter).first) == 0)
				{
					iter = results.erase(iter);
				}
				else
				{
					results.at((*iter).first) += (*iter).second;
					iter++;
				}
			}
            
			break;
		case OR:
			for (auto& element : right)
			{
				if (results.count(element.first) > 0)
				{
					results.at(element.first) += element.second;
				}
				else
				{
					results.emplace(element.first, element.second);
				}
			}
            
			break;
		case NOT:
			for (auto iter = results.begin(); iter != results.end();)
			{
				if (right.count((*iter).first) > 0)
				{
					iter = results.erase(iter);
				}
				else
				{
					iter++;
				}
			}
            
			break;
		default:
			break;
	}
}

unordered_map<unsigned int, unsigned int>& UserSearch::getResultSet()
{
	if (components.size() == 0 || computed)
	{
		return results;
	}
	
	unsigned int componentIndex = 0;
	unsigned int operationIndex = 0;
	
	while (componentIndex < components.size() && operationIndex < operations.size())
	{
		performOperation(components[componentIndex++].getResultSet(engine.getIndex()), operations[operationIndex++]);
	}
    
	computed = true;
	
	return results;
}

inline void withinDistance(string word, unordered_map<string, unsigned long>& loadTo, bool stem)
{
	char alphabet[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
	
	// Adding any one character (from the alphabet) anywhere in the word.
	for (unsigned int i = 0; i < word.size(); i++)
	{
		for (unsigned int j = 0; j < 26; j++)
		{
			string modified = word;
			modified.insert(i, 1, alphabet[j]);
            
			if (stem)
			{
				string parsed = InputProcessor::getInstance().parse(modified);
                
				if (parsed.size() == 0)
				{
					loadTo.emplace(modified, 0);
				}
				else
				{
					loadTo.emplace(parsed, 0);
				}
			}
			else
			{
				loadTo.emplace(modified, 0);
			}
		}
	}
	
	// Removing any one character from the word.
	if (word.size() > 1)
	{
		for (unsigned int i = 0; i < word.size(); i++)
		{
			string modified = word;
			modified.erase(i, 1);
			if (stem)
			{
				string parsed = InputProcessor::getInstance().parse(modified);
                
				if (parsed.size() == 0)
				{
					loadTo.emplace(modified, 0);
				}
				else
				{
					loadTo.emplace(parsed, 0);
				}
			}
			else
			{
				loadTo.emplace(modified, 0);
			}
		}
	}
	
	// Transposing (switching) the order of any two adjacent characters in a word.
	if (word.size() > 1)
	{
		for (unsigned int i = 0; i < word.size() - 1; i++)
		{
			string modified = word;
			char old = modified[i];
			modified[i] = modified[i + 1];
			modified[i + 1] = old;
            
			if (stem)
			{
				string parsed = InputProcessor::getInstance().parse(modified);
                
				if (parsed.size() == 0)
				{
					loadTo.emplace(modified, 0);
				}
				else
				{
					loadTo.emplace(parsed, 0);
				}
			}
			else
			{
				loadTo.emplace(modified, 0);
			}
		}
	}
	
	// Substituting any character in the word with another character.
	for (unsigned int i = 0; i < word.size(); i++)
	{
		for (unsigned int j = 0; j < 26; j++)
		{
			string modified = word;
			modified[i] = alphabet[j];
            
			if (stem)
			{
				string parsed = InputProcessor::getInstance().parse(modified);
                
				if (parsed.size() == 0)
				{
					loadTo.emplace(modified, 0);
				}
				else
				{
					loadTo.emplace(parsed, 0);
				}
			}
			else
			{
				loadTo.emplace(modified, 0);
			}
		}
	}
}

void UserSearch::autoCorrect(string& text)
{
	if (!InputProcessor::getInstance().isAlphaOnly(text))
	{
		return;
	}
    
	if (InputProcessor::getInstance().isStopWord(text))
	{
		return;
	}
    
	if (engine.getIndex()->getValues(text).size() > 0)
	{
		return;
	}
	
	unordered_map<string, unsigned long> distance1;
	unordered_map<string, unsigned long> distance2;
	withinDistance(text, distance1, true);
    
	for (auto& d1 : distance1)
	{
		if (InputProcessor::getInstance().isStopWord(d1.first))
		{
			engine.awaitResponse("Did you mean '" + d1.first + "' instead of '" + text + "'?", [&] (string& response)
			{
				if (response.size() > 0 && tolower(response[0]) == 'y')
				{
					text = d1.first;
				}
                
				return true;
			});
			
			return;
		}
        
		d1.second = engine.getIndex()->getValues(d1.first).size();
		withinDistance(d1.first, distance2, false);
	}
	
	for (auto& d2 : distance2)
	{
		if (InputProcessor::getInstance().isStopWord(d2.first))
		{
			engine.awaitResponse("Did you mean '" + d2.first + "' instead of '" + text + "'?", [&] (string& response)
			{
				if (response.size() > 0 && tolower(response[0]) == 'y')
				{
					text = d2.first;
				}
				return true;
			});
			
			return;
		}
        
		d2.second = engine.getIndex()->getValues(d2.first).size();
	}
	
	if (distance1.size() > 0)
	{
		string correctD1;
		unsigned long scoreD1 = 0;
		string correctD2;
		unsigned long scoreD2 = 0;
		
		for (auto& d1 : distance1)
		{
			if (d1.second > scoreD1)
			{
				correctD1 = d1.first;
				scoreD1 = d1.second;
			}
		}
		
		for (auto& d2 : distance2)
		{
			if (d2.second > scoreD2)
			{
				correctD2 = d2.first;
				scoreD2 = d2.second;
			}
		}
		
		if (scoreD2 >= 100 * scoreD1)
		{
			engine.awaitResponse("Did you mean '" + correctD2 + "' instead of '" + text + "'?", [&] (string& response)
			{
				if (response.size() > 0 && tolower(response[0]) == 'y')
				{
					text = correctD2;
				}
                
				return true;
			});
		}
		else
		{
			engine.awaitResponse("Did you mean '" + correctD1 + "' instead of '" + text + "'?", [&] (string& response)
			{
				if (response.size() > 0 && tolower(response[0]) == 'y')
				{
					text = correctD1;
				}
                
				return true;
			});
		}
	}
}
