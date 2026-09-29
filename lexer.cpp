#include <iostream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// Token custom cls toiri korechi
struct Token
{
    string type;
    string value;
    int line;
};

class Lexer
{
private:
    string code;
    int pos;
    int line;

    vector<Token> tokens;
    vector<string> errors;

    // crrnt char pabo
    char current()
    {
        if (pos < (int)code.size())
        {
            return code[pos];
        }

        return '\0';
    }

    // poroborti char ki ache dekhe
    char peek()
    {
        if (pos + 1 < (int)code.size())
        {
            return code[pos + 1];
        }

        return '\0';
    }

    // poroborti char e jay lexer
    void advance()
    {
        if (current() == '\n')
        {
            line++;
        }

        if (pos < (int)code.size())
        {
            pos++;
        }
    }

    // Token const toiri
    void addToken(string type, string value = "")
    {
        tokens.push_back({type, value, line});
    }

    // error msg add hobe
    void addError(string message)
    {
        errors.push_back(
            "Error on line " +
            to_string(line) +
            ": " +
            message);
    }

    //  integer literal porbo
    void readNumber()
    {
        string number = "";

        while (isdigit(current()))
        {
            number += current();
            advance();
        }

        addToken("INTEGER_LITERAL", number);
    }

    // identifier or keyword porbo
    void readIdentifier()
    {
        string word = "";

        while (isalnum(current()) || current() == '_')
        {
            word += current();
            advance();
        }

        // Keywords
        if (word == "shongkha")
        {
            addToken("INTEGER");
        }
        else if (word == "shobdo")
        {
            addToken("STRING");
        }
        else if (word == "jodi")
        {
            addToken("IF");
        }
        else if (word == "noilejodi")
        {
            addToken("ELSE_IF");
        }
        else if (word == "noile")
        {
            addToken("ELSE");
        }
        else if (word == "ghurtethakojodi")
        {
            addToken("WHILE");
        }
        else if (word == "dekhaw")
        {
            addToken("OUTPUT");
        }
        else if (word == "niyejaw")
        {
            addToken("INPUT");
        }
        else
        {
            addToken("IDENTIFIER", word);
        }
    }

    //  string literal porbo
    void readString()
    {
        int startLine = line;

        // Skip korbo
        advance();

        string value = "";

        while (current() != '\0' &&
               current() != '"')
        {
            // code er shesh char
            if (current() == '\n')
            {
                addError("unterminated string literal");
                return;
            }

            value += current();
            advance();
        }

        if (current() == '"')
        {
            advance();

            tokens.push_back(
                {"STRING_LITERAL", value, startLine});
        }
        else
        {
            addError("unterminated string literal");
        }
    }

public:
    Lexer(string code)
    {
        this->code = code;
        this->pos = 0;
        this->line = 1;
    }

    // token & error msg dibe
    pair<vector<Token>, vector<string>> tokenize()
    {
        while (current() != '\0')
        {
            char c = current();

            // Whitespace
            if (c == ' ' ||
                c == '\t' ||
                c == '\r')
            {
                advance();
                continue;
            }

            // New line
            if (c == '\n')
            {
                advance();
                continue;
            }

            // Comment
            // Everything after # is ignored until newline.
            if (c == '#')
            {
                while (current() != '\0' &&
                       current() != '\n')
                {
                    advance();
                }

                continue;
            }

            // Integer
            if (isdigit(c))
            {
                readNumber();
                continue;
            }

            // Identifier / keyword
            if (isalpha(c) || c == '_')
            {
                readIdentifier();
                continue;
            }

            // String
            if (c == '"')
            {
                readString();
                continue;
            }

            // Semicolon
            if (c == ';')
            {
                addToken("SEMICOLON", ";");
                advance();
                continue;
            }

            // Parentheses
            if (c == '(')
            {
                addToken("LPAREN", "(");
                advance();
                continue;
            }

            if (c == ')')
            {
                addToken("RPAREN", ")");
                advance();
                continue;
            }

            // Braces
            if (c == '{')
            {
                addToken("LBRACE", "{");
                advance();
                continue;
            }

            if (c == '}')
            {
                addToken("RBRACE", "}");
                advance();
                continue;
            }

            // Arithmetic operators
            if (c == '+')
            {
                addToken("PLUS", "+");
                advance();
                continue;
            }

            if (c == '-')
            {
                addToken("MINUS", "-");
                advance();
                continue;
            }

            if (c == '*')
            {
                addToken("MULTIPLY", "*");
                advance();
                continue;
            }

            if (c == '/')
            {
                addToken("DIVIDE", "/");
                advance();
                continue;
            }

            // Assignment or equality
            if (c == '=')
            {
                if (peek() == '=')
                {
                    addToken("EQUAL", "==");
                    advance();
                    advance();
                }
                else
                {
                    addToken("ASSIGN", "=");
                    advance();
                }

                continue;
            }

            // Not equal
            if (c == '!')
            {
                if (peek() == '=')
                {
                    addToken("NOT_EQUAL", "!=");
                    advance();
                    advance();
                }
                else
                {
                    addError("invalid token '!'");
                    advance();
                }

                continue;
            }

            // Greater / greater equal
            if (c == '>')
            {
                if (peek() == '=')
                {
                    addToken("GREATER_EQUAL", ">=");
                    advance();
                    advance();
                }
                else
                {
                    addToken("GREATER", ">");
                    advance();
                }

                continue;
            }

            // Less / less equal
            if (c == '<')
            {
                if (peek() == '=')
                {
                    addToken("LESS_EQUAL", "<=");
                    advance();
                    advance();
                }
                else
                {
                    addToken("LESS", "<");
                    advance();
                }

                continue;
            }

            // Anything else is invalid
            addError(
                "invalid character '" +
                string(1, c) +
                "'");

            advance();
        }

        // End of file token
        tokens.push_back({"EOF", "", line});

        return {tokens, errors};
    }
};

// Function used by main.cpp
pair<vector<Token>, vector<string>>
lexSource(const string &source)
{
    Lexer lexer(source);

    return lexer.tokenize();
}
