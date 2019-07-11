/**
 * @file porter2_stemmer.h
 * @author Sean Massung
 * @date September 2012
 *
 * Implementation of
 * http://snowball.tartarus.org/algorithms/english/stemmer.html
 *
 * Copyright (C) 2012 Sean Massung
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _PORTER2_STEMMER_H_
#define _PORTER2_STEMMER_H_

#include <vector>
#include <string>
#include <algorithm>

namespace Porter2Stemmer
{
void stem(std::string& word);

inline void trim(std::string& word)
{
    if (word == "<s>" || word == "</s>")
        return;

    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    std::remove_if(word.begin(), word.end(), [](char ch)
    {
    	return !((ch >= 'a' && ch <= 'z') || ch == '\'');
    });
}

namespace internal
{
inline size_t firstNonVowelAfterVowel(const std::string& word, size_t start);

inline size_t getStartR1(const std::string& word);

inline size_t getStartR2(const std::string& word, size_t startR1);

inline void changeY(std::string& word);

inline void step0(std::string& word);

bool step1A(std::string& word);

void step1B(std::string& word, size_t startR1);

inline void step1C(std::string& word);

void step2(std::string& word, size_t startR1);

inline void step3(std::string& word, size_t startR1, size_t startR2);

inline void step4(std::string& word, size_t startR2);

inline void step5(std::string& word, size_t startR1, size_t startR2);

inline bool isShort(const std::string& word);

inline bool special(std::string& word);

inline bool isVowel(char ch);

inline bool isVowelY(char ch);

inline bool endsWith(const std::string& word, const std::string& str);

inline bool endsInDouble(const std::string& word);

inline bool replaceIfExists(std::string& word, const std::string& suffix,
                     const std::string& replacement, size_t start);

inline bool isValidLIEnding(char ch);

inline bool containsVowel(const std::string& word, size_t start, size_t end);
}
}

#endif
