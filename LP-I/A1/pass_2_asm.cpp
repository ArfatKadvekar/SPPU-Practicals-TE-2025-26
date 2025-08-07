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
struct MachineCode { int address; string code; };

class TwoPassAssembler {
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
    vector<MachineCode> machineCode;

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

    void assignLiteralAddresses() {
        bool hasUnprocessedLiterals = false;
        
        for (int i = lastProcessedLiteral; i < littab.size(); i++) {
            if (littab[i].address == -1) {
                hasUnprocessedLiterals = true;
                break;
            }
        }
        
        if (!hasUnprocessedLiterals) return;
        
        pooltab.push_back({lastProcessedLiteral + 1});
        
        for (int i = lastProcessedLiteral; i < littab.size(); i++) {
            if (littab[i].address == -1) {
                littab[i].address = LC;
                
                vector<string> literalIC = {
                    "(DL,01)",
                    "(C," + littab[i].value.substr(2, littab[i].value.length() - 3) + ")"
                };
                
                intermediateCode.push_back(literalIC);
                lineLCs.push_back(LC);
                LC++;
            }
        }
        
        lastProcessedLiteral = littab.size();
    }

    void processLine(string line) {
        if (line.empty()) return;

        string label, opcode, op1, op2;
        stringstream ss(line);
        ss >> label;

        if (optab.count(label)) {
            opcode = label;
            label = "";
        } else {
            if (!label.empty()) {
                int symIndex = findSymbol(label);
                if (symIndex == -1)
                    symtab.push_back({label, LC});
                else if (symtab[symIndex].address == -1)
                    symtab[symIndex].address = LC;
            }
            ss >> opcode;
        }

        ss >> op1 >> op2;
        
        if (opcode.empty()) return;
        
        vector<string> icEntry = {"(" + optab[opcode].type + "," + optab[opcode].code + ")"};

        if (!op1.empty()) icEntry.push_back(formatOperand(op1));
        if (!op2.empty()) icEntry.push_back(formatOperand(op2));

        if (opcode == "START") {
            LC = stoi(op1);
            startingLC = LC;
            lineLCs.push_back(-1);
            intermediateCode.push_back(icEntry);
        } else if (opcode == "LTORG") {
            lineLCs.push_back(-1);
            intermediateCode.push_back(icEntry);
            assignLiteralAddresses();
        } else if (opcode == "END") {
            lineLCs.push_back(-1);
            intermediateCode.push_back(icEntry);
            assignLiteralAddresses();
        } else if (opcode == "DS") {
            lineLCs.push_back(LC);
            intermediateCode.push_back(icEntry);
            LC += stoi(op1);
        } else if (opcode == "ORIGIN") {
            lineLCs.push_back(-1);
            intermediateCode.push_back(icEntry);
            
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
        } else if (opcode == "EQU") {
            int targetIndex = findSymbol(label);
            smatch match;
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
            lineLCs.push_back(-1);
            intermediateCode.push_back(icEntry);
        } else {
            lineLCs.push_back(LC);
            intermediateCode.push_back(icEntry);
            LC++;
        }
    }

    // PASS 2 METHODS
    string resolveOperand(string operand) {
        if (operand == "-") return "";
        
        // Extract type and value from operand like "(S,1)" or "(L,2)"
        if (operand[0] != '(') return operand;
        
        size_t comma = operand.find(',');
        if (comma == string::npos) {
            // Register or condition code like "(1)" or "(6)"
            return operand.substr(1, operand.length() - 2);
        }
        
        string type = operand.substr(1, comma - 1);
        string rest = operand.substr(comma + 1);
        
        if (type == "S") {
            // Symbol reference
            size_t closeParen = rest.find(')');
            int symIndex = stoi(rest.substr(0, closeParen)) - 1;
            string result = to_string(symtab[symIndex].address);
            
            // Handle offset like "(S,1)+2"
            if (closeParen + 1 < rest.length()) {
                string offset = rest.substr(closeParen + 1);
                result += offset;
            }
            return result;
        } else if (type == "L") {
            // Literal reference
            size_t closeParen = rest.find(')');
            int litIndex = stoi(rest.substr(0, closeParen)) - 1;
            return to_string(littab[litIndex].address);
        } else if (type == "C") {
            // Constant
            return rest.substr(0, rest.length() - 1);
        }
        
        return operand;
    }

    void generateMachineCode() {
        for (int i = 0; i < intermediateCode.size(); i++) {
            if (lineLCs[i] == -1) continue; // Skip assembler directives
            
            string opcode = intermediateCode[i][0];
            
            // Extract opcode type and number
            size_t comma = opcode.find(',');
            string type = opcode.substr(1, comma - 1);
            string opcodeNum = opcode.substr(comma + 1, opcode.length() - comma - 2);
            
            if (type == "IS") {
                // Imperative Statement - generate machine code
                string machineInstr = opcodeNum;
                
                // Add operands
                for (int j = 1; j < intermediateCode[i].size(); j++) {
                    string resolvedOperand = resolveOperand(intermediateCode[i][j]);
                    if (!resolvedOperand.empty()) {
                        machineInstr += " " + resolvedOperand;
                    }
                }
                
                machineCode.push_back({lineLCs[i], machineInstr});
            } else if (type == "DL") {
                // Declarative Statement
                if (opcodeNum == "01") {
                    // DC - Define Constant - format like "00 0 001"
                    string value = resolveOperand(intermediateCode[i][1]);
                    machineCode.push_back({lineLCs[i], "00 0 " + string(3 - value.length(), '0') + value});
                } else if (opcodeNum == "02") {
                    // DS - Define Space - NO machine code generated
                    machineCode.push_back({lineLCs[i], "No machine code"});
                }
            }
        }
    }

public:
    void pass1(string filename) {
        cout << "\n=== PASS 1: PROCESSING ===\n";
        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            size_t start = line.find_first_not_of(" \t");
            if (start != string::npos) {
                size_t end = line.find_last_not_of(" \t");
                line = line.substr(start, end - start + 1);
            } else {
                line = "";
            }
            processLine(line);
        }
        file.close();
        cout << "Pass 1 completed successfully!\n";
    }

    void pass2() {
        cout << "\n=== PASS 2: GENERATING MACHINE CODE ===\n";
        generateMachineCode();
        cout << "Pass 2 completed successfully!\n";
    }

    void showPass1Results() {
        cout << "\n--- INTERMEDIATE CODE (Pass 1) ---\n";
        cout << setw(4) << "LC" << setw(15) << "Opcode" << setw(15) << "Operand1" << setw(15) << "Operand2\n";
        cout << string(60, '-') << '\n';
        
        for (int i = 0; i < intermediateCode.size(); i++) {
            if (lineLCs[i] == -1)
                cout << setw(4) << "-";
            else
                cout << setw(4) << lineLCs[i];
            
            for (int j = 0; j < intermediateCode[i].size(); j++)
                cout << setw(15) << intermediateCode[i][j];
            
            for (int j = intermediateCode[i].size(); j < 3; j++)
                cout << setw(15) << "-";
            
            cout << '\n';
        }

        cout << "\n--- SYMBOL TABLE ---\n";
        cout << setw(5) << "Idx" << setw(12) << "Symbol" << setw(10) << "Address\n";
        cout << string(30, '-') << '\n';
        for (int i = 0; i < symtab.size(); i++)
            cout << setw(5) << i + 1 << setw(12) << symtab[i].name << setw(10) << symtab[i].address << '\n';

        cout << "\n--- LITERAL TABLE ---\n";
        if (littab.empty()) {
            cout << "NIL (No literals found)\n";
        } else {
            cout << setw(5) << "Idx" << setw(12) << "Literal" << setw(10) << "Address\n";
            cout << string(30, '-') << '\n';
            for (int i = 0; i < littab.size(); i++)
                cout << setw(5) << i + 1 << setw(12) << littab[i].value << setw(10) << littab[i].address << '\n';
        }

        cout << "\n--- POOL TABLE ---\n";
        if (pooltab.empty()) {
            cout << "NIL (No pools created)\n";
        } else {
            for (int i = 0; i < pooltab.size(); i++)
                cout << "Pool " << i + 1 << ": starts at literal #" << pooltab[i].startIndex << '\n';
        }
    }

    void showPass2Results() {
        cout << "\n--- MACHINE CODE (Pass 2) ---\n";
        cout << setw(8) << "Address" << setw(20) << "Machine Code\n";
        cout << string(30, '-') << '\n';
        
        for (auto& mc : machineCode) {
            cout << setw(8) << mc.address << setw(20) << mc.code << '\n';
        }
    }

    void showCompleteResults() {
        showPass1Results();
        showPass2Results();
    }
};

int main() {
    TwoPassAssembler assembler;
    string filename;
    
    cout << "Two-Pass Assembler\n";
    cout << "==================\n";
    cout << "Enter assembly file name: ";
    cin >> filename;
    
    // Pass 1
    assembler.pass1(filename);
    
    // Pass 2
    assembler.pass2();
    
    // Show results
    assembler.showCompleteResults();
    
    return 0;
}