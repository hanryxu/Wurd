#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>

#include <iostream>
#include <fstream>
#include <algorithm>

TextEditor *createTextEditor(Undo *un) {
    return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo *undo)
        : TextEditor(undo) {
    reset();
}

StudentTextEditor::~StudentTextEditor() {
    m_text.clear();
}

bool StudentTextEditor::load(std::string file) {
    std::ifstream infile(file);
    if (!infile)
        return false;
    else {
        // successfully loaded
        reset();
        m_text.clear();
        std::string read_line;
        while (getline(infile, read_line)) {
            if (read_line.back() == '\r')
                read_line.pop_back();
            m_text.push_back(read_line);
        }
        if (!m_text.empty()) {
            m_r = 0, m_c = 0;
            m_curLine = m_text.begin();
        }
        return true;
    }
}

bool StudentTextEditor::save(std::string file) {
    std::ofstream outfile(file);
    if (!outfile)
        return false;
    else {
        for (std::list<std::string>::iterator it = m_text.begin(); it != m_text.end(); it++)
            outfile << (*it) << '\n';
        return true;
    }
}

void StudentTextEditor::reset() {
    m_text.clear();
    m_text.push_back("");
    m_r = 0, m_c = 0;
    m_curLine = m_text.begin();
    getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {
    switch (dir) {
        case Dir::UP:
            if (m_r > 0) {
                m_r--;
                m_curLine--;
                if (m_c > m_curLine->size())
                    m_c = m_curLine->size();
            }
            break;
        case Dir::DOWN:
            if (m_r < m_text.size() - 1) {
                m_r++;
                m_curLine++;
                if (m_c > m_curLine->size())
                    m_c = m_curLine->size();
            }
            break;
        case Dir::LEFT:
            if (m_c == 0 && m_r > 0) {
                m_r--;
                m_curLine--;
                m_c = m_curLine->size();
            } else if (m_c > 0)
                m_c--;
            break;
        case Dir::RIGHT:
            if (m_c == m_curLine->size() && m_r < m_text.size()) {
                m_r++;
                m_curLine++;
                m_c = 0;
            } else if (m_c < m_curLine->size())
                m_c++;
            break;
        case Dir::HOME:
            m_c = 0;
            break;
        case Dir::END:
            m_c = m_curLine->size();
            break;
        default:;
    }
}

void StudentTextEditor::del() {
    char ch = (*m_curLine)[m_c];
    if (m_c < (*m_curLine).size()) {
        (*m_curLine) = (*m_curLine).substr(0, m_c) + (*m_curLine).substr(m_c + 1);
        getUndo()->submit(Undo::Action::DELETE, m_r, m_c, ch);
    } else if (m_c == (*m_curLine).size() && m_r < m_text.size()) {
        (*m_curLine) = (*m_curLine) + (*std::next(m_curLine));
        m_text.erase(std::next(m_curLine));
        getUndo()->submit(Undo::Action::JOIN, m_r, m_c);
    }
    // else, do nothing
}

void StudentTextEditor::backspace() {
    if (m_c > 0) {
        char ch = (*m_curLine)[m_c - 1];
        (*m_curLine).erase(m_c - 1, 1);
        m_c--;
        getUndo()->submit(Undo::Action::DELETE, m_r, m_c, ch);
    } else if (m_c == 0 && m_r > 0) {
        m_r--;
        m_c = (*std::prev(m_curLine)).size();
        *std::prev(m_curLine) += (*m_curLine);
        m_curLine = std::prev(m_text.erase(m_curLine));
        getUndo()->submit(Undo::Action::JOIN, m_r, m_c);
    }
}

void StudentTextEditor::insert(char ch) {
    std::string str = "";
    int delta_m_c = 1;
    if (ch == '\t') {
        str = "    ";
        delta_m_c = 4;
    } else
        str = ch;
    (*m_curLine).insert(m_c, str);
    m_c += delta_m_c;
    getUndo()->submit(Undo::Action::INSERT, m_r, m_c, ch);
}

void StudentTextEditor::enter() {
    m_text.insert(std::next(m_curLine), (*m_curLine).substr(m_c));
    (*m_curLine) = (*m_curLine).substr(0, m_c);
    getUndo()->submit(Undo::Action::SPLIT, m_r, m_c);
    m_r++;
    m_c = 0;
    m_curLine++;
}

void StudentTextEditor::getPos(int &row, int &col) const {
    row = m_r;
    col = m_c;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string> &lines) const {
    if (startRow < 0 || startRow >= m_text.size() || numRows < 0)
        return -1;
    std::list<std::string>::const_iterator it = m_text.begin();
    std::advance(it, startRow);
    int lines_num = 0;
    lines.clear();
    while (it != m_text.end() && lines_num < numRows) {
        lines.push_back((*it));
        lines_num++;
        it++;
    }
    return lines_num;
}

void StudentTextEditor::undo() {
    int tar_count;
    std::string tar_text;
    enum Undo::Action action;
    action = getUndo()->get(m_r, m_c, tar_count, tar_text);
    std::list<std::string>::iterator it = m_text.begin();
    std::advance(it, m_r);
    switch (action) {
        case Undo::Action::INSERT:
            (*it).insert(m_c, tar_text);
            break;
        case Undo::Action::DELETE:
            (*it).erase(m_c, tar_count);
            break;
        case Undo::Action::SPLIT:
            m_text.insert(std::next(it), (*it).substr(m_c));
            (*it) = (*it).substr(0, m_c);
            break;
        case Undo::Action::JOIN:
            (*it) += (*std::next(it));
            m_text.erase(std::next(it));
            break;
        case Undo::Action::ERROR:
            break;
        default:;
    }
}