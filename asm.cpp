/*-----------------------------------------------------------------------
TITLE: Assembler (asm.cpp)
AUTHOR: NIKHIL PATHAK (2101CS50)
Declaration of Authorship
This file, asm.cpp, is part of the assignment of CS209/CS210 at the
department of Computer Science and Engineering, IIT Patna .
-------------------------------------------------------------------------*/

#include <bits/stdc++.h>
using namespace std;

map<int, string> warnings; // stores warnings
map<int, string> errors;   // stores errors

// Storing line with its program counter
struct lineInfo
{
    int prcont;
    string label, mnemonic, operand, prevOperand;
};

// Storing line information to be used in listing file
struct listInfo
{
    string address, macCode, statement;
};
vector<lineInfo> Lines;
vector<listInfo> List;       // Listing file information
vector<string> MachineCodes; // Machines codes in 8 bit hex form

map<string, pair<int, int>> symTab;    // {label, {address, lineNum}}
map<int, string> comments;             // {line, comment}
map<string, pair<string, int>> opcode; // {mnemonic, opcode, type of operand}
map<string, vector<int>> locLabels;    // {labels, {list of line numbers where this label is ever used}}
map<string, string> setVars;           // {variables(label), value associated}

void storeopcodes()
{
    /* We keep the opcodes in a format where the key is the opcode and it points to a pair
       where the first part refers to the opcode's hex conversion and the second part refers to the type of opcode
       requiring offset or value or nothing
        type 0 : nothing required
        type 1 : value required
        type 2 : offset required  */

    opcode["data"] = {"", 1};
    opcode["ldc"] = {"00", 1};
    opcode["adc"] = {"01", 1};
    opcode["ldl"] = {"02", 2};
    opcode["stl"] = {"03", 2};
    opcode["ldnl"] = {"04", 2};
    opcode["stnl"] = {"05", 2};
    opcode["add"] = {"06", 0};
    opcode["sub"] = {"07", 0};
    opcode["shl"] = {"08", 0};
    opcode["shr"] = {"09", 0};
    opcode["adj"] = {"0a", 1};
    opcode["a2sp"] = {"0b", 0};
    opcode["sp2a"] = {"0c", 0};
    opcode["call"] = {"0d", 2};
    opcode["return"] = {"0e", 0};
    opcode["brz"] = {"0f", 2};
    opcode["brlz"] = {"10", 2};
    opcode["br"] = {"11", 2};
    opcode["HALT"] = {"12", 0};
    opcode["SET"] = {"", 1};
}

class checker
{
public:
    // check alphabet
    int alphabet(char b)
    {
        if ((b >= 'a' && b <= 'z') || (b >= 'A' && b <= 'Z') || b == '_')
            return 1;
        else
            return 0;
    }
    // check digit
    int digit(char a)
    {
        if (a >= '0' && a <= '9')
            return 1;
        else
            return 0;
    }
    // check decimal
    int decimal(string s)
    {
        int ch = 1;
        if (s[0] == '0' && s.size() != 1)
            ch = 0;
        for (int i = 1; i < s.size(); i++)
        {
            if (s[i] >= '0' && s[i] <= '9')
                continue;
            else
            {
                ch = 0;
                break;
            }
        }
        return ch;
    }
    // check octal
    int octal(string s)
    {
        int ch = 1;
        int siz = s.size();
        if (siz < 2 || s[0] != '0')
            return 0;
        for (int i = 1; i < s.size(); i++)
        {
            if (s[i] >= '0' && s[i] <= '7')
                continue;
            else
                ch = 0;
        }

        return ch;
    }
    // check hexadecimal
    int hexadecimal(string s)
    {
        int ch = 1;
        int siz = s.size();
        if (siz < 3 || s[0] != '0' || tolower(s[1]) != 'x')
            return 0;
        for (int i = 2; i < s.size(); i++)
        {
            if ((s[i] >= '0' && s[i] <= '9') || (tolower(s[i]) >= 'a' && tolower(s[i]) <= 'f'))
                continue;
            else
                ch = 0;
        }
        return ch;
    }
    // checks if a string is a valid label
    int val_label(string s)
    {
        int siz = s.size();
        if (siz == 0)
            return 0;
        int ch = 1;
        if (!alphabet(s[0]))
            ch = 0;
        for (int i = 1; i < siz; i++)
        {
            if (alphabet(s[i]) || digit(s[i]))
                continue;
            else
                ch = 0;
        }
        return ch;
    }
};
checker chk;

// Following class contains different types of converter functions which converts from one base to another
class converter
{
public:
    
    string dec_hex(int num)
    {
        unsigned int num1 = num;
        string res = "";
        for (int i = 0; i < 8; i++)
        {
            int rem = num1 % 16;
            if (rem <= 9)
            {
                char ch = rem + '0';
                res += ch;
            }
            else
            {

                char ch = rem - 10 + 'a';
                res += ch;
            }
            num1 /= 16;
        }
        reverse(res.begin(), res.end());
        return res;
    }
    
    string oct_dec(string s)
    {
        int res = 0, pow = 1;
        int siz = s.size();
        for (int i = siz - 1; i >= 0; i--)
        {
            res += (pow * (s[i] - '0'));
            pow *= 8;
        }

        string dec = to_string(res);
        return dec;
    }

    string hex_dec(string s)
    {
        int res = 0, pow = 1;
        int siz = s.size();
        for (int i = siz - 1; i >= 0; i--)
        {
            if (chk.digit(s[i]))
                res += (pow * (s[i] - '0'));
            else
                res += (pow * ((tolower(s[i]) - 'a') + 10));
            pow *= 16;
        }

        string dec = to_string(res);
        return dec;
    }   
};
converter conv;

// This class contains some extra functions which we require during pass1
class solver
{
public:
    // Checking errors associated with labels
    void solveLabel(string label, int locctr, int prcont)
    {
        if (label.empty())
            return;
        bool test = chk.val_label(label);
        if (!test)
        {
            errors[locctr] = "Bogus Label name";
        }
        else
        {
            if (symTab.count(label) and symTab[label].first != -1)
            {
                errors[locctr] = "Duplicate label definition";
            }
            else
            {
                symTab[label] = {prcont, locctr};
            }
        }
    }

    // Checking errors associated with operands
    string solveOperand(string operand, int locctr)
    {
        string ret = "";
        if (chk.val_label(operand))
        {
            if (!symTab.count(operand))
            {
                symTab[operand] = {-1, locctr};                 // label used but wasn't declared so far...
            }
            locLabels[operand].push_back(locctr);
            return operand;
        }
        string temp = operand, sign = "";
        if (temp[0] == '-' or temp[0] == '+')
        {
            sign = temp[0];
            temp = temp.substr(1);
        }
        ret += sign;
        if (chk.octal(temp))
        {
            ret += conv.oct_dec(temp.substr(1));
        }
        else if (chk.hexadecimal(temp))
        {
            ret += conv.hex_dec(temp.substr(2));
        }
        else if (chk.decimal(temp))
        {
            ret += temp;
        }
        else
        {
            ret = "";
        }
        return ret;
    }

    // Checking errors associated with Mnemonics
    void solveMnemonics(string instName, string &operand, int locctr, int prcont, int rem, bool &flag)
    {
        if (instName.empty())
            return;
        if (!opcode.count(instName))
        {
            errors[locctr] = "Bogus Mnemonic";
        }
        else
        {
            int type = opcode[instName].second;
            int isOp = !operand.empty();
            if (type > 0)
            {
                if (!isOp)
                {
                    errors[locctr] = "Missing operand";
                }
                else if (rem > 0)
                {
                    errors[locctr] = "extra on end of line";
                }
                else
                {
                    string replaceOP = solveOperand(operand, locctr);
                    if (replaceOP.empty())
                    {
                        errors[locctr] = "Invalid format: not a valid label or a number";
                    }
                    else
                    {
                        operand = replaceOP;
                        flag = true;
                    }
                }
            }
            else if (type == 0 and isOp)
            {
                errors[locctr] = "unexpected operand";
            }
            else
            {
                flag = true;
            }
        }
    }
};
solver sol;

vector<string> extractWords(string curLine, int locctr)
{ // extract information from each line
    if (curLine.empty())
        return {};
    vector<string> res;
    stringstream curWord(curLine);
    string word;
    while (curWord >> word)
    {
        if (word.empty())
            continue;
        if (word[0] == ';')
            break;
        auto i = word.find(':');
        if (i != string::npos and word.back() != ':')
        { // case when there is no separation between ':' and the statement
            res.push_back(word.substr(0, i + 1));
            word = word.substr(i + 1);
        }
        if (word.back() == ';')
        { // case when there is no seperation between ';' and previous word
            word.pop_back();
            res.push_back(word);
            break;
        }
        res.push_back(word);
    }
    string comment = "";
    for (int i = 0; i < (int)curLine.size(); ++i)
    {
        if (curLine[i] == ';')
        {
            int j = i + 1;
            while (j < (int)curLine.size() and curLine[j] == ' ')
                ++j;
            for (; j < (int)curLine.size(); ++j)
            {
                comment += curLine[j];
            }
            break;
        }
    }
    if (!comment.empty())
        comments[locctr] = comment; // store comments
    return res;
}
//This stores the content from file in which each string is a line
void pass1(const vector<string> &readLines)
{
    int locctr = 0, prcont = 0;
    for (string curLine : readLines)
    {
        ++locctr;
        auto cur = extractWords(curLine, locctr);
        if (cur.empty())
            continue;
        string label = "", instName = "", operand = "";
        int pos = 0, sz = cur.size();
        if (cur[pos].back() == ':')
        {
            label = cur[pos];
            label.pop_back();
            ++pos;
        }
        if (pos < sz)
        {
            instName = cur[pos];
            ++pos;
        }
        if (pos < sz)
        {
            operand = cur[pos];
            ++pos;
        }
        sol.solveLabel(label, locctr, prcont);
        bool flag = false;
        string prevOperand = operand;
        sol.solveMnemonics(instName, operand, locctr, prcont, sz - pos, flag);
        Lines.push_back({prcont, label, instName, operand, prevOperand});
        prcont += flag;
        if (flag and instName == "SET")
        {
            if (label.empty())
            {
                errors[locctr] = "label(or variable) name missing";
            }
            else
            {
                // Storing SET instruction information
                setVars[label] = operand;
            }
        }
    }
    // Checking for some errors...
    for (auto label : symTab)
    {
        if (label.second.first == -1)
        {
            for (auto lineNum : locLabels[label.first])
            {
                errors[lineNum] = "no such label";
            }
        }
        else if (!locLabels.count(label.first))
        {
            warnings[label.second.second] = "label declared but not used";
        }
    }
}

// Inserting into List vector
void pushInList(int prcont, string macCode, string label, string mnemonic, string operand)
{
    if (!label.empty())
        label += ": ";
    if (!mnemonic.empty())
        mnemonic += " ";
    string statement = label + mnemonic + operand;
    List.push_back({conv.dec_hex(prcont), macCode, statement});
}

// Generating machine codes and building list vector
void pass2()
{
    for (auto curLine : Lines)
    {
        string label = curLine.label, mnemonic = curLine.mnemonic, operand = curLine.operand;
        string prevOperand = curLine.prevOperand;
        int prcont = curLine.prcont, type = -1;
        if (!mnemonic.empty())
        {
            type = opcode[mnemonic].second;
        }
        string Mcode = "        ";
        if (type == 2)
        {
            // offset is required
            int offset = (symTab.count(operand) ? symTab[operand].first - (prcont + 1) : (int)stoi(operand));
            Mcode = conv.dec_hex(offset).substr(2) + opcode[mnemonic].first;
        }
        else if (type == 1 and mnemonic != "data" and mnemonic != "SET")
        {
            // value is required
            int value = (symTab.count(operand) ? symTab[operand].first : (int)stoi(operand));
            Mcode = conv.dec_hex(value).substr(2) + opcode[mnemonic].first;
            if (setVars.count(operand))
            {
                // if in case the operand is a variable used in SET operation
                Mcode = conv.dec_hex(stoi(setVars[operand])).substr(2) + opcode[mnemonic].first;
            }
        }
        else if (type == 1 and (mnemonic == "data" || mnemonic == "SET"))
        {
            // specical case for data  and SET mnemonic
            Mcode = conv.dec_hex(stoi(operand));
        }
        else if (type == 0)
        {
            // nothing is required
            Mcode = "000000" + opcode[mnemonic].first;
        }
        else
        {
            // do nothing...
        }
        MachineCodes.push_back(Mcode);
        pushInList(prcont, Mcode, label, mnemonic, prevOperand);
    }
}

// Writing errors / warnings into .log file
void errors_warnings()
{
    ofstream printerrors("logfile.log");
    cout << "log file generated" << endl;
    if (errors.size() == 0)
    {
        printerrors << "NO ERRORS" << endl;
        for (auto it : warnings)
        {
            printerrors << "Line Number " << it.first << " WARNING: " << it.second << endl;
        }
        printerrors.close();
        return;
    }

    for (auto it : errors)
    {
        printerrors << "Line Number " << it.first << " ERROR: " << it.second << endl;
    }
    printerrors.close();
}

vector<string> readLines; // stores each line

// Reading from the input file
void read_file()
{
    string fileName;
    ifstream cinfile;
    cout << "Enter ASM file name to assemble: ";
	cin >> fileName;
    cinfile.open(fileName);
    if (cinfile.fail())
    {
        cout << "Input file doesn't exit" << endl;
        exit(0);
    }
    string curLine;
    while (getline(cinfile, curLine))
    {
        readLines.push_back(curLine);
    }
    cinfile.close();
}

// Writing into listing file(.lst) and machine codes object file(.o)
void write_file()
{
    ofstream coutList("listfile.lst");
    for (auto cur : List)
    {
        coutList << cur.address << " " << cur.macCode << " " << cur.statement << endl;
    }
    coutList.close();
    cout << "listing file generated" << endl;
    ofstream coutMCode;
    coutMCode.open("machineCode.o", ios::binary | ios::out);
    for (auto code : MachineCodes)
    {
        if (code.empty() or code == "        ")
            continue;
        unsigned int cur = (unsigned int)stoi(conv.hex_dec(code));
        static_cast<int>(cur);
        coutMCode.write((const char *)&cur, sizeof(unsigned int));
    }
    coutMCode.close();
    cout << "object file generated" << endl;
}

int main()
{
    read_file();                                                    // read from file
    storeopcodes();                                                 // intialise Opcode table
    pass1(readLines);                                               // first pass
    errors_warnings();                                              // if found errors, write and terminate
    if (errors.empty())
    {                                                               // if no errors
        pass2();                                                    // go for second pass
        write_file();                                               // write machine code and listing file
    }
    return 0;
}