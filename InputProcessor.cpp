#include <fstream>
#include "InputProcessor.h"
#include "Porter2_Stemmer.h"
using namespace std;

const InputProcessor* InputProcessor::INSTANCE = new InputProcessor();

const InputProcessor& InputProcessor::getInstance()
{
	return *INSTANCE;
}

InputProcessor::InputProcessor()
{
	ifstream input("stopwords.txt");
	string word;
    
	while (input >> word)
	{
		word.erase(std::remove_if(word.begin(), word.end(), [] (char c)
		{
			return !(c == '\'' || (c >= 'a' && c <= 'z'));
		}), word.end());
        
		if (word.size() == 0)
		{
			continue;
		}
        
		stopWords.emplace(word);
        
		word.erase(std::remove_if(word.begin(), word.end(), [] (char c)
		{
			return c == '\'';
		}), word.end());
        
		stopWords.emplace(word);
	}
}

InputProcessor::InputProcessor(const InputProcessor& copy)
{
    stopWords = copy.stopWords;
}

InputProcessor::InputProcessor(InputProcessor&& copy)
{
    stopWords = copy.stopWords;
}

InputProcessor& InputProcessor::operator=(const InputProcessor& copy)
{
	return *this;
}

InputProcessor& InputProcessor::operator=(InputProcessor&& copy)
{
	return *this;
}

string InputProcessor::parse(const string& base) const
{
	string processed = base;
    
	if (processed.size() == 0)
	{
		return processed;
	}

	lowerCase(processed);
    
	processed.erase(std::remove_if(processed.begin(), processed.end(), [] (char c)
	{
		return !(c == '\'' || (c >= 'a' && c <= 'z'));
	}), processed.end());
    
	if (processed.size() == 0)
	{
		return processed;
	}

	if (stopWords.count(processed) > 0)
	{
		return "";
	}

	stemString(processed);

	return processed;
}

inline void InputProcessor::stemString(string& text) const
{
	string key = text;
    
	if (stemCache.find(key) != stemCache.end())
	{
		text = stemCache.at(text);
		return;
	}
    
	Porter2Stemmer::stem(text);
	stemCache.emplace(key, text);
}

string& InputProcessor::stripHTML(string& text) const
{
	for (unsigned int i = 0; i < text.size(); i++)
	{
		if (text[i] == '<')
		{
			unsigned int j = i;
            
			while (text[j] != '>' && j < text.size())
			{
				j++;
			}
            
			text.erase(i, j - i + 1);
			i--;
		}
	}
	
	return text;
}

string& InputProcessor::lowerCase(string& text) const
{
    for (unsigned int i = 0; i < text.size(); i++)
    {
        text[i] = tolower(text[i]);
    }
	
	return text;
}

bool InputProcessor::isAlphaOnly(const string& text) const
{
    for (unsigned int i = 0; i < text.size(); i++)
    {
        if (!isalpha(text[i]))
        {
            return false;
        }
    }
    
    return true;
}

bool InputProcessor::isStopWord(const string& text) const
{
	string processed = text;
    
	if (processed.size() == 0)
	{
		return false;
	}
	
	lowerCase(processed);
	
	return stopWords.count(processed) > 0;
}
