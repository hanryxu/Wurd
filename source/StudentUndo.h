#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"

#include <stack>
#include <string>
#include <vector>

class StudentUndo : public Undo {
public:

    void submit(Action action, int row, int col, char ch = '0');

    Action get(int &row, int &col, int &count, std::string &text);

    void clear();

private:
    struct act {
        Action a_action_type;
        int a_row;
        int a_col;
        int a_count;
        std::string a_str;

        act(Action action, int row, int col, int count = 0, std::string str = "")
                : a_action_type(action), a_row(row), a_col(col), a_count(count), a_str(str) {}
    };

    std::stack<act> m_actions;
};

#endif // STUDENTUNDO_H_
