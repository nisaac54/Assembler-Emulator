//
//      Implementation of the Instruction class
//
#include "stdafx.h"

#include "Instruction.h"
#include "Errors.h"
#include "SymTab.h"

/**/
/*
NAME

            Instruction::InstructionType Instruction::ParseInstruction - Determing each instructions type

SYNOPSIS

            Instruction::InstructionType Instruction::ParseInstruction( string a_line )

                        a_line      -> a string storing the current instruction
               


DESCRIPTION

           This function will read the current line and first delete any comments on a line.
           Then it will parse each instruction into seperate fields regarding...

           | Label | opcode | operand1 | operand2 | 

           This function will also check if the current line is a format error
           Finally it will determine which instruction is currently being read

RETURNS

            retruns an instruction type to PassI( ) and PassII( )

AUTHOR
        
             Dr.Miller & Narzan Isaac

*/
/**/

// parse an instruction and return its type.  Yes, I want a call by value.
Instruction::InstructionType Instruction::ParseInstruction(string a_line)
{
    // Record the original statement.  This will be needed in the sceond pass.
    m_instruction = a_line;

    // Delete any comment from the line.
    DeleteComment( a_line );

    // Record label, opcode, and operands.  Up to you to deal with formatting errors.
    bool isFormatError = RecordFields( a_line );
    
    // Check if this is a comment.
    if (m_Label.empty() && m_OpCode.empty())
    {
        return InstructionType::ST_Comment;
    }

    // Determine the instruction type
    else if (m_OpCode == "END") {
        m_type = InstructionType::ST_End;
    }
    else if (m_OpCode == "ORG") {
        m_type = InstructionType::ST_AssemblerInstr;
    }
    else if (m_OpCode == "DS") {
        m_type = InstructionType::ST_AssemblerInstr;
    }
    else if (m_OpCode == "DC") {
        m_type = InstructionType::ST_AssemblerInstr;
    }
    else if (m_OpCode != "") {
        m_type = InstructionType::ST_MachineLanguage;
    }
    return m_type;
}

// Record the fields that make up the instructions.
bool Instruction::RecordFields( const string &a_line )
{
    // Get the fields that make up the instruction.
    bool isFormatError =! ParseLineIntoFields( a_line, m_Label, m_OpCode, m_Operand1, m_Operand2);

    // if code was a comment, there is nothing to do.
    if (m_OpCode.empty() && m_Label.empty()) return isFormatError;

    // Record whether the operands are numeric and their value if they are.
    m_IsNumericOperand1 = isStrNumber(m_Operand1);
    if (m_IsNumericOperand1) m_Operand1NumericValue = stoi(m_Operand1);

    m_IsNumericOperand2 = isStrNumber(m_Operand2);
    if (m_IsNumericOperand2) m_Operand1NumericValue = stoi(m_Operand2);

    // For the sake of comparing, convert the op code to upper case.
    for (char& c : m_OpCode)
    {
        c = toupper(c);
    }
    return true;
}

// Parse the intruction into fields.
bool Instruction::ParseLineIntoFields(string a_line, string& a_label, string& a_OpCode,
    string& a_Operand1, string& a_Operand2)
{
    // Get rid of any commas from the line.
    replace(a_line.begin(), a_line.end(), ',', ' ');

    // Get the elements of the line.  That is the label, op code, operand1, and operand2.
    string endStr;
    a_label = a_OpCode = a_Operand1 = a_Operand2 = "";
    istringstream ins(a_line);
    if (a_line[0] == ' ' || a_line[0] == '\t')
    {
        a_label = "";
        ins >> a_OpCode >> a_Operand1 >> a_Operand2 >> endStr;
    }
    else
    {
        ins >> a_label >> a_OpCode >> a_Operand1 >> a_Operand2 >> endStr;
    }
    // If there is extra data, return false.
    return endStr.empty() ? true : false;
}

bool Instruction::isStrNumber(const string& a_str)
{
    if (a_str.empty()) return false;

    // If there is a - or a plus, make sure there are more characters.
    int ichar = 0;
    if (a_str[0] == '-' || a_str[0] == '+')
    {
        ichar++;
        if (a_str.length() < 2) return false;
    }
    // Make sure that the remaining characters are all digits
    for (; ichar < a_str.length(); ichar++)
    {
        if (!isdigit(a_str[ichar])) return false;
    }
    return true;
}


/**/
/*

NAME

            int Instruction::LocateNextInstruction - Finds the location of the next instruction.

SYNOPSIS

            int Instruction::LocateNextInstruction( int a_loc )

                a_loc	    -> an integer storing the value of the current location.


DESCRIPTION

            This function will read the current location and 
            returns the location of the next location.


RETURNS

            returns an integer regarding the next instructions location to PassI( ) and PassII( )


*/
/**/

int Instruction::LocationNextInstruction(int a_loc)
{
    
    if (m_type == InstructionType::ST_AssemblerInstr) 
    {
        int op_loc = 0;
        istringstream ins(m_Operand1);
        ins >> op_loc;

        if (m_OpCode == "ORG") {
            return op_loc;
        }
        else if (m_OpCode == "DC") {
            return a_loc + 1;
        }
        else if (m_OpCode == "DS") {
            return a_loc + op_loc;
        }
        else if (m_OpCode == "END") {
            return a_loc;
        }
    }
    return a_loc + 1;
}

// Print header for PassII
void Instruction::PassIIHeader()
{
    cout << "Translation of Program: " << endl << endl;
    cout << " Location " << " Contents " << "\t Original Statement" << endl;
}


// Checks if HALT is the last statment
int Instruction::CheckHaltError(int a_hcount)
{
    if (m_OpCode == "HALT") {
        a_hcount++;
    }
    else if (a_hcount == 1 && m_Operand2 != "")
    {
        Errors::RecordError(string("Halt is not the last statment"));
        Errors::DisplayErrors();
        Errors::InitErrorReporting();
        a_hcount = 0;
    }
    return a_hcount;
}


/**/
/*

NAME

            string Instruction::Translation - Translate Assembly Code to Machine Language

SYNOPSIS

            string Instruction::Translation( const int &a_loc, const map<string, int> &a_stMap, SymbolTable m_symtab )

                a_loc	    -> an integer storing the value of the current location.
                a_stMap  	-> a Map from the SymbolTable which stores symbols and there locations
                m_symtab    -> a Symbol Table object to touch elements of the Symbol Table class


DESCRIPTION

            This function will first call ErrorChecking to check for any
            errors reading the current line. Then it will call 
            Compute Address to determine the correct address to add.
            Finally it will add the correct strings storing the Opcode,
            register, Operand1, and Operand2, into a string that
            stores the machine language. It will then return 
            that string. 

RETURNS

            returns a string storing machine language to PassII( )

*/
/**/

string Instruction::Translation(const int& a_loc, const map<string, int> &a_stMap, SymbolTable m_symtab)
{
    ErrorChecking(a_loc, a_stMap, m_symtab);

    if (a_loc < 0 || a_loc > 999'999) {
        Errors::RecordError(string("Translation Exited, Called: Locations Out of Bounds"));
        Errors::DisplayErrors();
        exit(1);
    }
    stringstream ss4 = ComputeAddress(a_stMap);
    string address = "";
    ss4 >> address;
    
    string machineLang = "";
    string op1 = "";
    string op2 = "";
    stringstream ss1;
    stringstream ss2;

    if (m_Operand2 != "")
    {
        if (m_Operand1 == "0")
        {
            m_Operand1 = reg.at(8);
        }
        machineLang += OpCodeTable[m_OpCode];
        machineLang += m_Operand1;
        machineLang += address;
    }
    else // Operand2 is not empty
    {
        if (m_OpCode == "HALT")
        {
            machineLang += OpCodeTable[m_OpCode];
            machineLang += reg.at(8);
            machineLang += "000000";
        }
        else if (m_OpCode == "DC")
        {
            ss1 << m_Operand1;
            ss1 >> op1;

            if (op1.size() < 5)
            {
                for (int i = op1.size(); i < 4; i++)
                {
                    op2 = "0" + op2;
                }
            }
            machineLang += OpCodeTable[m_OpCode];
            machineLang += "000";
            machineLang += op2;
            machineLang += op1;
        }
        else if (m_OpCode == "READ")
        {
            machineLang += OpCodeTable[m_OpCode];
            machineLang += reg.at(8);
            machineLang += address;
        }
    }

    // Final check to see if size is correct
    if (machineLang.size() != 9 && m_OpCode != "ORG" && m_OpCode != "DS") {
        Errors::RecordError(string("Did Not Translate Input Properly"));
    }
    
    ss1.clear();
    ss2.clear();
    ss4.clear();
    return machineLang;
}


/**/
/*

NAME

            stringstream Instruction::ComputeAddress - Compute the address of the current line

SYNOPSIS

            stringstream Instruction::ComputeAddress( map< string, int > a_stMap)

                a_stMap  	-> a Map from the SymbolTable which stores symbols and there locations


DESCRIPTION

            This function will convert operands 1 and 2 from the symbol table map to a string
            and store them into a new string. Whichever string is not zero is the address we need. 
            It will then check the operands size and add the corresponding zeros using a while loop.
            if the operands size is greater then or equal to 7 then the program is over the memory limit and 
            we terminate, otherwise if any thing else goes wrong we record an error.
            

RETURNS

            returns a stringstream storing the current address to Translation( )


*/
/**/

stringstream Instruction::ComputeAddress(map<string, int> a_stMap)
{
    string op1_Loc = to_string(a_stMap[m_Operand1]);
    string op2_Loc = to_string(a_stMap[m_Operand2]);
    string address = "";
    stringstream ss4;

    if (op2_Loc == "0" && m_IsNumericOperand1 == false)
    {
        while (op1_Loc.size() < 6)
        {
            op1_Loc = "0" + op1_Loc;
        }
        ss4 << op1_Loc;

        if (op1_Loc.size() >= 7)
        {
            Errors::RecordError(string("ComputeAddress Exited, Called: Location Out of Bounds"));
            Errors::DisplayErrors();
            exit(1);
        }
        return ss4;
    }
    if (m_IsNumericOperand1 == true && m_IsNumericOperand2 == false)
    {
        while (op2_Loc.size() < 6)
        {
            op2_Loc = "0" + op2_Loc;
        }
        ss4 << op2_Loc;

        if (op2_Loc.size() >= 7)
        {
            Errors::RecordError(string("ComputeAddress Exited, Called: Location Out of Bounds"));
            Errors::DisplayErrors();
            exit(1);
        }
        return ss4;
    }
    Errors::RecordError(string("Failed To Compute Address Correctly"));
    return ss4;
}


// Simple Helper function to Check for Errors
// Check if string passed is a number 
bool Instruction::IsNumber(const string& a_str)
{
    for (const char& c : a_str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}

/**/
/*

NAME

            void Instruction::ErrorChecking - Check Assembly Program for Syntax Errors

SYNOPSIS

           void Instruction::ErrorChecking ( const int& a_loc, map<string, int> a_stMap, SymbolTable m_symtab )

                m_symtab    -> a object from the Symbol Table Class
                stMap  		-> a Map from the SymbolTable which stores symbols and there locations
                a_loc       -> an integer storing the current location

DESCRIPTION

            Check each line and determine if each instruction type is a valid instruction.
            Will use the symbol table object to use the LookUpSymbol function and determine 
            if each label read is in the symbol table


RETURNS

           returns void to Translation( )

*/
/**/

void Instruction::ErrorChecking(const int& a_loc, map<string, int> a_stMap, SymbolTable m_symtab) 
{
    // Check Base Case 
    if (IsNumber(m_OpCode)) {
        Errors::RecordError(string("OpCode Must be Symbolic"));
    }

    if (m_OpCode == "ORG") {
        if (!m_Operand2.empty() || m_Operand1.empty()) {
            Errors::RecordError(string("ORG must take one numeric Operand"));
        }
        if (a_loc != 0) {
            Errors::RecordError(string("ORG Must Be The First instruction"));
        }
        if (!IsNumber(m_Operand1)) {
            Errors::RecordError(string("ORG Operand 1 must be numeric"));
        }
        if (m_Operand1 != "100")
        {
            Errors::RecordError(string("ORG must start at 100"));
        }

    }
    else if (m_OpCode == "HALT") {
        if (!m_Operand1.empty() || !m_Operand2.empty()) {
        Errors::RecordError(string("HALT takes no Operands"));
        }
    }
    else if (m_OpCode == "DC" || m_OpCode == "DS") {
        if (m_Label == "") {
            Errors::RecordError(string("Must have Label"));
        }
        if (!IsNumber(m_Operand1)) {
            Errors::RecordError(string("First Operand must be Numeric"));
        }
        if (IsNumber(m_Operand1) && m_Operand2 != "") {
            Errors::RecordError(string("Extra Operand"));
        }
        if (m_Operand1.size() >= 5) {
            Errors::RecordError(string("Memory to Big"));
        }

        // Check if Label is in the symbol table
        m_symtab.LookupSymbol(m_Label, a_loc);
    }
    else if (m_OpCode == "READ") {
        if (m_Operand2.empty()) {
            m_symtab.LookupSymbol(m_Operand1, a_stMap[m_Operand1]);
        }
        else {
            m_symtab.LookupSymbol(m_Operand2, a_stMap[m_Operand2]);
        }
        if (IsNumber(m_Operand1) && IsNumber(m_Operand2)) {
            Errors::RecordError(string("Missing Operand or Second Operand must be Symbolic"));

        }
        if (m_Operand1.empty()) {
            Errors::RecordError(string("Must take at least one Operand"));
        }
    }
    else if (m_OpCode == "ADD" || m_OpCode == "SUB" || m_OpCode == "DIV" || m_OpCode == "MULT" || m_OpCode == "STORE" || m_OpCode == "WRITE" ||
             m_OpCode == "LOAD" || m_OpCode == "B" || m_OpCode == "BM" || m_OpCode == "BZ" || m_OpCode == "BP") {

        if (m_Operand1.empty()) {
            Errors::RecordError(string("Missing first Operand"));
        }
        if (IsNumber(m_Operand2)) {
            Errors::RecordError(string("Missing Second Operand or Operand not Symbolic"));
        }
        if (!IsNumber(m_Operand1))
        {
            Errors::RecordError(string("First Operand must be numeric and between 1 and 9"));
        }
        else if (!m_Operand1.empty() && IsNumber(m_Operand1) && m_OpCode != "WRITE")
        {
            int op1_Number = stoi(m_Operand1);
            if (op1_Number > 9 || op1_Number < 1)
            {
                Errors::RecordError(string("First Operand must be a value between 1 and 9"));
            }
        }

        // Check if Label is in the symbol table
        m_symtab.LookupSymbol(m_Operand2, a_stMap[m_Operand2]);
    }
}


// Convert machineLang string to long long 
long long Instruction::ConvertStrTo_Long(string a_machineLang)
{
    long long machineLang = 0;
    if (isStrNumber(a_machineLang))
    {
        machineLang = stol(a_machineLang);
    }
    return machineLang;
}

/**/
/*
NAME

            void Instruction::Display_Translations - Display the translated program

SYNOPSIS

           void Instruction::Display_Translation ( const int& a_loc, const string& a_machineLang )

                a_loc	        -> an integer storing the value of the current location.
                a_machineLang  	-> a string storing the machine language equivlent to the assembly program


DESCRIPTION

            Then function will display the location, machine language,
            and original instruction type of each line.
            

RETURNS

            returns void to PassII( )

*/
/**/

void Instruction::Display_Translation(const int& a_loc, const string& a_machineLang)
{
    if (m_type == Instruction::InstructionType::ST_AssemblerInstr) {
        if (m_OpCode == "DC") {
            cout << a_loc << " \t " << a_machineLang << " \t" << m_instruction << endl;
        }
        else if (m_OpCode == "DS") {
            cout << a_loc << " \t\t " << a_machineLang << " \t" << m_instruction << endl;
        }
        else if (m_OpCode == "ORG") {
            cout << a_loc << "          " << a_machineLang << "\t\t" << m_instruction << endl;
        }
        else {
            cout << a_loc << " " << m_instruction << endl;
        }
    }
    else {
        cout << a_loc << " \t " << a_machineLang << " \t" << m_instruction << endl;
    }
}