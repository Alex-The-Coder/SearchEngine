# SearchEngine
A fully functional Search Engine to index and peruse past supreme court cases

Case Files Download: https://www.courtlistener.com/api/bulk-data/opinions/scotus.tar.gz

Thunderbolt Features:
- User Command System
* Ability to index entire directories or individual files (user command)
* Debug information and statistics about index size and most common words
* Clearing index
* Saving and loading index to/from disk cache file

- Robust user search system
* Index comprised of stemmed words with pre-compiled stop words left out of index
* User queries stemmed and stopped to match index
* Supports multi-word queries
* Supports multi-keyword or multi-phrase searches with AND/OR/NOT operations to join the different result sets
* Built-in autocorrect system to suggest spelling corrections based on indexed words and phrases
* In-memory storage of index and file contents for instant (< 1 ms) query response

- High configurability
* Able to store index in a Hash Table or AVL Tree based on user selection
* Able to convert Hash Table data to AVL Tree data (and vice versa)
* Able to search and index other files outside of the supreme court corpus as long as the JSON formatting is followed
