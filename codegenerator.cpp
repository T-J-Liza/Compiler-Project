#include <string>
#include <vector>
#include <memory>

using namespace std;

#include "ast.cpp"

class Generator
{
private:
    // Conversion to Python.
    string expression(shared_ptr<Expr> expr)
    {
        // Integer literal
        if (expr->kind == Expr::INTEGER)
        {
            return expr->value;
        }

        // String literal
        if (expr->kind == Expr::STRING)
        {
            string result = "\"";

            // Escape quotes and backslashes
            // for valid Python strings.
            for (char c : expr->value)
            {
                if (c == '"' || c == '\\')
                {
                    result += '\\';
                }

                result += c;
            }

            result += "\"";

            return result;
        }

        // Variable
        if (expr->kind == Expr::VARIABLE)
        {
            return expr->value;
        }

        // Binary expression
        return "(" +
               expression(expr->left) +
               " " +
               expr->value +
               " " +
               expression(expr->right) +
               ")";
    }

    // Generate one indented line.
    void addLine(
        string &output,
        int indent,
        string text)
    {
        output += string(indent * 4, ' ');
        output += text;
        output += "\n";
    }

    // Generate a list of statements.
    void generateStatements(
        const vector<shared_ptr<Statement>> &statements,
        string &output,
        int indent)
    {
        for (auto statement : statements)
        {
            generateStatement(
                statement,
                output,
                indent);
        }
    }

    // Generate a single statement.
    void generateStatement(
        shared_ptr<Statement> statement,
        string &output,
        int indent)
    {
        // Declaration

        if (statement->kind ==
            Statement::DECLARATION)
        {
            if (statement->expr)
            {
                addLine(
                    output,
                    indent,
                    statement->name +
                        " = " +
                        expression(statement->expr));
            }
            else
            {
                // Python has no direct uninitialized
                // variable declaration.
                addLine(
                    output,
                    indent,
                    statement->name +
                        " = None");
            }

            return;
        }

        // -------------------------
        // Assignment
        // -------------------------

        if (statement->kind ==
            Statement::ASSIGNMENT)
        {
            addLine(
                output,
                indent,
                statement->name +
                    " = " +
                    expression(statement->expr));

            return;
        }

        // Input

        if (statement->kind ==
            Statement::INPUT)
        {
            string inputCode;

            if (statement->type == "INTEGER")
            {
                inputCode = "int(input())";
            }
            else
            {
                inputCode = "input()";
            }

            addLine(
                output,
                indent,
                statement->name +
                    " = " +
                    inputCode);

            return;
        }

        // Output

        if (statement->kind ==
            Statement::OUTPUT)
        {
            addLine(
                output,
                indent,
                "print(" +
                    expression(statement->expr) +
                    ")");

            return;
        }

        // WHILE

        if (statement->kind ==
            Statement::WHILE_STATEMENT)
        {
            addLine(
                output,
                indent,
                "while " +
                    expression(statement->condition) +
                    ":");

            if (statement->body.empty())
            {
                addLine(
                    output,
                    indent + 1,
                    "pass");
            }
            else
            {
                generateStatements(
                    statement->body,
                    output,
                    indent + 1);
            }

            return;
        }

        // IF / ELSE IF / ELSE

        if (statement->kind ==
            Statement::IF_STATEMENT)
        {
            // IF
            addLine(
                output,
                indent,
                "if " +
                    expression(statement->condition) +
                    ":");

            if (statement->body.empty())
            {
                addLine(
                    output,
                    indent + 1,
                    "pass");
            }
            else
            {
                generateStatements(
                    statement->body,
                    output,
                    indent + 1);
            }

            // ELSE IF
            for (auto &branch : statement->elseIfs)
            {
                addLine(
                    output,
                    indent,
                    "elif " +
                        expression(branch.first) +
                        ":");

                if (branch.second.empty())
                {
                    addLine(
                        output,
                        indent + 1,
                        "pass");
                }
                else
                {
                    generateStatements(
                        branch.second,
                        output,
                        indent + 1);
                }
            }

            // ELSE
            if (!statement->elseBody.empty())
            {
                addLine(
                    output,
                    indent,
                    "else:");

                generateStatements(
                    statement->elseBody,
                    output,
                    indent + 1);
            }

            return;
        }
    }

public:
    string generate(const Program &program)
    {
        string output;

        for (auto statement : program.statements)
        {
            generateStatement(
                statement,
                output,
                0);
        }

        return output;
    }
};

// Function used by main.cpp
string generatePython(const Program &program)
{
    Generator generator;

    return generator.generate(program);
}