#include "StudentSpellCheck.h"
#include <string>
#include <vector>

#include <iostream>
#include <fstream>

SpellCheck *createSpellCheck() {
    return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
    erase(root);
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
    std::fstream dict_file(dictionaryFile);
    if (!dict_file)
        return false;
    std::string word = "";
    while (getline(dict_file, word))
        insert(word);
    return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string> &suggestions) {
    bool isIn = search(word);
    if (!isIn) {
        int cnt = 0;
        suggestions.clear();
        for (int i = 0; i < word.length() && i < max_suggestions; i++) {
            char ch = word[i];
            for (int j = 0; j < 27; j++)
                if (ch != alpha[j]) {
                    word[i] = alpha[j];
                    if (search(word)) {
                        suggestions.push_back(word);
                        cnt++;
                    }
                }
            word[i] = ch;
        }
    }
    return false;
}

void StudentSpellCheck::spellCheckLine(const std::string &line, std::vector<SpellCheck::Position> &problems) {
    problems.clear();
    std::vector<bool> isValid;
    std::string word = "";
    Position pos;
    std::string l_line = line + " ";
    for (int i = 0; i < l_line.length(); i++) {
        if (word == "") {
            pos.start = i;
            pos.end = i;
            problems.push_back(pos);
        }
        int int_ch = lower(line[i]);
        if (int_ch != -1)
            word += alpha[int_ch];
        else {
            problems.back().end = i;
            if (word == "" || search(word))
                problems.pop_back();
            word = "";
        }
    }
}
