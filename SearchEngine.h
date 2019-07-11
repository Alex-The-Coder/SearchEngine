#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "IndexInterface.h"
#include <string>
#include <list>
#include <ctime>
using namespace std;

struct DocumentInfo
{
	unsigned int id;
	string path;
	string title;
	time_t date;
};

//Thunderbolt Legal Search Engine
class SearchEngine
{
private:
	IndexInterface<string, unsigned int, unsigned int>* index = nullptr;
	unordered_map<unsigned int, DocumentInfo> documents;
	unordered_map<string, function<void(list<string>&)>> commands;
	bool shutDown = false;
public:
	SearchEngine();
	
	IndexInterface<string, unsigned int, unsigned int>* getIndex();
	void awaitResponse(string, function<bool(string&)>);
	void awaitResponseReference(string&, function<bool(string&)>);
private:
	void expandDirectory(string&, long, function<void(string&)>);
	void loadFile(const string&, const string&);
	
	void loadDocumentCache(string&);
	void saveDocumentCache(string&);
};

#endif /* SEARCHENGINE_H */
