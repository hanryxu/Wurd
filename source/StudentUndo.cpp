#include "StudentUndo.h"
#include <iostream>
#include <fstream>

Undo *createUndo() {
    return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
    std::string str = std::string(1, ch);
    if (ch == '\t')
        str = "    ";
    switch (action) {
        case Action::INSERT:
            if (!m_actions.empty() && m_actions.top().a_action_type == action && m_actions.top().a_row == row &&
                m_actions.top().a_col == col - str.length()) {
                m_actions.top().a_col = col;
                m_actions.top().a_count += str.length();
                m_actions.top().a_str += str;
            } else {
                m_actions.push(act(action, row, col, str.size(), str));
            }
            break;
        case Action::DELETE:
            if (!m_actions.empty() && m_actions.top().a_action_type == action && m_actions.top().a_row == row) {
                if (m_actions.top().a_col == col) {             //del
                    m_actions.top().a_str += str;
                } else if (m_actions.top().a_col == col + 1) {  //backspace
                    m_actions.top().a_str = str + m_actions.top().a_str;
                    m_actions.top().a_row = row;
                    m_actions.top().a_col = col;
                    m_actions.top().a_count = str.length();
                }
            } else {
                m_actions.push(act(action, row, col, str.size(), str));
            }
            break;
        case Action::JOIN:
        case Action::SPLIT:
            m_actions.push(act(action, row, col));
            break;
        case Action::ERROR:
            break;
    }

}

StudentUndo::Action StudentUndo::get(int &row, int &col, int &count, std::string &text) {
    if (m_actions.empty())
        return Action::ERROR;
    act action = m_actions.top();
    m_actions.pop();
    switch (action.a_action_type) {
        case Action::INSERT:
            row = action.a_row;
            col = action.a_col - action.a_count;
            count = action.a_count;
            text = "";
            return Action::DELETE;
        case Action::DELETE:
            row = action.a_row;
            col = action.a_col;
            count = 1;
            text = action.a_str;
            return Action::INSERT;
        case Action::SPLIT:
            row = action.a_row;
            col = action.a_col;
            count = 1;
            text = "";
            return Action::JOIN;
        case Action::JOIN:
            row = action.a_row;
            col = action.a_col;
            count = 1;
            text = "";
            return Action::SPLIT;
        case Action::ERROR:
            return Action::ERROR;
    }
    return Action::ERROR;
}

void StudentUndo::clear() {
    while (!m_actions.empty())
        m_actions.pop();
}
