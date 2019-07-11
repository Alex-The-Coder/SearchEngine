#include <stdlib.h>
#include <dirent.h>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <fstream>
#include "SearchEngine.h"
#include "UserSearch.h"
#include "InputProcessor.h"
#include "AVLTree.h"
#include "HashTable.h"
#include "json.hpp"
using namespace std;
using nlohmann::json;

SearchEngine::SearchEngine()
{
	DocumentInfo info;
	commands["index"] = [&] (list<string>& args)
	{
		if (args.size() >= 3)
		{
			string& persistentPath = *std::next(args.begin(), 1);
			string& documentCache = *std::next(args.begin(), 2);
            
			if (InputProcessor::getInstance().lowerCase(args.front()) == "save")
			{
				if (index == nullptr)
				{
					cout << "Index does not exist to save!" << endl;
					return;
				}
                
				cout << "Saving index to " << persistentPath << "..." << endl;
				index->saveToFile(persistentPath);
				saveDocumentCache(documentCache);
				cout << "Saved index to disk" << endl;
			}
			else
			{
				if (index != nullptr)
				{
					cout << "An index already exists!" << endl;
					return;
				}
                
				bool avl = (InputProcessor::getInstance().lowerCase(args.front()) == "true");
				cout << "Loading index from " << persistentPath << "..." << endl;
                
				if (avl)
				{
					index = new AVLTree<string, unsigned int, unsigned int>();
				}
				else
				{
					index = new HashTable<string, unsigned int, unsigned int>();
				}
                
				index->loadFromFile(persistentPath);
				cout << "LOADED INDEX" << endl;
				loadDocumentCache(documentCache);
				cout << "Loaded index from disk" << endl;
			}
		}
		else if (args.size() >= 1)
		{
			if (index != nullptr)
			{
				cout << "An index already exists!" << endl;
				return;
			}
            
			bool avl = (InputProcessor::getInstance().lowerCase(args.front()) == "true");
			cout << "Creating new index..." << endl;
			if (avl)
			{
				index = new AVLTree<string, unsigned int, unsigned int>();
			}
			else
			{
				index = new HashTable<string, unsigned int, unsigned int>();
			}
            
			cout << "Created new index" << endl;
		}
		else
		{
			cout << "Index Status: " << (index == nullptr ? "Does not Exist" : "Exists") << endl;
		}
	};
	commands["clearindex"] = [&] (list<string>& args)
	{
		if (index == nullptr)
		{
			cout << "Index does not exist to clear!" << endl;
			return;
		}
        
		delete index;
		index = nullptr;
		documents.clear();
	};
	commands["addindex"] = [&] (list<string>& args)
	{
		if (index == nullptr)
		{
			cout << "Index does not exist to modify!" << endl;
			return;
		}
		
		if (args.size() >= 2)
		{
			loadFile(args.front(), *std::next(args.begin(), 1));
		}
		else if (args.size() == 1)
		{
			string& path = args.front();
			expandDirectory(path, -1, [&] (string& file)
			{
				loadFile(path, file);
			});
		}
		else
		{
			cout << "Invalid arguments for command 'addindex'" << endl;
		}
	};
	commands["statistics"] = [&] (list<string>& args)
	{
		if (index == nullptr)
		{
			cout << "Index does not exist" << endl;
			return;
		}
		
		cout << "Total Opinions Indexed: " << documents.size() << endl;
        
		if (documents.size() > 0 && index->getElements() > 0)
		{
			cout << "Average Words Per Opinion: " << (index->getElements() / documents.size()) << endl;
			list<string> topWords;
			index->getMostCommonKeys(50, topWords);
			cout << "Top " << topWords.size() << " Most Frequent Words:" << endl;
			unsigned int place = 1;
            
			for (string& word : topWords)
			{
				cout << place++ << ". " << word << endl;
			}
		}
	};
	commands["stats"] = commands["statistics"];
	commands["search"] = [&] (list<string>& args)
	{
		if (index == nullptr)
		{
			cout << "Index does not exist!" << endl;
			return;
		}
		
		UserSearch search(*this);
		awaitResponse("Enter search terms:", [&] (string& input)
		{
			if (search.buildSearch(input))
			{
				unordered_map<unsigned int, unsigned int>& results = search.getResultSet();
				
				if (results.size() == 0)
				{
					cout << "No results found for those search terms." << endl;
					return true;
				}
				
				vector<DocumentInfo> resultInfo(results.size());
				unsigned int index = 0;
                
				for (auto& element : results)
				{
					resultInfo[index++] = documents.at(element.first);
				}
                
				std::sort(resultInfo.begin(), resultInfo.end(), [&] (DocumentInfo& a, DocumentInfo& b)
				{
					time_t now = std::time(NULL);
					long aElapsed = (now - a.date) / 60 / 60 / 24 / 30; // Months since A creation
					long bElapsed = (now - b.date) / 60 / 60 / 24 / 30; // Months since B creation
					
					// Exponential decay relevance
					double aRelevance = results.at(a.id) * std::pow(0.75, aElapsed);
					double bRelevance = results.at(b.id) * std::pow(0.75, bElapsed);
					
					return aRelevance > bRelevance;
				});
				
				cout << "Page 1 out of " << ((resultInfo.size() / 15) + 1) << endl;
                
				for (unsigned int i = 0 * 15; i < resultInfo.size() && i < 15; i++)
				{
					cout << (i + 1) << ". " << resultInfo[i].title << endl;
				}
                
				cout << "Enter a number to access a document, or page <#> to view another page. Enter 'exit' to close results." << endl;
                
				awaitResponse("", [&] (string& input)
				{
					if (InputProcessor::getInstance().lowerCase(input) == "exit")
					{
						return true;
					}
					
					istringstream parseInput(input);
					string read;
					parseInput >> read;
                    
					if (read == "page")
					{
						unsigned int page;
						parseInput >> page;
                        
						if ((page - 1) * 15 < resultInfo.size())
						{
							cout << "Page " << page << " out of " << ((resultInfo.size() / 15) + 1) << endl;
                            
							for (unsigned int i = (page - 1) * 15; i < resultInfo.size() && i < (page * 15); i++)
							{
								cout << (i + 1) << ". " << resultInfo[i].title << endl;
							}
                            
							cout << "Enter a number to access a document, or page <#> to view another page. Enter 'exit' to close results." << endl;
						}
						else
						{
							cout << "Requested page out of range!" << endl;
						}
					}
					else
					{
						unsigned int i = stoi(read) - 1;
                        
						if (i < resultInfo.size())
						{
							DocumentInfo info = resultInfo[i];
							ifstream f(resultInfo[i].path);
							json j;
							f >> j;
							
							string html;
							string lawbox;
							string plain;
                            
							if (!j["html"].empty())
							{
								j["html"].get_to(html);
								InputProcessor::getInstance().stripHTML(html);
							}
                            
							if (!j["html_lawbox"].empty())
							{
								j["html_lawbox"].get_to(lawbox);
								InputProcessor::getInstance().stripHTML(lawbox);
							}
                            
							if (!j["plain_text"].empty())
							{
								j["plain_text"].get_to(plain);
								InputProcessor::getInstance().stripHTML(plain);
							}
							
							cout << "Title: " << info.title << endl;
							cout << "Year: " << ((info.date / 60 / 60 / 24 / 365) + 1970) << endl;
                            
							awaitResponse("Options: HTML/Lawbox/PlainText/Back", [&] (string& input)
							{
								if (InputProcessor::getInstance().lowerCase(input) == "back")
								{
									return true;
								}
								else
								{
									if (input == "html")
									{
										if (html.size() > 0)
										{
											cout << html << endl;
										}
										else
										{
											cout << "No HTML for this document." << endl;
										}
									}
									else if (input == "lawbox")
									{
										if (lawbox.size() > 0)
										{
											cout << lawbox << endl;
										}
										else
										{
											cout << "No Lawbox for this document." << endl;
										}
									}
									else if (input == "plaintext")
									{
										if (plain.size() > 0)
										{
											cout << plain << endl;
										}
										else
										{
											cout << "No PlainText for this document." << endl;
										}
									}
									else
									{
										cout << "Invalid option!" << endl;
									}
                                    
									return false;
								}
							});
							
							cout << "Enter a number to access a document, or page <#> to view another page. Enter 'exit' to close results." << endl;
						}
						else
						{
							cout << "Invalid document!" << endl;
						}
					}
					
					return false;
				});
				
				return true;
			}
			
			return false;
		});
	};
	commands["stop"] = [&] (list<string>& args)
	{
		shutDown = true;
	};
	
	cout << "Launching Thunderbolt Legal Search Engine..." << endl;
	while (!shutDown)
	{
		cout << "Enter a command:" << endl;
		string read;
		getline(cin, read);
		istringstream processor(read);
		
		string cmd;
		list<string> args;
		
		if (processor >> cmd)
		{
			InputProcessor::getInstance().lowerCase(cmd);
            
			if (commands.count(cmd))
			{
				string arg;
                
				while (processor >> arg)
				{
					args.emplace_back(arg);
				}
				
				commands.at(cmd)(args);
			}
		}
	}
}

IndexInterface<string, unsigned int, unsigned int>* SearchEngine::getIndex()
{
	return index;
}

void SearchEngine::awaitResponse(string userPrompt, function<bool(string&)> consumer)
{
	awaitResponseReference(userPrompt, consumer);
}

void SearchEngine::awaitResponseReference(string& userPrompt, function<bool(string&)> consumer)
{
	bool responded = false;
    
	while (!responded)
	{
		cout << userPrompt << endl;
		string read;
		getline(cin, read);
		responded = consumer(read);
	}
}

void SearchEngine::expandDirectory(string& path, long maxFiles, function<void(string&)> consumer)
{
	list<string> expanded;
	string extension = ".json";
	DIR *dir;
	struct dirent *ent;
	
	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL && (maxFiles == -1 || expanded.size() < maxFiles))
		{
			unsigned long len = strlen(ent->d_name);
			if (ent->d_type == DT_REG &&
				len > extension.length() &&
				strcmp(ent->d_name + len - extension.length(), extension.c_str()) == 0)
			{
				expanded.emplace_back(ent->d_name);
			}
		}
        
		closedir(dir);
		
		for (string& file : expanded)
		{
			consumer(file);
		}
	}
	else
	{
		throw invalid_argument("Provided path \"" + path + "\" could not be opened");
	}
}

inline string parseCaseTitle(string& absoluteURL)
{
	istringstream sections(absoluteURL);
	string title;
	
	string r1;
    
	for (int i = 0; i < 4; i++)
	{
		getline(sections, r1, '/');
	}
    
	istringstream split(r1);
	
	string r2;
    
	while (getline(split, r2, '-'))
	{
		if (r2 == "v")
		{
			r2 = "v.";
		}
		else if (r2.size() > 0)
		{
			r2[0] = toupper(r2[0]);
            
			if (r2[r2.size() - 1] == '/')
			{
				r2 = r2.substr(0, r2.size() - 1);
			}
		}
        
		title += (r2 + ' ');
	}
	
	return title.substr(0, title.size() - 1);
}

inline time_t parseDate(string& date)
{
	std::tm parsed;
	memset(&parsed, 0, sizeof(parsed));
	strptime(date.c_str(), "%Y-%m-%dT%H:%M:%SZ", &parsed);
    
	return mktime(&parsed);
}

inline unsigned int parseSection(string& extracted, unsigned int docId, IndexInterface<string, unsigned int, unsigned int>* index)
{
	istringstream section(extracted);
	string word;
	unsigned int location = 0;
    
	while (section >> word)
	{
		InputProcessor::getInstance().stripHTML(word);
		string parsed = InputProcessor::getInstance().parse(word);
        
		if (parsed.size() > 0)
		{
			index->insert(parsed, docId, location++);
		}
	}
	
	return location;
}

void SearchEngine::loadFile(const string& directory, const string& file)
{
	string path = directory;
    
	if (directory[directory.size() - 1] != '/')
	{
		path += '/';
	}
    
	path += file;
	
	FILE* input = fopen(path.c_str(), "rb");
	
	if (input)
	{
		char* inputBuf = nullptr;
		long fileLength = 0;
		fseek(input, 0, SEEK_END);
		fileLength = ftell(input);
		
		fseek(input, 0, SEEK_SET);
		inputBuf = (char*) malloc(fileLength * sizeof(char));
		fread(inputBuf, sizeof(char), fileLength, input);
		fclose(input);
		
		json j = json::parse(inputBuf, inputBuf + fileLength);
		free(inputBuf);
		
		DocumentInfo info;
        
		if (!j["id"].empty())
		{
			j["id"].get_to(info.id);
		}
        
		info.path = path;
		string extracted;
        
		if (!j["absolute_url"].empty())
		{
			extracted.clear();
			j["absolute_url"].get_to(extracted);
			info.title = parseCaseTitle(extracted);
		}
        
		if (!j["date_created"].empty())
		{
			extracted.clear();
			j["date_created"].get_to(extracted);
			info.date = parseDate(extracted);
		}
        
		unsigned int relevance = 0;
        
		if (!j["html"].empty())
		{
			extracted.clear();
			j["html"].get_to(extracted);
			InputProcessor::getInstance().stripHTML(extracted);
			relevance += parseSection(extracted, info.id, index);
		}
        
		if (!j["html_lawbox"].empty())
		{
			extracted.clear();
			j["html_lawbox"].get_to(extracted);
			InputProcessor::getInstance().stripHTML(extracted);
			relevance += parseSection(extracted, info.id, index);
		}
        
		if (!j["plain_text"].empty())
		{
			extracted.clear();
			j["plain_text"].get_to(extracted);
			InputProcessor::getInstance().stripHTML(extracted);
			relevance += parseSection(extracted, info.id, index);
		}
		
		if (relevance > 0)
		{
			documents.emplace(info.id, info);
		}
	}
	else
	{
		throw invalid_argument("File \"" + path + "\" could not be opened");
	}
}

void SearchEngine::saveDocumentCache(string& cachePath)
{
	ostringstream buffer;
	buffer << documents.size() << '\n';
    
	for (auto& element : documents)
	{
		buffer << element.first << '\n';
		buffer << element.second.path << '\n';
		buffer << element.second.title << '\n';
		buffer << element.second.date << '\n';
	}
	
	FILE* output = fopen(cachePath.c_str(), "wb");
	fwrite(buffer.str().c_str(), 1, ((long)buffer.tellp()) + 1, output);
	fclose(output);
}

void SearchEngine::loadDocumentCache(string& cachePath)
{
	FILE* input = fopen(cachePath.c_str(), "rb");
    
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
		
		unsigned long total;
		processor >> total;
        
		for (unsigned long i = 0; i < total; i++)
		{
			DocumentInfo info;
			processor >> info.id;
			processor.ignore();
			getline(processor, info.path);
			getline(processor, info.title);
			processor >> info.date;
			documents.emplace(info.id, info);
		}
	}
}
