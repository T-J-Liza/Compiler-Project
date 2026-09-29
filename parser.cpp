#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

using namespace std;

#include "ast.cpp"


class Parser
{
private:
    vector<Token> tokens;
    int pos;
    vector<string> errors;


    // Current token
    Token current()
    {
        return tokens[pos];
    }


    // Check current token
    bool check(string type)
    {
        return current().type == type;
    }


    // Consume token if it matches
    bool match(string type)
    {
        if (check(type))
        {
            pos++;
            return true;
        }

        return false;
    }


    // Add parser error
    void error(string message)
    {
        errors.push_back(
            "Error on line " +
            to_string(current().line) +
            ": " +
            message
        );
    }


    // Require a specific token
    bool expect(string type, string message)
    {
        if (match(type))
        {
            return true;
        }

        error(message);
        return false;
    }


    // Basic syntax error recovery.
    //
    // Skip tokens until:
    // 1. semicolon
    // 2. closing brace
    // 3. next line
    // 4. EOF
    void recover()
    {
        int startLine = current().line;

        while (!check("EOF") &&
               !check("SEMICOLON") &&
               !check("RBRACE"))
        {
            if (current().line > startLine)
            {
                break;
            }

            pos++;
        }

        if (check("SEMICOLON"))
        {
            pos++;
        }
    }


    // statement
    shared_ptr<Statement> parseStatement()
    {
        if (check("INTEGER") ||
            check("STRING"))
        {
            return parseDeclaration();
        }

        if (check("IDENTIFIER"))
        {
            return parseAssignment();
        }

        if (check("IF"))
        {
            return parseIf();
        }

        if (check("WHILE"))
        {
            return parseWhile();
        }

        if (check("INPUT"))
        {
            return parseInput();
        }

        if (check("OUTPUT"))
        {
            return parseOutput();
        }

        error(
            "unexpected token '" +
            current().value +
            "'"
        );

        recover();

        return nullptr;
    }


    // Declaration:
    //
    // shongkha x;
    // shongkha x = 10;
    //
    // shobdo name;
    // shobdo name = "Mahim";
    shared_ptr<Statement> parseDeclaration()
    {
        Token typeToken = current();
        pos++;

        string type;

        if (typeToken.type == "INTEGER")
        {
            type = "INTEGER";
        }
        else
        {
            type = "STRING";
        }


        if (!check("IDENTIFIER"))
        {
            error(
                "expected identifier after declaration"
            );

            recover();
            return nullptr;
        }


        string name = current().value;
        int line = current().line;

        pos++;


        auto statement = make_shared<Statement>(
            Statement::DECLARATION,
            line
        );

        statement->type = type;
        statement->name = name;


        // Optional initialization
        if (match("ASSIGN"))
        {
            statement->expr = parseExpression();

            if (!statement->expr)
            {
                recover();
                return nullptr;
            }
        }


        if (!expect(
                "SEMICOLON",
                "expected ';' after declaration"))
        {
            recover();
            return nullptr;
        }

        return statement;
    }


    // Assignment:
    
    // x = 20;
    // name = "Rahim";
    shared_ptr<Statement> parseAssignment()
    {
        Token variable = current();
        pos++;


        auto statement = make_shared<Statement>(
            Statement::ASSIGNMENT,
            variable.line
        );

        statement->name = variable.value;


        if (!expect(
                "ASSIGN",
                "expected '=' after variable"))
        {
            recover();
            return nullptr;
        }


        statement->expr = parseExpression();

        if (!statement->expr)
        {
            recover();
            return nullptr;
        }


        if (!expect(
                "SEMICOLON",
                "expected ';' after assignment"))
        {
            recover();
            return nullptr;
        }

        return statement;
    }


    // Parse:
    //
    // {
    //     statements
    // }
    vector<shared_ptr<Statement>> parseBlock()
    {
        vector<shared_ptr<Statement>> body;


        if (!expect(
                "LBRACE",
                "expected '{'"))
        {
            return body;
        }


        while (!check("RBRACE") &&
               !check("EOF"))
        {
            auto statement = parseStatement();

            if (statement)
            {
                body.push_back(statement);
            }
        }


        if (!expect(
                "RBRACE",
                "expected '}'"))
        {
            return body;
        }


        return body;
    }


    // IF / ELSE IF / ELSE
    shared_ptr<Statement> parseIf()
    {
        Token token = current();
        pos++;


        auto statement = make_shared<Statement>(
            Statement::IF_STATEMENT,
            token.line
        );


        if (!expect(
                "LPAREN",
                "expected '(' after jodi"))
        {
            recover();
            return nullptr;
        }


        statement->condition = parseCondition();

        if (!statement->condition)
        {
            recover();
            return nullptr;
        }


        if (!expect(
                "RPAREN",
                "expected ')' after condition"))
        {
            recover();
            return nullptr;
        }


        statement->body = parseBlock();


        // ELSE IF
        while (match("ELSE_IF"))
        {
            if (!expect(
                    "LPAREN",
                    "expected '(' after noilejodi"))
            {
                recover();
                break;
            }


            auto condition = parseCondition();

            if (!condition)
            {
                recover();
                break;
            }


            if (!expect(
                    "RPAREN",
                    "expected ')' after condition"))
            {
                recover();
                break;
            }


            auto body = parseBlock();

            statement->elseIfs.push_back(
                {condition, body}
            );
        }


        // ELSE
        if (match("ELSE"))
        {
            statement->elseBody = parseBlock();
        }


        return statement;
    }


    // WHILE
    shared_ptr<Statement> parseWhile()
    {
        Token token = current();
        pos++;


        auto statement = make_shared<Statement>(
            Statement::WHILE_STATEMENT,
            token.line
        );


        if (!expect(
                "LPAREN",
                "expected '(' after ghurtethakojodi"))
        {
            recover();
            return nullptr;
        }


        statement->condition = parseCondition();

        if (!statement->condition)
        {
            recover();
            return nullptr;
        }


        if (!expect(
                "RPAREN",
                "expected ')' after condition"))
        {
            recover();
            return nullptr;
        }


        statement->body = parseBlock();

        return statement;
    }


    // Input:

    // niyejaw(age);
    shared_ptr<Statement> parseInput()
    {
        Token token = current();
        pos++;


        auto statement = make_shared<Statement>(
            Statement::INPUT,
            token.line
        );


        if (!expect(
                "LPAREN",
                "expected '(' after niyejaw"))
        {
            recover();
            return nullptr;
        }


        if (!check("IDENTIFIER"))
        {
            error("input requires a variable");
            recover();
            return nullptr;
        }


        statement->name = current().value;
        pos++;


        if (!expect(
                "RPAREN",
                "expected ')' after input variable"))
        {
            recover();
            return nullptr;
        }


        if (!expect(
                "SEMICOLON",
                "expected ';' after input"))
        {
            recover();
            return nullptr;
        }


        return statement;
    }


    // Output:
    //
    // dekhaw(x);
    // dekhaw("Hello");
    shared_ptr<Statement> parseOutput()
    {
        Token token = current();
        pos++;


        auto statement = make_shared<Statement>(
            Statement::OUTPUT,
            token.line
        );


        if (!expect(
                "LPAREN",
                "expected '(' after dekhaw"))
        {
            recover();
            return nullptr;
        }


        statement->expr = parseExpression();

        if (!statement->expr)
        {
            recover();
            return nullptr;
        }


        if (!expect(
                "RPAREN",
                "expected ')' after output expression"))
        {
            recover();
            return nullptr;
        }


        if (!expect(
                "SEMICOLON",
                "expected ';' after output"))
        {
            recover();
            return nullptr;
        }


        return statement;
    }


    // Condition:
    //
    // expression comparison expression
    //
    // Example:
    // age >= 18
    shared_ptr<Expr> parseCondition()
    {
        auto left = parseExpression();

        if (!left)
        {
            return nullptr;
        }


        if (check("GREATER") ||
            check("LESS") ||
            check("GREATER_EQUAL") ||
            check("LESS_EQUAL") ||
            check("EQUAL") ||
            check("NOT_EQUAL"))
        {
            string op = current().value;
            pos++;


            auto right = parseExpression();

            if (!right)
            {
                return nullptr;
            }


            auto expression = make_shared<Expr>(
                Expr::BINARY,
                op,
                left->line
            );

            expression->left = left;
            expression->right = right;

            return expression;
        }


        error(
            "expected comparison operator in condition"
        );

        return nullptr;
    }


    // expression
    //
    // expression -> term (+ term | - term)*
    shared_ptr<Expr> parseExpression()
    {
        auto left = parseTerm();

        if (!left)
        {
            return nullptr;
        }


        while (check("PLUS") ||
               check("MINUS"))
        {
            string op = current().value;
            pos++;


            auto right = parseTerm();

            if (!right)
            {
                return nullptr;
            }


            auto expression = make_shared<Expr>(
                Expr::BINARY,
                op,
                left->line
            );

            expression->left = left;
            expression->right = right;

            left = expression;
        }


        return left;
    }


    // term
    //
    // term -> factor (* factor | / factor)*
    //
    // This gives * and / higher precedence than + and -.
    shared_ptr<Expr> parseTerm()
    {
        auto left = parseFactor();

        if (!left)
        {
            return nullptr;
        }


        while (check("MULTIPLY") ||
               check("DIVIDE"))
        {
            string op = current().value;
            pos++;


            auto right = parseFactor();

            if (!right)
            {
                return nullptr;
            }


            auto expression = make_shared<Expr>(
                Expr::BINARY,
                op,
                left->line
            );

            expression->left = left;
            expression->right = right;

            left = expression;
        }


        return left;
    }


    // factor

    // factor -> integer
    //         | string
    //         | identifier
    //         | (expression)
    shared_ptr<Expr> parseFactor()
    {
        // Integer literal
        if (check("INTEGER_LITERAL"))
        {
            auto expression = make_shared<Expr>(
                Expr::INTEGER,
                current().value,
                current().line
            );

            pos++;

            return expression;
        }


        // String literal
        if (check("STRING_LITERAL"))
        {
            auto expression = make_shared<Expr>(
                Expr::STRING,
                current().value,
                current().line
            );

            pos++;

            return expression;
        }


        // Variable
        if (check("IDENTIFIER"))
        {
            auto expression = make_shared<Expr>(
                Expr::VARIABLE,
                current().value,
                current().line
            );

            pos++;

            return expression;
        }


        // Parenthesized expression
        if (match("LPAREN"))
        {
            auto expression = parseExpression();

            if (!expect(
                    "RPAREN",
                    "expected ')'"))
            {
                return nullptr;
            }

            return expression;
        }


        error("malformed expression");

        return nullptr;
    }


public:

    Parser(vector<Token> tokens)
    {
        this->tokens = tokens;
        this->pos = 0;
    }


    // Parse complete program
    pair<Program, vector<string>> parse()
    {
        Program program;


        while (!check("EOF"))
        {
            auto statement = parseStatement();

            if (statement)
            {
                program.statements.push_back(statement);
            }
        }


        return {program, errors};
    }
};


// Function used by main.cpp
pair<Program, vector<string>>
parseTokens(const vector<Token>& tokens)
{
    Parser parser(tokens);

    return parser.parse();
}