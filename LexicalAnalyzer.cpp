#include <iostream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// Token type constants
const string INTEGER = "INTEGER";
const string IDENTIFIER = "IDENTIFIER";
const string ASSIGNMENT = "ASSIGNMENT";
const string INTEGER_LITERAL = "INTEGER_LITERAL";
const string PRINT = "PRINT";
const string PLUS = "PLUS";
const string MINUS = "MINUS";
const string MULTIPLICATION = "MULTIPLICATION";

// Token class
class Token
{
public:
    string type_;
    string value;
    int line;

    Token(string type_, string value, int line)
    {
        this->type_ = type_;
        this->value = value;
        this->line = line;
    }

    string toString()
    {
        if (!value.empty())
        {
            return "<" + type_ + ", " + value + ">";
        }
        return "<" + type_ + ">";
    }
};

// Lexer class
class Lexer
{
private:
    string code;
    int pos;
    int line;
    vector<Token> tokens;

public:
    Lexer(string code)
    {
        this->code = code;
        this->pos = 0;
        this->line = 0;
    }

    // Get current character
    char getChar()
    {
        if (pos < code.length())
        {
            return code[pos];
        }
        return '\0';
    }

    // Move to next character
    void advance()
    {
        if (pos < code.length())
        {
            if (code[pos] == '\n')
            {
                line++;
            }
            pos++;
        }
    }

    // Tokenize the source code
    pair<vector<Token>, string> tokenize()
    {
        string error = "";

        while (true)
        {
            char c = getChar();

            // End of file
            if (c == '\0')
            {
                break;
            }

            // Whitespace
            else if (c == ' ' || c == '\t' || c == '\n')
            {
                advance();
            }

            // Integer literal
            else if (isdigit(c))
            {
                string num = "";

                while (c != '\0' && isdigit(c))
                {
                    num += c;
                    advance();
                    c = getChar();
                }

                tokens.push_back(
                    Token(INTEGER_LITERAL, num, line));
            }

            // Identifier / keyword
            else if (isalpha(c))
            {
                string words = "";

                while (c != '\0' &&
                       (isalnum(c) || c == '_'))
                {

                    words += c;
                    advance();
                    c = getChar();
                }

                if (words == "integer")
                {
                    tokens.push_back(
                        Token(INTEGER, "", line));
                }

                else if (words == "ptr")
                {
                    tokens.push_back(
                        Token(PRINT, "", line));
                }

                else
                {
                    tokens.push_back(
                        Token(IDENTIFIER, words, line));
                }
            }

            // Assignment :
            else if (c == ':')
            {
                tokens.push_back(
                    Token(ASSIGNMENT, string(1, c), line));
                advance();
            }

            // Plus +
            else if (c == '+')
            {
                tokens.push_back(
                    Token(PLUS, string(1, c), line));
                advance();
            }

            // Minus -
            else if (c == '-')
            {
                tokens.push_back(
                    Token(MINUS, string(1, c), line));
                advance();
            }

            // Multiplication *
            else if (c == '*')
            {
                tokens.push_back(
                    Token(MULTIPLICATION, string(1, c), line));
                advance();
            }

            // Illegal character
            else
            {
                error = "LexerError: Illegal Char: '" +
                        string(1, c) +
                        "' at line " +
                        to_string(line);
                break;
            }
        }

        return {tokens, error};
    }
};

// Main function
int main()
{

    string code =
        "integer x\n"
        "x : 10 + 20\n"
        "ptr x";

    Lexer lexer(code);

    auto result = lexer.tokenize();

    vector<Token> tokens = result.first;
    string error = result.second;

    // Print tokens
    for (Token token : tokens)
    {
        cout << token.toString() << endl;
    }

    // Print error if exists
    if (!error.empty())
    {
        cout << error << endl;
    }

    return 0;
}