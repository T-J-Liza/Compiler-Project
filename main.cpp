#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <memory>

using namespace std;
#include "ast.cpp"

// Functions implemented in other compiler phases.
pair<vector<Token>, vector<string>> lexSource(const string& source);

pair<Program, vector<string>>
parseTokens(const vector<Token>& tokens);

pair<bool, vector<string>>
semanticCheck(const Program& program);

string generatePython(const Program& program);


int main(int argc, char* argv[])
{
    // Compiler usage: banglish_compiler source.bl output.py

    if (argc != 3)
    {
        cout << "Usage: banglish_compiler source.bl output.py\n";
        return 1;
    }

    // Read source file
    ifstream input(argv[1]);

    if (!input)
    {
        cout << "Error: could not open source file.\n";
        return 1;
    }

    string source(
        (istreambuf_iterator<char>(input)),
        istreambuf_iterator<char>()
    );

    // Phase 1: Lexical Analysis

    auto lexed = lexSource(source);

    for (const string& error : lexed.second)
    {
        cout << error << endl;
    }

    if (!lexed.second.empty())
    {
        cout << "Compilation failed.\n";
        return 1;
    }

    // Phase 2: Syntax Analysis

    auto parsed = parseTokens(lexed.first);

    for (const string& error : parsed.second)
    {
        cout << error << endl;
    }

    if (!parsed.second.empty())
    {
        cout << "Compilation failed.\n";
        return 1;
    }

    // Phase 3: Semantic Analysis

    auto checked = semanticCheck(parsed.first);

    for (const string& error : checked.second)
    {
        cout << error << endl;
    }

    if (!checked.first)
    {
        cout << "Compilation failed.\n";
        return 1;
    }

    // Phase 4: Code Generation

    ofstream output(argv[2]);

    if (!output)
    {
        cout << "Error: could not create output file.\n";
        return 1;
    }

    string pythonCode = generatePython(parsed.first);

    output << pythonCode;

    cout << "Compilation successful. "
         << "Python code written to "
         << argv[2] << endl;

    return 0;
}