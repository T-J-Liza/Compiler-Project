#include <iostream>
#include <map>
#include <string>

using namespace std;

class Symbol
{
public:
string name;
string type;
int value;

```
Symbol()
{
    name = "";
    type = "";
    value = 0;
}

Symbol(string n, string t, int v)
{
    name = n;
    type = t;
    value = v;
}
```

};

class SymbolTable
{
map<string, Symbol> table;

public:

```
bool exists(string name)
{
    return table.find(name) != table.end();
}

void insert(string name, string type)
{
    if (exists(name))
    {
        cout << "Semantic Error: Variable already declared" << endl;
        return;
    }

    table[name] = Symbol(name, type, 0);
}

void update(string name, int value)
{
    if (!exists(name))
    {
        cout << "Semantic Error: Variable not declared" << endl;
        return;
    }

    table[name].value = value;
}

int getValue(string name)
{
    if (!exists(name))
    {
        cout << "Semantic Error: Variable not declared" << endl;
        return 0;
    }

    return table[name].value;
}

void display()
{
    cout << "\nSymbol Table\n";

    for (auto item : table)
    {
        cout << item.second.name << " ";
        cout << item.second.type << " ";
        cout << item.second.value << endl;
    }
}
```

};
