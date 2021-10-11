#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>
#include <map>

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() {
        root = new trieNode;
    }

    virtual ~StudentSpellCheck();

    bool load(std::string dict_file);

    bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string> &suggestions);

    void spellCheckLine(const std::string &line, std::vector<Position> &problems);

private:
    const std::string alpha = "abcdefghijklmnopqrstuvwxyz'";

    class trieNode {
    public:
        trieNode *m_child[27];
        bool m_isEnd;

        trieNode() {
            m_isEnd = false;
            std::memset(m_child, 0, sizeof(m_child));
        }
    };

    trieNode *root;

    void insert(std::string word) {
        trieNode *node = root;
        for (int i = 0; i < word.size(); i++) {
            int next = lower(word[i]);
            if (next != -1) {
                if (node->m_child[next] == nullptr)
                    node->m_child[next] = new trieNode;
                node = node->m_child[next];
            }
        }
        node->m_isEnd = true;
    }

    bool search(std::string word) {
        trieNode *node = root;
        for (int i = 0; i < word.size(); i++) {
            int next = lower(word[i]);
            if (next != -1) {
                node = node->m_child[next];
                if (node == nullptr)
                    return false;
            }
        }
        return node->m_isEnd;
    }

    int lower(char ch) {
        if (ch >= 'A' && ch <= 'Z') {
            return (ch - 'A');
        } else {
            if (ch >= 'a' && ch <= 'z')
                return (ch - 'a');
            else if (ch == '\'')
                return 26;
        }
        return -1;
    }

    void erase(trieNode *node) {
        if (node != nullptr) {
            for (int i = 0; i < 27; i++)
                erase(node->m_child[i]);
            delete node;
        }
    }
};

#endif  // STUDENTSPELLCHECK_H_
