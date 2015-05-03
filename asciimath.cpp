#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>
using namespace std;

#ifdef HAVE_LIBREADLINE
#  if defined(HAVE_READLINE_READLINE_H)
#    include <readline/readline.h>
#  elif defined(HAVE_READLINE_H)
#    include <readline.h>
#  else /* !defined(HAVE_READLINE_H) */
extern char *readline (const char *prompt);
#  endif /* !defined(HAVE_READLINE_H) */
char *cmdline = NULL;
#else /* !defined(HAVE_READLINE_READLINE_H) */
/* no readline */
#define LINEMAX 1024
char *readline(const char *prompt) {
    if (prompt) {
        fprintf(stderr, "> ");
        fflush(stderr);
    }
    char *buff = new char[LINEMAX];
    if (fgets(buff, LINEMAX, stdin) == NULL) {
        delete[] buff;
        return NULL;
    }
    return buff;
}
#endif /* HAVE_LIBREADLINE */

#ifdef HAVE_READLINE_HISTORY
#  if defined(HAVE_READLINE_HISTORY_H)
#    include <readline/history.h>
#  elif defined(HAVE_HISTORY_H)
#    include <history.h>
#  else /* !defined(HAVE_HISTORY_H) */
extern void add_history (const char *string);
#  endif /* defined(HAVE_READLINE_HISTORY_H) */
#else
/* no history */
void add_history(const char *string) {
}
#endif /* HAVE_READLINE_HISTORY */

#define all(o) (o).begin(), (o).end()
#define allr(o) (o).rbegin(), (o).rend()
const int INF = 2147483647;
typedef long long ll;
typedef pair<int, int> ii;
typedef vector<int> vi;
typedef vector<ii> vii;
typedef vector<vi> vvi;
typedef vector<vii> vvii;
template <class T> int size(T &x) { return x.size(); }

// assert or gtfo

class expr {
public:
    int height, width, lvc;
    virtual ~expr() {
    }
    virtual void draw(char **res, int x, int y)=0;
};

class numexpr: public expr {
public:
    string num;
    numexpr(string num) {
        this->num = num;
        height = 1;
        width = size(num);
        lvc = 0;
    }

    ~numexpr() {
    }

    void draw(char **res, int x, int y) {
        for (int i = 0; i < size(num); i++) {
            res[x][y + i] = num[i];
        }
    }
};

class powexpr: public expr {
public:
    expr *a, *b;
    powexpr(expr *a, expr *b) {
        this->a = a;
        this->b = b;
        height = a->height + b->height;
        width = a->width + b->width;
        lvc = b->height + a->lvc;
    }

    ~powexpr() {
        delete a;
        delete b;
    }

    void draw(char **res, int x, int y) {
        a->draw(res, x + b->height, y);
        b->draw(res, x, y + a->width);
    }
};

class divexpr: public expr {
public:
    expr *a, *b;
    divexpr(expr *a, expr *b) {
        this->a = a;
        this->b = b;
        height = a->height + 1 + b->height;
        width = max(a->width, b->width);
        lvc = a->height;
    }

    ~divexpr() {
        delete a;
        delete b;
    }

    void draw(char **res, int x, int y) {
        a->draw(res, x, y + (width - a->width + 1) / 2);
        b->draw(res, x + a->height + 1, y + (width - b->width + 1) / 2);

        for (int i = 0; i < width; i++) {
            res[x + a->height][y + i] = '-';
        }
    }
};

class negexpr: public expr {
public:
    expr *e;
    negexpr(expr *e) {
        this->e = e;
        height = e->height;
        width = 1 + e->width;
        lvc = e->lvc;
    }

    ~negexpr() {
        delete e;
    }

    void draw(char **res, int x, int y) {
        e->draw(res, x, y + 1);
        res[x + e->lvc][y] = '-';
    }
};

class twoexpr: public expr {
public:
    char op;
    expr *a, *b;
    twoexpr(expr *a, char op, expr *b) {
        this->a = a;
        this->op = op;
        this->b = b;

        int aabove = a->lvc,
            abelow = a->height - a->lvc - 1;

        int babove = b->lvc,
            bbelow = b->height - b->lvc - 1;

        width = a->width + 3 + b->width;
        height = max(aabove, babove) + 1 + max(abelow, bbelow);
        lvc = max(aabove, babove);
    }

    ~twoexpr() {
        delete a;
        delete b;
    }

    void draw(char **res, int x, int y) {

        int aabove = a->lvc;
        int babove = b->lvc;

        int above = max(aabove, babove);

        a->draw(res, x + (above - aabove), y);
        b->draw(res, x + (above - babove), y + a->width + 3);
        res[x + lvc][y + a->width + 1] = op;
    }
};

class insideexpr: public expr {
public:
    char l, r;
    expr *e;
    insideexpr(char l, expr *e, char r) {
        this->l = l;
        this->e = e;
        this->r = r;
        height = e->height;
        width = e->width + 2;
        lvc = (e->height) / 2;
    }

    ~insideexpr() {
        delete e;
    }

    void draw(char **res, int x, int y) {
        e->draw(res, x, y + 1);
        for (int i = 0; i < height; i++) {
            res[x+i][y] = l;
            res[x+i][y + e->width + 1] = r;
        }
    }
};

class hiddeninsideexpr: public expr {
public:
    expr *e;
    hiddeninsideexpr(expr *e) {
        this->e = e;
        height = e->height;
        width = e->width;
        lvc = e->lvc;
    }

    ~hiddeninsideexpr() {
        delete e;
    }

    void draw(char **res, int x, int y) {
        e->draw(res, x, y);
    }
};

/*

e0 := e1 = e0 |
      e1

e1 := e2 + e1 |
      e2 - e1 |
      e2

e2 := e3 * e2 |
      e3 / e2 |
      e3

e3 := e3 ^ e4 |
      e3

e4 := -e4 |
      e5

e5 := NUM |
      VAR |
      ( e0 ) |
      { e0 }

*/

bool is_op(char c) {
    switch (c) {
        case '=':
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        case '(':
        case ')':
        case '{':
        case '}':
            return true;
    }
    return false;
}

string token;
stringstream ss;
string s;
int at = 0;

void error(string msg) {
    cerr << "error: " << msg << endl;
    exit(1);
}

void expect(string tok, string msg) {
    if (token != tok) {
        error(msg);
    }
}

void pop() {
    while (at < size(s) && s[at] == ' ')
        at++;

    if (at == size(s)) {
        token = "";
        return;
    }

    ss.str("");
    ss.clear();
    if (('0' <= s[at] && s[at] <= '9') || s[at] == '.') {
        bool founddigit = false,
             founddot = false;

        while (at < size(s) && (('0' <= s[at] && s[at] <= '9') || (!founddot && s[at] == '.'))) {
            if (s[at] == '.') founddot = true;
            else founddigit = true;

            ss << s[at++];
        }

        if (!founddigit) {
            error("invalid numeric literal");
        }
    } else if (is_op(s[at]) || ('a' <= s[at] && s[at] <= 'z') || ('A' <= s[at] && s[at] <= 'Z')) {
        ss << s[at++];
    } else {
        error(string("unrecognized token: ") + s[at]);
    }

    token = ss.str();
}

expr* e0();

expr* e5() {
    if (token == "(") {
        pop();
        expr *res = e0();
        expect(")", "expected ), got " + token);
        pop();
        return new insideexpr('(', res, ')');
    } else if (token == "{") {
        pop();
        expr *res = e0();
        expect("}", "expected }, got " + token);
        pop();
        return new hiddeninsideexpr(res);
    } else {
        if (token == "") {
            error("unexpected end of expression");
        }

        char c = token[0];
        if (!(c == '.' || ('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))) {
            error(string("invalid literal: ") + token);
        }

        numexpr *res = new numexpr(token);
        pop();
        return res;
    }
}

expr *e4() {
    if (token == "-") {
        pop();
        expr *e = e4();
        return new negexpr(e);
    } else {
        return e5();
    }
}

expr* e3() {
    vector<expr*> exps;
    exps.push_back(e4());

    while (true) {
        if (token == "^") {
            pop();
            exps.push_back(e4());
        } else {
            break;
        }
    }

    expr *res = exps[size(exps) - 1];
    for (int i = size(exps) - 2; i >= 0; i--) {
        res = new powexpr(exps[i], res);
    }

    return res;
}

expr* e2() {
    vector<expr*> exps;
    vector<char> ops;
    exps.push_back(e3());

    while (true) {
        if (token == "*") {
            pop();
            ops.push_back('*');
            exps.push_back(e3());
        } else if (token == "/") {
            pop();
            ops.push_back('/');
            exps.push_back(e3());
        } else {
            break;
        }
    }

    expr *res = exps[0];
    for (int i = 0; i < size(ops); i++) {
        if (ops[i] == '*') {
            res = new twoexpr(res, '*', exps[i+1]);
        } else if (ops[i] == '/') {
            res = new divexpr(res, exps[i+1]);
        } else {
            assert(false);
        }
    }

    return res;
}

expr* e1() {
    vector<expr*> exps;
    vector<char> ops;
    exps.push_back(e2());

    while (true) {
        if (token == "+") {
            pop();
            ops.push_back('+');
            exps.push_back(e2());
        } else if (token == "-") {
            pop();
            ops.push_back('-');
            exps.push_back(e2());
        } else {
            break;
        }
    }

    expr *res = exps[0];
    for (int i = 0; i < size(ops); i++) {
        if (ops[i] == '+') {
            res = new twoexpr(res, '+', exps[i+1]);
        } else if (ops[i] == '-') {
            res = new twoexpr(res, '-', exps[i+1]);
        } else {
            assert(false);
        }
    }

    return res;
}

expr* e0() {
    vector<expr*> exps;
    exps.push_back(e1());

    while (true) {
        if (token == "=") {
            pop();
            exps.push_back(e1());
        } else {
            break;
        }
    }

    expr *res = exps[0];
    for (int i = 1; i < size(exps); i++) {
        res = new twoexpr(res, '=', exps[i]);
    }

    return res;
}

void display() {
    at = 0;
    pop();

    expr *e = e0();

    expect("", "unexpected token at end of expression: " + token);

    char **res = new char*[e->height];
    for (int i = 0; i < e->height; i++) {
        res[i] = new char[e->width];
        memset(res[i], ' ', e->width);
    }

    e->draw(res, 0, 0);

    for (int i = 0; i < e->height; i++) {
        int end = e->width - 1;
        while (end >= 0 && res[i][end] == ' ') {
            end--;
        }

        for (int j = 0; j <= end; j++) {
            cout << res[i][j];
        }
        cout << endl;
    }

    delete e;

    for (int i = 0; i < e->height; i++)
        delete[] res[i];

    delete[] res;
}

int main(int argc, char *argv[]) {

    if (argc >= 2) {
        stringstream ss;
        for (int i = 1; i < argc; i++) {
            ss << argv[i];
        }
        s = ss.str();
        display();
    } else {
        while (true) {
            char *line = readline("> ");
            if (line == NULL) {
                break;
            }

            add_history(line);

            s = string(line);
            delete[] line;
            display();
        }
    }

    return 0;
}

