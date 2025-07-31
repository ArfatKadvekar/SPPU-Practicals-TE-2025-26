#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
#include <regex>
using namespace std;

struct OpEntry { string type, code; };
struct Symbol { string name; int address = -1; };
struct Literal { string value; int address = -1; };
struct Pool { int startIndex; };

class AssemblerPass1 {
    map<string, OpEntry> optab = {
        {"STOP", {"IS", "00"}}, {"ADD", {"IS", "01"}}, {"SUB", {"IS", "02"}},
        {"MULT", {"IS", "03"}}, {"MOVER", {"IS", "04"}}, {"MOVEM", {"IS", "05"}},
        {"COMP", {"IS", "06"}}, {"BC", {"IS", "07"}}, {"DIV", {"IS", "08"}},
        {"READ", {"IS", "09"}}, {"PRINT", {"IS", "10"}},
        {"START", {"AD", "01"}}, {"END", {"AD", "02"}}, {"ORIGIN", {"AD", "03"}},
        {"EQU", {"AD", "04"}}, {"LTORG", {"AD", "05"}},
        {"DC", {"DL", "01"}}, {"DS", {"DL", "02"}}
    };

    map<string, int> regtab = {{"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}};
    map<string, int> condtab = {{"LT", 1}, {"LE", 2}, {"EQ", 3}, {"GT", 4}, {"GE", 5}, {"ANY", 6}};

    vector<Symbol> symtab;
    vector<Literal> littab;
    vector<Pool> pooltab;
    vector<vector<string>> intermediateCode;
    vector<int> lineLCs;

    int LC = 0;
    int startingLC = 0;
    int lastProcessedLiteral = 0;

    int addSymbol(string name) {
        for (int i = 0; i < symtab.size(); i++)
            if (symtab[i].name == name) return i;
        symtab.push_back({name, -1});
        return symtab.size() - 1;
    }

    int findSymbol(string name) {
        for (int i = 0; i < symtab.size(); i++)
            if (symtab[i].name == name) return i;
        return -1;
    }

    int addLiteral(string val) {
        for (int i = lastProcessedLiteral; i < littab.size(); i++) {
            if (littab[i].value == val && littab[i].address == -1) return i;
        }
        littab.push_back({val});
        return littab.size() - 1;
    }

    string formatOperand(string op) {
        if (op.empty()) return "-";
        if (op[0] == '=') return "(L," + to_string(addLiteral(op) + 1) + ")";
        if (regtab.count(op)) return "(" + to_string(regtab[op]) + ")";
        if (condtab.count(op)) return "(" + to_string(condtab[op]) + ")";
        if (isdigit(op[0]) || op[0] == '\'' || op[0] == '"') return "(C," + op + ")";

        smatch match;
        if (regex_match(op, match, regex(R"((\w+)\s*([+-])\s*(\d+))"))) {
            string sym = match[1];
            string sign = match[2];
            int offset = stoi(match[3]);
            int symIndex = addSymbol(sym);
            return "(S," + to_string(symIndex + 1) + ")" + sign + to_string(offset);
        }

        return "(S," + to_string(addSymbol(op) + 1) + ")";
    }

    void assignLiteralAddressesBeforeCurrentLine() {
        vector<vector<string>> newLiterals;
        vector<int> newLCs;
        bool added = false;

        for (int i = lastProcessedLiteral; i < littab.size(); i++) {
            if (littab[i].address == -1) {
                littab[i].address = LC++;
                newLiterals.push_back({
                    "(DL,01)",
                    "(C," + littab[i].value.substr(2, littab[i].value.length() - 3) + ")"
                });
                newLCs.push_back(littab[i].address);
                added = true;
            }
        }

        if (added) {
            pooltab.push_back({lastProcessedLiteral + 1});
            lastProcessedLiteral = littab.size();

            // Insert before LTORG or END (which is the last entry in intermediateCode)
            for (int i = 0; i < newLiterals.size(); i++) {
                intermediateCode.insert(intermediateCode.end() - 1, newLiterals[i]);
                lineLCs.insert(lineLCs.end() - 1, newLCs[i]);
            }
        }
    }

    void processLine(string line) {
        if (line.empty()) return;

        string label, opcode, op1, op2;
        stringstream ss(line);
        ss >> label;

        if (optab.count(label)) {
            opcode = label;
        } else {
            int symIndex = findSymbol(label);
            if (symIndex == -1)
                symtab.push_back({label, LC});
            else if (symtab[symIndex].address == -1)
                symtab[symIndex].address = LC;
            ss >> opcode;
        }

        ss >> op1 >> op2;
        vector<string> icEntry = {"(" + optab[opcode].type + "," + optab[opcode].code + ")"};

        if (!op1.empty()) icEntry.push_back(formatOperand(op1));
        if (!op2.empty()) icEntry.push_back(formatOperand(op2));

        if (opcode == "START") {
            LC = stoi(op1);
            startingLC = LC;
            lineLCs.push_back(-1);
            intermediateCode.push_back(icEntry);
        } else if (opcode == "LTORG" || opcode == "END") {
            intermediateCode.push_back(icEntry);
            lineLCs.push_back(-1);
            assignLiteralAddressesBeforeCurrentLine();
        } else if (opcode == "DS") {
            lineLCs.push_back(LC);
            intermediateCode.push_back(icEntry);
            LC += stoi(op1);
        } else if (opcode == "ORIGIN") {
            smatch match;
            if (regex_match(op1, match, regex(R"((\w+)\s*([+-])\s*(\d+))"))) {
                string sym = match[1];
                string sign = match[2];
                int offset = stoi(match[3]);
                int symIndex = findSymbol(sym);
                if (symIndex != -1 && symtab[symIndex].address != -1) {
                    LC = (sign == "+") ? symtab[symIndex].address + offset : symtab[symIndex].address - offset;
                }
            } else {
                int symIndex = findSymbol(op1);
                if (symIndex != -1 && symtab[symIndex].address != -1) {
                    LC = symtab[symIndex].address;
                }
            }
            lineLCs.push_back(-1);
            intermediateCode.push_back(icEntry);
        } else if (opcode == "EQU") {
            smatch match;
            int targetIndex = findSymbol(label);
            if (regex_match(op1, match, regex(R"((\w+)\s*([+-])\s*(\d+))"))) {
                string sym = match[1];
                string sign = match[2];
                int offset = stoi(match[3]);
                int symIndex = findSymbol(sym);
                if (symIndex != -1 && symtab[symIndex].address != -1)
                    symtab[targetIndex].address = (sign == "+") ? symtab[symIndex].address + offset : symtab[symIndex].address - offset;
            } else {
                int symIndex = findSymbol(op1);
                if (symIndex != -1 && symtab[symIndex].address != -1)
                    symtab[targetIndex].address = symtab[symIndex].address;
            }
        } else {
            lineLCs.push_back(LC++);
            intermediateCode.push_back(icEntry);
        }
    }

public:
    void assemble(string filename) {
        ifstream file(filename);
        string line;
        while (getline(file, line)) processLine(line);
    }

    void showResult() {
        cout << "\n--- INTERMEDIATE CODE ---\n";
        for (int i = 0; i < intermediateCode.size(); i++) {
            if (lineLCs[i] == -1)
                cout << setw(4) << "-";
            else
                cout << setw(4) << lineLCs[i];
            for (auto& col : intermediateCode[i])
                cout << setw(15) << col;
            cout << '\n';
        }

        cout << "\n--- SYMBOL TABLE ---\n";
        cout << setw(5) << "Idx" << setw(10) << "Symbol" << setw(10) << "Addr\n";
        for (int i = 0; i < symtab.size(); i++)
            cout << setw(5) << i + 1 << setw(10) << symtab[i].name << setw(10) << symtab[i].address << '\n';

        cout << "\n--- LITERAL TABLE ---\n";
        if (littab.empty())
            cout << "NIL (No literals found)\n";
        else {
            cout << setw(5) << "Idx" << setw(10) << "Literal" << setw(10) << "Addr\n";
            for (int i = 0; i < littab.size(); i++)
                cout << setw(5) << i + 1 << setw(10) << littab[i].value << setw(10) << littab[i].address << '\n';
        }

        cout << "\n--- POOL TABLE ---\n";
        if (pooltab.empty())
            cout << "NIL (No pools created)\n";
        else {
            for (int i = 0; i < pooltab.size(); i++)
                cout << "Pool " << i + 1 << ": starts at literal #" << pooltab[i].startIndex << '\n';
        }
    }
};

int main() {
    AssemblerPass1 pass1;
    string filename;
    cout << "Enter assembly file name: ";
    cin >> filename;
    pass1.assemble(filename);
    pass1.showResult();
    return 0;
}
