//
// Class to parse and provide information about instructions.  Note: you will be adding more functionality.
//

/**/
/*
Added Functionality Overview:

    Public Functions:
      1.  string Translation(const int& a_loc, const map<string, int> &stMap, SymbolTable m_symtab);   ->  Convert Assembly Language to Machine Language

      2.  void PassIIHeader();                                                          ->  Print header for PassII

      3.  inline string &GetInstruction() { return m_instruction; }                     ->  Access original instruction

      4.  void Display_Translation(const int& a_loc, const string& machineLang);        -> Display and Convert Translation

      5.  enum class InstructionType:: ST_NULL                                          -> Initilize value

      6. long long ConvertStrTo_Long(string a_machineLang);                             -> Converts a machine language string to long long



    Private Functions:
      1.  bool IsNumber(const string& a_str);                                    -> Light check if string is number

      2.  stringstream ComputeAddress(map<string, int> a_stMap)                  -> Compute the right address                     

      3.  void ErrorChecking(const int& a_loc, map<string, int> a_stMap, SymbolTable m_symtab);       -> Check for Syntax Errors in Assembly Code


    Private Variables:
      1.  map<string, string> OpCodeTable       -> A map regarding OpCode Information

      2.  vector<string> reg                    -> A vector to hold registers 1 - 9

*/
/**/

#pragma once
#include "SymTab.h"

// The elements of an instruction.
class Instruction {

public:

    Instruction( ) {};
    ~Instruction( ) { };

    // Codes to indicate the type of instruction we are processing.  Why is this inside the
    // class?  We should make this an enum class.  We will do this during a lecture.
    enum class InstructionType {
        ST_MachineLanguage, 	// A machine language instruction.
        ST_AssemblerInstr,      // Assembler Language instruction.
        ST_Comment,             // Comment or blank line
        ST_End,                 // end instruction.
        ST_Error,               // Statement has an error.
        ST_NULL                 // Initilize value to an unused number
    };

    // Parse the Instruction.
    InstructionType ParseInstruction(string a_line);

    // Compute the location of the next instruction.
    int LocationNextInstruction(int a_loc);

    // To access the label
    inline string &GetLabel( ) { return m_Label; };

    // To determine if a label is blank.
    inline bool isLabel( ) { return ! m_Label.empty(); };


 
    // To print header for PassII
    void PassIIHeader();

    // Check if Halt is the last statment 
    int CheckHaltError(int a_hcounter);

    // To convert the instruction to machine language
    string Translation(const int& a_loc, const map<string, int> &a_stMap, SymbolTable m_symtab);

    // Display location, machine Language, and Assembly Program Instruction
    void Display_Translation(const int& a_loc, const string& a_machineLang);

    // Convert machine language string to long long
    long long ConvertStrTo_Long(string a_machineLang);

    // To access original instruction
    inline string& GetInstruction() { return m_instruction; }

private:


    // The elemements of a instruction
    string m_Label;        // The label.
    string m_OpCode;       // The symbolic op code.
    string m_Operand1;     // The first operand. 
    string m_Operand2;     // The second operand.


    string m_instruction;    // The original instruction.

// Derived values.
    int m_NumOpCode = 0; // The numerical value of the op code for machine language equivalents.

    // The type of instruction.
    InstructionType m_type = InstructionType::ST_NULL;  

    // == true if the operand 1 is numeric.
    bool m_IsNumericOperand1 = false;

    // The value of the operand 1 if it is numeric.
    int m_Operand1NumericValue = 0; 
    
    // == true if the operand 2 is numeric.
    bool m_IsNumericOperand2 = false;

    // The value of the operand 2 if it is numeric.
    int m_Operand2NumericValue = 0;
// Derived values End


    // Delete any comments from the statement.
    void DeleteComment(string &a_line)
    {
        size_t isemi1 = a_line.find(';');
        if (isemi1 != string::npos)
        {
            a_line.erase(isemi1);
        }
    }
    // Record the fields of the instructions.
    bool RecordFields( const string &a_line );

    // Get the fields that make up the statement.  This function returns false if there
    // are extra fields.
    bool ParseLineIntoFields(string a_line, string& a_label, string& a_OpCode,
                             string& a_Operand1, string& a_Operand2);

    // Check if a string contains a number. 
    bool isStrNumber(const string& a_str);



    // Check if a string is a number
    bool IsNumber(const string& a_str);

    // Get Correct address
    stringstream ComputeAddress(map<string, int> a_stMap);

    // Check Assembly Program for errors
    void ErrorChecking(const int& a_loc, map<string, int> a_stMap, SymbolTable m_symtab);

    vector<string> reg = { "1","2","3","4","5","6","7","8","9" };

    map<string, string> OpCodeTable =
    {
        {"ADD","01"}, {"SUB","02"}, {"MULT","03"}, {"DIV","04"}, {"LOAD","05"}, {"STORE","06"}, {"READ","07"}, {"WRITE","08"},
        {"ADDR","09"},{"SUBR","10"}, {"MULTR","11"}, {"DIVR","12"}, {"B","13"}, {"BM","14"}, {"BZ","15"}, {"BP","16"}, {"HALT","17"},
        {"DC","00"}, {"DS","00"}, {"ORG","00"}, {"END","00"}
    };

};


