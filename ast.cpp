#include <string>
#include <vector>
#include <memory>
#include <utility>

using namespace std;

// -------------------------
// Token
// -------------------------

struct Token
{
    string type;
    string value;
    int line;
};

// -------------------------
// Expression
// -------------------------

struct Expr
{
    enum Kind
    {
        INTEGER,
        STRING,
        VARIABLE,
        BINARY
    };

    Kind kind;

    // Literal value, variable name,
    // or binary operator.
    string value;

    shared_ptr<Expr> left;
    shared_ptr<Expr> right;

    int line;

    Expr(
        Kind kind,
        string value = "",
        int line = 0)
    {
        this->kind = kind;
        this->value = value;
        this->line = line;
    }
};

// Statement

struct Statement
{
    enum Kind
    {
        DECLARATION,
        ASSIGNMENT,
        IF_STATEMENT,
        WHILE_STATEMENT,
        INPUT,
        OUTPUT
    };

    Kind kind;

    string name;
    string type;

    shared_ptr<Expr> expr;
    shared_ptr<Expr> condition;

    // Statements inside { }
    vector<shared_ptr<Statement>> body;

    // ELSE IF branches

    // condition + body
    vector<
        pair<
            shared_ptr<Expr>,
            vector<shared_ptr<Statement>>>>
        elseIfs;

    // ELSE body
    vector<shared_ptr<Statement>> elseBody;

    int line;

    Statement(
        Kind kind,
        int line = 0)
    {
        this->kind = kind;
        this->line = line;
    }
};

// Program

struct Program
{
    vector<shared_ptr<Statement>> statements;
};