class Symbol
{
public:
    string name;
    string type;
    int value;
    int line;

    Symbol()
    {
        name = "";
        type = "";
        value = 0;
        line = 0;
    }

    Symbol(string name,
           string type,
           int value,
           int line)
    {
        this->name = name;
        this->type = type;
        this->value = value;
        this->line = line;
    }
};
class SymbolTable
{
private:
    map<string, Symbol> table;

public:
    bool exists(string name)
    {
        return table.find(name) != table.end();
    }

    bool insert(string name,
                string type,
                int value,
                int line)
    {
        if (exists(name))
        {
            cout << "Semantic Error: Variable '"
                 << name
                 << "' already declared."
                 << endl;

            return false;
        }

        table[name] =
            Symbol(name, type, value, line);

        return true;
    }

    bool update(string name, int value)
    {
        if (!exists(name))
        {
            cout << "Semantic Error: Variable '"
                 << name
                 << "' not declared."
                 << endl;

            return false;
        }

        table[name].value = value;

        return true;
    }

    Symbol get(string name)
    {
        return table[name];
    }

    void display()
    {
        cout << endl;
        cout << "========== SYMBOL TABLE =========="
             << endl;

        cout << "Name\tType\tValue\tLine"
             << endl;

        cout << "----------------------------------"
             << endl;

        for (auto &item : table)
        {
            Symbol s = item.second;

            cout << s.name << "\t"
                 << s.type << "\t"
                 << s.value << "\t"
                 << s.line << endl;
        }

        cout << "=================================="
             << endl;
    }
};