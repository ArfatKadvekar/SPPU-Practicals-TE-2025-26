#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

struct Parameter {
    string name;
    string defaultValue;
    bool isKeyword;
};

struct MacroDefinition {
    string name;
    vector<Parameter> parameters;
    vector<string> body;
    int positionalCount;
    int keywordCount;
};

struct MDTEntry {
    int index;
    string statement;
};

struct MNTEntry {
    string name;
    int pp;        // #PP - Number of Positional Parameters
    int kp;        // #KP - Number of Keyword Parameters  
    int mdtp;      // MDTP - Macro Definition Table Pointer
    int kpdtp;     // KPDTP - Keyword Parameter Default Table Pointer
};

struct KPDTEntry {
    int index;
    string paramName;
    string defaultValue;
};

struct PNTEntry {
    int index;
    string paramName;
};

struct ALAEntry {
    string formal;
    string actual;
};

struct ICEntry {
    int lc;
    string statement;
    string type;
};

class MacroProcessor {
private:
    vector<MacroDefinition> macros;
    vector<MNTEntry> mnt;     // Macro Name Table
    vector<MDTEntry> mdt;     // Macro Definition Table
    vector<KPDTEntry> kpdtab; // Keyword Parameter Default Table
    vector<vector<PNTEntry>> pntab; // Parameter Name Table (per macro)
    vector<ICEntry> intermediateCode;
    vector<string> expandedCode;
    map<string, MacroDefinition> macroMap;
    map<string, int> macroToMntIndex;
    
public:
    void readInputFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Cannot open input file " << filename << endl;
            return;
        }
        
        string line;
        bool inMacro = false;
        MacroDefinition currentMacro;
        int lc = 100;
        
        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty()) continue;
            
            stringstream ss(line);
            string token;
            ss >> token;
            
            if (token == "MACRO") {
                inMacro = true;
                ss >> currentMacro.name;
                currentMacro.parameters.clear();
                currentMacro.body.clear();
                currentMacro.positionalCount = 0;
                currentMacro.keywordCount = 0;
                
                // Parse parameters - collect all parameters first
                vector<string> paramTokens;
                string param;
                while (ss >> param) {
                    if (param.back() == ',') {
                        param.pop_back();
                    }
                    paramTokens.push_back(param);
                }
                
                // Process parameters to determine positional vs keyword
                bool keywordSection = false;
                bool hasError = false;
                
                for (const string& paramToken : paramTokens) {
                    Parameter p;
                    
                    // Check if it's a keyword parameter (contains =)
                    size_t eqPos = paramToken.find('=');
                    if (eqPos != string::npos) {
                        keywordSection = true;
                        p.name = paramToken.substr(0, eqPos);
                        p.defaultValue = paramToken.substr(eqPos + 1);
                        p.isKeyword = true;
                        currentMacro.keywordCount++;
                        currentMacro.parameters.push_back(p);
                    } else {
                        if (keywordSection) {
                            cout << "Error: Positional parameter " << paramToken << " cannot appear after keyword parameters!" << endl;
                            hasError = true;
                            // Still count it as positional for completeness but mark error
                        }
                        p.name = paramToken;
                        p.defaultValue = "";
                        p.isKeyword = false;
                        currentMacro.positionalCount++;
                        currentMacro.parameters.push_back(p);
                    }
                }
                
                if (hasError) {
                    cout << "Warning: Macro " << currentMacro.name << " has invalid parameter ordering but will be processed." << endl;
                }
                
                cout << "Debug: Macro " << currentMacro.name << " - Positional: " << currentMacro.positionalCount << ", Keyword: " << currentMacro.keywordCount << endl;
            }
            else if (token == "MEND") {
                inMacro = false;
                macros.push_back(currentMacro);
                macroMap[currentMacro.name] = currentMacro;
            }
            else if (inMacro) {
                currentMacro.body.push_back(line);
            }
            else {
                // Add all non-macro statements to intermediate code
                ICEntry entry;
                entry.lc = lc++;
                entry.statement = line;
                entry.type = "NORMAL";
                intermediateCode.push_back(entry);
            }
        }
        file.close();
    }
    
    void passOne() {
        cout << "=== PASS 1: PROCESSING ===\n";
        
        int mdtIndex = 1;
        int kpdtIndex = 1;
        
        // Build MNT, MDT, KPDTAB, and PNTAB
        for (int macroIdx = 0; macroIdx < macros.size(); macroIdx++) {
            const auto& macro = macros[macroIdx];
            
            // Build MNT entry
            MNTEntry mntEntry;
            mntEntry.name = macro.name;
            mntEntry.pp = macro.positionalCount;
            mntEntry.kp = macro.keywordCount;
            mntEntry.mdtp = mdtIndex;
            mntEntry.kpdtp = (macro.keywordCount > 0) ? kpdtIndex : 0;
            mnt.push_back(mntEntry);
            macroToMntIndex[macro.name] = macroIdx;
            
            // Build PNTAB for this macro
            vector<PNTEntry> currentPNT;
            for (int i = 0; i < macro.parameters.size(); i++) {
                PNTEntry pntEntry;
                pntEntry.index = i + 1;
                pntEntry.paramName = macro.parameters[i].name;
                currentPNT.push_back(pntEntry);
            }
            pntab.push_back(currentPNT);
            
            // Build KPDTAB entries for keyword parameters
            for (const auto& param : macro.parameters) {
                if (param.isKeyword) {
                    KPDTEntry kpdtEntry;
                    kpdtEntry.index = kpdtIndex++;
                    kpdtEntry.paramName = param.name;
                    kpdtEntry.defaultValue = param.defaultValue;
                    kpdtab.push_back(kpdtEntry);
                }
            }
            
            // Build MDT entries
            for (const auto& stmt : macro.body) {
                MDTEntry mdtEntry;
                mdtEntry.index = mdtIndex++;
                // Convert parameters to (P,n) format
                string processedStmt = convertToParameterFormat(stmt, macro);
                mdtEntry.statement = processedStmt;
                mdt.push_back(mdtEntry);
            }
            
            // Add MEND to MDT
            MDTEntry mendEntry;
            mendEntry.index = mdtIndex++;
            mendEntry.statement = "MEND";
            mdt.push_back(mendEntry);
        }
        
        // Check for macro calls in intermediate code
        for (auto& entry : intermediateCode) {
            stringstream ss(entry.statement);
            string firstToken;
            ss >> firstToken;
            
            if (macroMap.find(firstToken) != macroMap.end()) {
                entry.type = "MACRO_CALL";
            }
        }
        
        cout << "Pass 1 completed successfully!\n\n";
    }
    
    string convertToParameterFormat(const string& stmt, const MacroDefinition& macro) {
        string result = stmt;
        
        // Replace each parameter with (P,n) format
        for (int i = 0; i < macro.parameters.size(); i++) {
            string paramName = macro.parameters[i].name;
            string replacement = "(P," + to_string(i + 1) + ")";
            
            size_t pos = 0;
            while ((pos = result.find(paramName, pos)) != string::npos) {
                // Check if it's a whole word match (not part of another word)
                bool isWholeWord = true;
                if (pos > 0 && (isalnum(result[pos-1]) || result[pos-1] == '_')) {
                    isWholeWord = false;
                }
                if (pos + paramName.length() < result.length() && 
                    (isalnum(result[pos + paramName.length()]) || result[pos + paramName.length()] == '_')) {
                    isWholeWord = false;
                }
                
                if (isWholeWord) {
                    result.replace(pos, paramName.length(), replacement);
                    pos += replacement.length();
                } else {
                    pos += paramName.length();
                }
            }
        }
        
        return result;
    }
    
    vector<ALAEntry> parseArguments(const string& stmt, const MacroDefinition& macro) {
        vector<ALAEntry> ala;
        stringstream ss(stmt);
        string macroName;
        ss >> macroName; // Skip macro name
        
        // Initialize ALA with default values for keyword parameters
        for (const auto& param : macro.parameters) {
            ALAEntry entry;
            entry.formal = param.name;
            entry.actual = param.isKeyword ? param.defaultValue : "";
            ala.push_back(entry);
        }
        
        string arg;
        int positionalIndex = 0;
        
        while (ss >> arg) {
            if (arg.back() == ',') {
                arg.pop_back();
            }
            
            // Check if it's a keyword argument (contains =)
            size_t eqPos = arg.find('=');
            if (eqPos != string::npos) {
                string paramName = arg.substr(0, eqPos);
                string paramValue = arg.substr(eqPos + 1);
                
                // Find the parameter in ALA and update it
                for (auto& entry : ala) {
                    if (entry.formal == paramName) {
                        entry.actual = paramValue;
                        break;
                    }
                }
            } else {
                // Positional argument
                if (positionalIndex < ala.size() && !macro.parameters[positionalIndex].isKeyword) {
                    ala[positionalIndex].actual = arg;
                    positionalIndex++;
                }
            }
        }
        
        return ala;
    }
    
    string substituteParameters(const string& stmt, const vector<ALAEntry>& ala) {
        string result = stmt;
        
        // Replace (P,n) with actual parameter values
        for (int i = 0; i < ala.size(); i++) {
            string paramPattern = "(P," + to_string(i + 1) + ")";
            size_t pos = 0;
            while ((pos = result.find(paramPattern, pos)) != string::npos) {
                result.replace(pos, paramPattern.length(), ala[i].actual);
                pos += ala[i].actual.length();
            }
        }
        
        return result;
    }
    
    void passTwo() {
        cout << "=== PASS 2: MACRO EXPANSION ===\n";
        
        int lc = 100;
        
        for (const auto& icEntry : intermediateCode) {
            if (icEntry.type == "MACRO_CALL") {
                stringstream ss(icEntry.statement);
                string macroName;
                ss >> macroName;
                
                if (macroMap.find(macroName) != macroMap.end()) {
                    const MacroDefinition& macro = macroMap[macroName];
                    vector<ALAEntry> ala = parseArguments(icEntry.statement, macro);
                    
                    // Find MDT entries for this macro
                    int mdtStart = -1;
                    int mdtEnd = -1;
                    for (const auto& mntEntry : mnt) {
                        if (mntEntry.name == macroName) {
                            mdtStart = mntEntry.mdtp;
                            break;
                        }
                    }
                    
                    if (mdtStart != -1) {
                        // Find the end of this macro's MDT entries (before MEND of this macro)
                        for (int i = 0; i < mdt.size(); i++) {
                            if (mdt[i].index >= mdtStart && mdt[i].statement == "MEND") {
                                mdtEnd = mdt[i].index;
                                break;
                            }
                        }
                        
                        // Expand macro body (exclude MEND)
                        for (const auto& mdtEntry : mdt) {
                            if (mdtEntry.index >= mdtStart && mdtEntry.index < mdtEnd) {
                                string expandedStmt = substituteParameters(mdtEntry.statement, ala);
                                expandedCode.push_back(expandedStmt);
                            }
                        }
                    }
                }
            } else {
                expandedCode.push_back(icEntry.statement);
            }
        }
        
        cout << "Pass 2 completed successfully!\n\n";
    }
    
    void displayResults() {
        cout << "1. Pass 1 output:\n\n";
        
        // Display MNT
        cout << "MNT\n";
        cout << left << setw(8) << "Name" << setw(6) << "#PP" << setw(6) << "#KP" << setw(8) << "MDTP" << setw(8) << "KPDTP" << endl;
        for (const auto& entry : mnt) {
            cout << left << setw(8) << entry.name 
                 << setw(6) << entry.pp 
                 << setw(6) << entry.kp
                 << setw(8) << entry.mdtp
                 << setw(8) << (entry.kpdtp == 0 ? "NULL" : to_string(entry.kpdtp)) << endl;
        }
        
        // Display KPDTAB
        if (!kpdtab.empty()) {
            cout << "\nKPDTAB\n";
            cout << left << setw(6) << "Index" << setw(10) << "Param" << setw(12) << "Default" << endl;
            for (const auto& entry : kpdtab) {
                cout << left << setw(6) << entry.index 
                     << setw(10) << entry.paramName 
                     << setw(12) << entry.defaultValue << endl;
            }
        }
        
        // Display MDT
        cout << "\nMDT\n";
        for (const auto& entry : mdt) {
            cout << left << setw(4) << entry.index << entry.statement << endl;
        }
        
        // Display PNTAB for each macro
        for (int i = 0; i < macros.size(); i++) {
            cout << "\nPNTAB(" << macros[i].name << ")\n";
            for (const auto& pntEntry : pntab[i]) {
                cout << left << setw(4) << pntEntry.index << pntEntry.paramName << endl;
            }
        }
        
        // Display Expanded Code
        cout << "\n--- EXPANDED CODE (Pass 2) ---\n";
        cout << left << setw(6) << "LC" << setw(35) << "Statement" << endl;
        cout << string(41, '-') << endl;
        
        int lc = 100;
        for (const auto& stmt : expandedCode) {
            cout << left << setw(6) << lc << setw(35) << stmt << endl;
            lc++;
        }
    }
};

int main() {
    cout << "Two-Pass Macro Processor\n";
    cout << "========================\n";
    
    MacroProcessor processor;
    
    string inputFile;
    cout << "Enter macro file name (or press Enter for default 'input.txt'): ";
    getline(cin, inputFile);
    
    if (inputFile.empty()) {
        inputFile = "input.txt";
    }
    
    processor.readInputFile(inputFile);
    processor.passOne();
    processor.passTwo();
    processor.displayResults();
    
    return 0;
}