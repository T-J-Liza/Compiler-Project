
class Parser
{
private:
    vector<Token> tokens;
    int pos;
    string error;

public:
    Parser(vector<Token> tokens)
    {
        this->tokens = tokens;
        pos = 0;
        error = "";
    }
    Token current()
    {
        if (pos < (int)tokens.size())
            return tokens[pos];

        return Token("EOF", "", -1);
    }
    void advance()
    {
        if (pos < (int)tokens.size())
            pos++;
    }

    bool check(string type)
    {
        return current().type_ == type;
    }

    bool match(string type)
    {
        if (check(type))
        {
            advance();
            return true;
        }

        return false;
    }

    void parserError(string message)
    {
        if (error.empty())
        {
            error = "ParserError: " +
                    message +
                    " at line " +
                    to_string(current().line);
        }
    }
    bool parse()
    {
        while (current().type_ != "EOF")
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

        else if (check(IDENTIFIER))
            return assignment();

        else if (check(PRINT))
            return printStatement();

        else
        {
            parserError(
                "Unexpected token " +
                current().type_);

            return false;
        }
    }
    bool declaration()
    {

        advance();

        if (!check(IDENTIFIER))
        {
            parserError(
                "Expected identifier after 'integer'");

            return false;
        }

        advance();

        return true;
    }
    bool assignment()
    {

        advance();

        if (!match(ASSIGNMENT))
        {
            parserError(
                "Expected ':' after identifier");

            return false;
        }

        int dummy;

        if (!expression(dummy))
            return false;

        return true;
    }
    bool printStatement()
    {
        // ptr
        advance();

        // identifier
        if (!check(IDENTIFIER))
        {
            parserError(
                "Expected identifier after 'ptr'");

            return false;
        }

        advance();

        return true;
    }
    bool expression(int &result)
    {
        if (!term(result))
            return false;

        while (check(PLUS) || check(MINUS))
        {
            string op = current().type_;

            advance();

            int right;

            if (!term(right))
                return false;

            if (op == PLUS)
                result += right;

            else if (op == MINUS)
                result -= right;
        }

        return true;
    }
    bool term(int &result)
    {
        if (!factor(result))
            return false;

        while (check(MULTIPLICATION))
        {
            advance();

            int right;

            if (!factor(right))
                return false;

            result *= right;
        }

        return true;
    }
    bool factor(int &result)
    {
        if (check(INTEGER_LITERAL))
        {
            result = stoi(current().value);
            advance();

            return true;
        }

        else if (check(IDENTIFIER))
        {
            advance();

            result = 0;

            return true;
        }

        else
        {
            parserError(
                "Expected integer or identifier");

            return false;
        }
    }
    string getError()
    {
        return error;
    }
};