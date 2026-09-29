#include <string>
#include <vector>
#include <map>
#include <memory>
#include <utility>

using namespace std;

#include "ast.cpp"


class Semantic
{
private:
    // Simple symbol table:
    
    // variable name -> type
    
    // Example:
    // age  -> INTEGER
    // name -> STRING
    map<string, string> symbols;

    vector<string> errors;


    // Find the type of an expression.
    
    // INTEGER -> INTEGER
    // STRING  -> STRING
    // variable -> variable's declared type
    
    // Arithmetic requires INTEGER operands.
    string expressionType(shared_ptr<Expr> expression)
    {
        if (!expression)
        {
            return "ERROR";
        }


        // Integer literal
        if (expression->kind == Expr::INTEGER)
        {
            return "INTEGER";
        }


        // String literal
        if (expression->kind == Expr::STRING)
        {
            return "STRING";
        }


        // Variable
        if (expression->kind == Expr::VARIABLE)
        {
            string name = expression->value;


            if (!symbols.count(name))
            {
                errors.push_back(
                    "Error on line " +
                    to_string(expression->line) +
                    ": variable '" +
                    name +
                    "' is not declared."
                );

                return "ERROR";
            }


            return symbols[name];
        }


        // Binary expression
        string leftType =
            expressionType(expression->left);

        string rightType =
            expressionType(expression->right);


        if (leftType == "ERROR" ||
            rightType == "ERROR")
        {
            return "ERROR";
        }


        string op = expression->value;


        // Arithmetic
        if (op == "+" ||
            op == "-" ||
            op == "*" ||
            op == "/")
        {
            if (leftType != "INTEGER" ||
                rightType != "INTEGER")
            {
                errors.push_back(
                    "Error on line " +
                    to_string(expression->line) +
                    ": arithmetic requires integer operands."
                );

                return "ERROR";
            }

            return "INTEGER";
        }


        // Comparisons are handled separately.
        return "ERROR";
    }


    // Check a condition.
    bool checkCondition(shared_ptr<Expr> expression)
    {
        if (!expression)
        {
            return false;
        }


        if (expression->kind != Expr::BINARY)
        {
            errors.push_back(
                "Error on line " +
                to_string(expression->line) +
                ": invalid condition."
            );

            return false;
        }


        string op = expression->value;


        bool validOperator =
            op == ">" ||
            op == "<" ||
            op == ">=" ||
            op == "<=" ||
            op == "==" ||
            op == "!=";


        if (!validOperator)
        {
            errors.push_back(
                "Error on line " +
                to_string(expression->line) +
                ": invalid condition."
            );

            return false;
        }


        string leftType =
            expressionType(expression->left);

        string rightType =
            expressionType(expression->right);


        if (leftType == "ERROR" ||
            rightType == "ERROR")
        {
            return false;
        }


        // Both sides must have the same type.
        if (leftType != rightType)
        {
            errors.push_back(
                "Error on line " +
                to_string(expression->line) +
                ": invalid comparison between different types."
            );

            return false;
        }


        // Only integer values support
        // ordered comparisons.
        if ((op == ">" ||
             op == "<" ||
             op == ">=" ||
             op == "<=") &&
            leftType != "INTEGER")
        {
            errors.push_back(
                "Error on line " +
                to_string(expression->line) +
                ": only integers support ordered comparison."
            );

            return false;
        }


        return true;
    }


    void checkBody(
        const vector<shared_ptr<Statement>>& body
    )
    {
        for (auto statement : body)
        {
            checkStatement(statement);
        }
    }


    void checkStatement(
        shared_ptr<Statement> statement
    )
    {
        // Declaration

        if (statement->kind ==
            Statement::DECLARATION)
        {
            string name = statement->name;


            // Duplicate declaration
            if (symbols.count(name))
            {
                errors.push_back(
                    "Error on line " +
                    to_string(statement->line) +
                    ": variable '" +
                    name +
                    "' is already declared."
                );

                return;
            }


            symbols[name] = statement->type;


            // Optional initialization
            if (statement->expr)
            {
                string actualType =
                    expressionType(statement->expr);


                if (actualType != "ERROR" &&
                    actualType != statement->type)
                {
                    errors.push_back(
                        "Error on line " +
                        to_string(statement->line) +
                        ": type mismatch."
                    );
                }
            }


            return;
        }


        // Assignment

        if (statement->kind ==
            Statement::ASSIGNMENT)
        {
            string name = statement->name;


            if (!symbols.count(name))
            {
                errors.push_back(
                    "Error on line " +
                    to_string(statement->line) +
                    ": variable '" +
                    name +
                    "' is not declared."
                );

                return;
            }


            string actualType =
                expressionType(statement->expr);


            if (actualType != "ERROR" &&
                actualType != symbols[name])
            {
                errors.push_back(
                    "Error on line " +
                    to_string(statement->line) +
                    ": type mismatch."
                );
            }


            return;
        }

        // Input

        if (statement->kind ==
            Statement::INPUT)
        {
            string name = statement->name;


            if (!symbols.count(name))
            {
                errors.push_back(
                    "Error on line " +
                    to_string(statement->line) +
                    ": variable '" +
                    name +
                    "' is not declared."
                );

                return;
            }


            // Save the declared type inside
            // the AST for code generation.
            statement->type = symbols[name];

            return;
        }


        // Output

        if (statement->kind ==
            Statement::OUTPUT)
        {
            expressionType(statement->expr);

            return;
        }


        // IF / ELSE IF / ELSE

        if (statement->kind ==
            Statement::IF_STATEMENT)
        {
            checkCondition(statement->condition);

            checkBody(statement->body);


            for (auto& branch : statement->elseIfs)
            {
                checkCondition(branch.first);

                checkBody(branch.second);
            }


            checkBody(statement->elseBody);

            return;
        }

        // WHILE

        if (statement->kind ==
            Statement::WHILE_STATEMENT)
        {
            checkCondition(statement->condition);

            checkBody(statement->body);

            return;
        }
    }


public:

    pair<bool, vector<string>>
    check(const Program& program)
    {
        for (auto statement : program.statements)
        {
            checkStatement(statement);
        }


        return {
            errors.empty(),
            errors
        };
    }
};


// Function used by main.cpp
pair<bool, vector<string>>
semanticCheck(const Program& program)
{
    Semantic semantic;

    return semantic.check(program);
}