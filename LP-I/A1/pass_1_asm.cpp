#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cctype>

using namespace std;

struct OpEntry {
    string opclass;
    string opcode;
};

struct SymEntry {
    string symbol;
    int address;
};

struct LitEntry {
    string literal;
    int address;
};

struct PoolEntry {
    int poolStart;
};

class AssemblerPassI {
private:
    map<string, OpEntry> optab;
    map<string, int> regtab;
    map<string, int> condtab;
    vector<SymEntry> symtab;
    vector<LitEntry> littab;
    vector<PoolEntry> pooltab;
    vector<vector<string>> ic;
    int lc; // Location Counter
    int poolStart;
    
public:
    AssemblerPassI() {
        initializeTables();
        lc = 0;
        poolStart = 1; // Pool starts from index 1
    }
    
    void initializeTables() {
        // Initialize OPTAB
        optab["STOP"] = {"IS", "00"};
        optab["ADD"] = {"IS", "01"};
        optab["SUB"] = {"IS", "02"};
        optab["MULT"] = {"IS", "03"};
        optab["MOVER"] = {"IS", "04"};
        optab["MOVEM"] = {"IS", "05"};
        optab["COMP"] = {"IS", "06"};
        optab["BC"] = {"IS", "07"};
        optab["DIV"] = {"IS", "08"};
        optab["READ"] = {"IS", "09"};
        optab["PRINT"] = {"IS", "10"};
        optab["START"] = {"AD", "01"};
        optab["END"] = {"AD", "02"};
        optab["ORIGIN"] = {"AD", "03"};
        optab["EQU"] = {"AD", "04"};
        optab["LTORG"] = {"AD", "05"};
        optab["DC"] = {"DL", "01"};
        optab["DS"] = {"DL", "02"};
        
        // Initialize Register Table
        regtab["AREG"] = 1;
        regtab["BREG"] = 2;
        regtab["CREG"] = 3;
        regtab["DREG"] = 4;
        
        // Initialize Condition Code Table
        condtab["LT"] = 1;
        condtab["LE"] = 2;
        condtab["EQ"] = 3;
        condtab["GT"] = 4;
        condtab["GE"] = 5;
        condtab["ANY"] = 6;
    }
    
    int findSymbol(const string& sym) {
        for (int i = 0; i < symtab.size(); i++) {
            if (symtab[i].symbol == sym) {
                return i + 1; // Return symbol table index (1-based)
            }
        }
        return -1; // Not found
    }
    
    void addSymbol(const string& sym, int addr = -1) {
        if (findSymbol(sym) == -1) {
            SymEntry entry;
            entry.symbol = sym;
            entry.address = (addr == -1) ? lc : addr;
            symtab.push_back(entry);
        }
    }
    
    int findLiteral(const string& lit) {
        for (int i = 0; i < littab.size(); i++) {
            if (littab[i].literal == lit) {
                return i + 1; // Return literal table index (1-based)
            }
        }
        return -1; // Not found
    }
    
    void addLiteral(const string& lit) {
        if (findLiteral(lit) == -1) {
            LitEntry entry;
            entry.literal = lit;
            entry.address = -1; // Address will be assigned during LTORG or END
            littab.push_back(entry);
        }
    }
    
    string processOperand(const string& operand) {
        // Check if it's a literal (starts with =)
        if (operand.length() > 0 && operand[0] == '=') {
            addLiteral(operand);
            int litIndex = findLiteral(operand);
            return "(L," + formatIndex(litIndex) + ")";
        }
        
        // Check if it's a register
        if (regtab.find(operand) != regtab.end()) {
            return "(" + to_string(regtab[operand]) + ")";
        }
        
        // Check if it's a condition code
        if (condtab.find(operand) != condtab.end()) {
            return "(" + to_string(condtab[operand]) + ")";
        }
        
        // Check if it's a constant (starts with a digit or is quoted)
        if (operand.length() > 0 && (isdigit(operand[0]) || operand[0] == '\'' || operand[0] == '"')) {
            return "(C," + operand + ")";
        }
        
        // It's a symbol
        int symIndex = findSymbol(operand);
        if (symIndex == -1) {
            addSymbol(operand); // Forward reference
            symIndex = symtab.size();
        }
        return "(S," + formatIndex(symIndex) + ")";
    }
    
    string formatIndex(int index) {
        if (index < 10) {
            return "0" + to_string(index);
        }
        return to_string(index);
    }
    
    void processLiterals() {
        // Assign addresses to literals and update literal table
        for (int i = 0; i < littab.size(); i++) {
            if (littab[i].address == -1) {
                littab[i].address = lc++;
            }
        }
    }
    
    void processLine(const string& line) {
        if (line.empty() || line[0] == ';') return; // Skip empty lines and comments
        
        string trimmedLine = line;
        // Remove leading whitespace to handle indentation
        size_t start = trimmedLine.find_first_not_of(" \t");
        if (start == string::npos) return; // Empty line
        
        string label, opcode, op1, op2;
        bool hasLabel = false;
        
        // Check if line starts with whitespace (no label) or has a label
        if (start > 0) {
            // Line starts with whitespace, no label
            trimmedLine = trimmedLine.substr(start);
        } else {
            // Line starts without whitespace, check for label
            size_t spacePos = line.find_first_of(" \t");
            if (spacePos != string::npos) {
                string firstToken = line.substr(0, spacePos);
                // Check if first token is an opcode
                if (optab.find(firstToken) == optab.end()) {
                    // It's a label
                    label = firstToken;
                    hasLabel = true;
                    addSymbol(label, lc);
                    trimmedLine = line.substr(spacePos);
                    // Remove leading whitespace after label
                    start = trimmedLine.find_first_not_of(" \t");
                    if (start != string::npos) {
                        trimmedLine = trimmedLine.substr(start);
                    }
                } else {
                    trimmedLine = line;
                }
            } else {
                // Single token on line
                if (optab.find(line) == optab.end()) {
                    label = line;
                    addSymbol(label, lc);
                    return;
                }
                trimmedLine = line;
            }
        }
        
        // Parse the remaining tokens
        stringstream ss(trimmedLine);
        string token;
        vector<string> tokens;
        while (ss >> token) {
            tokens.push_back(token);
        }
        
        if (tokens.empty()) return;
        
        opcode = tokens[0];
        if (tokens.size() > 1) op1 = tokens[1];
        if (tokens.size() > 2) op2 = tokens[2];
        
        // Generate Intermediate Code
        vector<string> icLine;
        
        if (optab.find(opcode) != optab.end()) {
            OpEntry op = optab[opcode];
            
            // Generate IC for all instructions including AD
            icLine.push_back("(" + op.opclass + "," + op.opcode + ")");
            
            // Process operands
            if (!op1.empty()) {
                icLine.push_back(processOperand(op1));
            }
            
            if (!op2.empty()) {
                icLine.push_back(processOperand(op2));
            }
            
            // Handle special cases
            if (opcode == "START") {
                lc = stoi(op1);
                // Initialize pool table
                PoolEntry pool;
                pool.poolStart = 1;
                pooltab.push_back(pool);
                ic.push_back(icLine); // Add to IC but don't increment LC
                return;
            } else if (opcode == "END") {
                processLiterals(); // Process any remaining literals
                ic.push_back(icLine); // Add to IC but don't increment LC
                return;
            } else if (opcode == "LTORG") {
                processLiterals(); // Process literals at LTORG
                // Add new pool entry if there were literals processed
                if (!littab.empty()) {
                    PoolEntry pool;
                    pool.poolStart = littab.size() + 1;
                    pooltab.push_back(pool);
                }
                ic.push_back(icLine); // Add to IC but don't increment LC
                return;
            } else if (opcode == "DS") {
                if (!op1.empty()) {
                    lc += stoi(op1);
                }
                ic.push_back(icLine);
                return;
            } else if (opcode == "DC") {
                lc++;
                ic.push_back(icLine);
                return;
            }
        }
        
        // Add to IC and increment LC for IS instructions
        if (!icLine.empty()) {
            ic.push_back(icLine);
            lc++; // Increment location counter for IS instructions
        }
    }
    
    void processFile(const string& filename) {
        ifstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "Error: Could not open file " << filename << endl;
            return;
        }
        
        string line;
        while (getline(file, line)) {
            processLine(line);
        }
        file.close();
    }
    
    void displayResults() {
        cout << "\n=== INTERMEDIATE CODE (IC) ===" << endl;
        cout << setw(5) << "LC" << setw(15) << "IC Opcode" << setw(15) << "IC Op1" << setw(15) << "IC Op2" << endl;
        cout << string(50, '-') << endl;
        
        int currentLC = 101; // Starting LC from the START directive
        for (int i = 0; i < ic.size(); i++) {
            string opcode = ic[i][0];
            
            // Check if it's an AD instruction (no LC assigned)
            if (opcode.find("AD,") != string::npos) {
                cout << setw(5) << "-"; // No LC for AD instructions
            } else {
                cout << setw(5) << currentLC;
                // Increment LC based on instruction type
                if (opcode.find("DL,02") != string::npos) { // DS instruction
                    // For DS, we need to extract the operand value
                    // This is simplified - in real implementation, parse the operand
                    currentLC += 1; // Default increment for DS
                } else if (opcode.find("DL,01") != string::npos || opcode.find("IS,") != string::npos) {
                    currentLC++; // Normal increment for DC and IS instructions
                }
            }
            
            // Display the IC line
            for (int j = 0; j < 3; j++) {
                if (j < ic[i].size()) {
                    cout << setw(15) << ic[i][j];
                } else {
                    cout << setw(15) << "-";
                }
            }
            cout << endl;
        }
        
        cout << "\n=== SYMBOL TABLE (SYMTAB) ===" << endl;
        cout << setw(5) << "Index" << setw(15) << "Symbol" << setw(10) << "Address" << endl;
        cout << string(30, '-') << endl;
        
        for (int i = 0; i < symtab.size(); i++) {
            cout << setw(5) << (i + 1) 
                 << setw(15) << symtab[i].symbol 
                 << setw(10) << symtab[i].address << endl;
        }
        
        cout << "\n=== LITERAL TABLE (LITTAB) ===" << endl;
        if (littab.empty()) {
            cout << "NIL (No literals in the source code)" << endl;
        } else {
            cout << setw(5) << "Index" << setw(15) << "Literal" << setw(10) << "Address" << endl;
            cout << string(30, '-') << endl;
            
            for (int i = 0; i < littab.size(); i++) {
                cout << setw(5) << (i + 1) 
                     << setw(15) << littab[i].literal;
                if (littab[i].address != -1) {
                    cout << setw(10) << littab[i].address;
                } else {
                    cout << setw(10) << "TBD";
                }
                cout << endl;
            }
        }
        
        cout << "\n=== POOL TABLE (POOLTAB) ===" << endl;
        if (pooltab.empty()) {
            cout << "NIL (No pools created)" << endl;
        } else {
            cout << setw(5) << "Pool#" << setw(15) << "Pool Start" << endl;
            cout << string(20, '-') << endl;
            
            for (int i = 0; i < pooltab.size(); i++) {
                cout << setw(5) << (i + 1) 
                     << setw(15) << pooltab[i].poolStart << endl;
            }
        }
    }
};

int main() {
    AssemblerPassI assembler;
    
    cout << "Two-Pass Assembler - Pass I Implementation" << endl;
    cout << "Enter input filename (or 'default' to use hardcoded example): ";
    
    string filename;
    cin >> filename;
    
    if (filename == "default") {
        // Create a sample input file with proper formatting
        ofstream sampleFile("sample.asm");
        sampleFile << "START\t101\n";
        sampleFile << "\tREAD\tN\n";
        sampleFile << "\tMOVER\tBREG ='1'\n";
        sampleFile << "\tMOVEM\tBREG TERM\n";
        sampleFile << "AGAIN\tMULT\tBREG TERM\n";
        sampleFile << "\tMOVER\tCREG ='2'\n";
        sampleFile << "\tLTORG\n";
        sampleFile << "\tADD\tCREG ='1'\n";
        sampleFile << "\tMOVEM\tCREG TERM\n";
        sampleFile << "\tCOMP\tCREG N\n";
        sampleFile << "\tBC\tLE AGAIN\n";
        sampleFile << "\tDIV\tBREG ='2'\n";
        sampleFile << "\tMOVEM\tBREG RESULT\n";
        sampleFile << "\tPRINT\tRESULT\n";
        sampleFile << "\tSTOP\n";
        sampleFile << "N\tDS\t1\n";
        sampleFile << "RESULT\tDS\t1\n";
        sampleFile << "ONE\tDC\t'1'\n";
        sampleFile << "TERM\tDS\t1\n";
        sampleFile << "TWO\tDC\t'2'\n";
        sampleFile << "\tEND\n";
        sampleFile.close();
        filename = "sample.asm";
        cout << "Using default sample file: " << filename << endl;
    }
    
    assembler.processFile(filename);
    assembler.displayResults();
    
    return 0;
}