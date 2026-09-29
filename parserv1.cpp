#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Parser
{
    vector<Token> tokens;
    int pos;
    string error;

public:
    Parser(vector<Token> t)
    {
        tokens = t;
        pos = 0;
        error = "";
    }

    Token current()
    {
        if (pos < tokens.size())
            return tokens[pos];

        return Token("EOF", "", -1);
    }

    void advance()
    {
        pos++;
    }

    bool check(string type)
    {
        return current().type_ == type;
    }

    bool parse()
    {
        while (!check("EOF"))
        {
            if (!statement())
                return false;
        }

        return true;
    }

    bool statement()
    {
        if (check(INTEGER))
            return declaration();

        if (check(IDENTIFIER))
            return assignment();

        if (check(PRINT))
            return printStatement();

        error = "Parser Error";
        return false;
    }

    bool declaration()
    {
        advance();

        if (!check(IDENTIFIER))
        {
            error = "Expected identifier";
            return false;
        }

        advance();
        return true;
    }

    bool assignment()
    {
        advance();

        if (!check(ASSIGNMENT))
        {
            error = "Expected :";
            return false;
        }

        advance();

        expression();

        return true;
    }

    bool printStatement()
    {
        advance();

        if (!check(IDENTIFIER))
        {
            error = "Expected identifier";
            return false;
        }

        advance();
        return true;
    }

    int expression()
    {
        int result = term();

        while (check(PLUS) || check(MINUS))
        {
            string op = current().type_;
            advance();

            int right = term();

            if (op == PLUS)
                result = result + right;
            else
                result = result - right;
        }

        return result;
    }

    int term()
    {
        int result = factor();

        while (check(MULTIPLICATION))
        {
            advance();

            int right = factor();

            result = result * right;
        }

        return result;
    }

    int factor()
    {
        if (check(INTEGER_LITERAL))
        {
            int value = stoi(current().value);
            advance();

            return value;
        }

        if (check(IDENTIFIER))
        {
            advance();

            return 0;
        }

        error = "Expected number or identifier";

        return 0;
    }

    string getError()
    {
        return error;
    }
};
