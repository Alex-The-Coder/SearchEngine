#ifndef INPUTPROCESSOR_H_
#define INPUTPROCESSOR_H_

#include <string>
#include <unordered_set>
#include <unordered_map>
using namespace std;

class InputProcessor
{
private:
	static const InputProcessor* INSTANCE;
	mutable unordered_map<string, string> stemCache;
	unordered_set<string> stopWords;
public:
	static const InputProcessor& getInstance();
	
	string parse(const string& base) const;
	string& stripHTML(string&) const;
	string& lowerCase(string&) const;
	bool isAlphaOnly(const string&) const;
	bool isStopWord(const string&) const;
private:
	InputProcessor();
	InputProcessor(const InputProcessor&);
	InputProcessor(InputProcessor&&);
	InputProcessor& operator=(const InputProcessor&);
	InputProcessor& operator=(InputProcessor&&);
	
	void stemString(string&) const;
};

#endif /* INPUTPROCESSOR_H_ */
