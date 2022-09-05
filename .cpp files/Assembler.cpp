//
//      Implementation of the Assembler class.
//
#include "stdafx.h"
#include "Assembler.h"
#include "Errors.h"

// Constructor for the assembler.  Note: we are passing argc and argv to the file access constructor.
// See main program.  
Assembler::Assembler(int argc, char* argv[])
    : m_facc(argc, argv)
{
    // Nothing else to do here at this point.
}
// Destructor currently does nothing.  You might need to add something as you develope this project.  If not, we can delete it.
Assembler::~Assembler()
{
}

/**/
/*
NAME

            void Assembler::PassI() -

SYNOPSIS

            void Assembler::PassII()
                m_facc	        -> FileAccess object
                m_symtab	    -> Symbol Table object
                m_inst	        -> Instruction object


DESCRIPTION

            Read every line of input from a text file and collect a label if present.
            Stores each label in the symbol table and keep track of the current location.
            If no label is present on the current line then the function will continue
            to the next line in search of a label.

RETURNS

            retruns void to Assem.cpp ( )

AUTHOR

            Dr.Miller

*/
/**/

void Assembler::PassI()
{
    // Tracks the location of the instructions to be generated.
    int loc = 0;        

    // Successively process each line of source code.
    for (; ; ) {

        // Read the next line from the source file.
        string line;
        if (!m_facc.GetNextLine(line)) {

            // If there are no more lines, we are missing an end statement.
            // We will let this error be reported by Pass II.
            return;
        }
        // Parse the line and get the instruction type.
        Instruction::InstructionType st = m_inst.ParseInstruction(line);

        // If this is an end statement, there is nothing left to do in pass I.
        // Pass II will determine if the end is the last statement and report an error if it isn't.
        if (st == Instruction::InstructionType::ST_End) return;

        // Labels can only be on machine language and assembler language
        // instructions.  So, skip comments.
        if (st == Instruction::InstructionType::ST_Comment)
        {
            continue;
        }
        // Handle the case where there is an error.

        // If the instruction has a label, record it and its location in the
        // symbol table.
          if (m_inst.isLabel()) {

            m_symtab.AddSymbol(m_inst.GetLabel(), loc);
        }
        // Compute the location of the next instruction.
        loc = m_inst.LocationNextInstruction(loc);
    }
}


/**/
/*
NAME

            void Assembler::PassII() - Reports Errors in Assembly Program, Gernerates and Displays Machine Code 

SYNOPSIS

            void Assembler::PassII() 
                m_facc	        -> FileAccess object
                m_symtab	    -> Symbol Table object
                m_inst	        -> Instruction object
                m_enul	        -> Emulator object


DESCRIPTION

            Reads every line of the input file such as ( Label, Opcode, Operand1, Operand2 ).
            m_facc object is used to rewind the file and read from the beginning. Then m_inst is used 
            to Parse each instruction and check if each OpCode is placed correctly. m_inst is also 
            used to Translate the assembly program to machine language, Display the Translation, and 
            Convert the machine language string to a long long. Finally if there is enough memory 
            present it uses the m_emul object to Insert the long long mahcine language into memory

RETURNS

            retruns void to Assem.cpp ( )

*/
/**/
void Assembler::PassII()
{
    Errors::PauseProgram();

    // Tracks the location of the instructions to be generated.
    int loc = 0;                            
    m_facc.rewind();

    // Print header for second table
    m_inst.PassIIHeader();

    Instruction::InstructionType st;
    st = Instruction::InstructionType::ST_NULL;

    // Dont declare in the for loop
    int a_hcount = 0;

    // Successively process each line of source code.
    for (; ; ) {

        string line;

        // If the last line is not equal to END 
        // then we are missing an end statment
        if (!m_facc.GetNextLine(line))
        {
            Instruction::InstructionType ect = st;
            if (ect != Instruction::InstructionType::ST_End) {
                Errors::RecordError(string("Missing End Statment"));
                Errors::DisplayErrors();
                return;
            }
            return;
        }

        // Parse the line and get the instruction type.
        st = m_inst.ParseInstruction(line);


        // If end is not the last statment display an error
        // And display additional info.. if any after the end statment
        if (st == Instruction::InstructionType::ST_End)
        {
            cout << " \t\t\t " << m_inst.GetInstruction() << endl;
            if (m_facc.GetNextLine(line))
            {
                Errors::RecordError(string("More After End Statement"));
            }
        }

        // Print out Comments that sit on there own line
        if (st == Instruction::InstructionType::ST_Comment)
        {
            cout << " \t\t\t " << m_inst.GetInstruction() << endl;
            continue;
        }

        // Check if HALT is the last statment
        a_hcount = m_inst.CheckHaltError(a_hcount);

        if ((st == Instruction::InstructionType::ST_AssemblerInstr) || (st == Instruction::InstructionType::ST_MachineLanguage))
        {
           // Tranlate assembly program to a maching language string
           string str_machineLang = m_inst.Translation(loc, m_symtab.GetSymbolTable(), m_symtab);

           m_inst.Display_Translation(loc, str_machineLang);

           long long long_machineLang = m_inst.ConvertStrTo_Long(str_machineLang);

           // Insert memory is a bool
           if (!m_emul.insertMemory(loc, long_machineLang)) {
               Errors::RecordError(string("Insufficient memory"));
           }
        }
        loc = m_inst.LocationNextInstruction(loc);

        Errors::DisplayErrors();
        Errors::InitErrorReporting();
    }
}

void Assembler::RunProgramInEmulator()
{
    Errors::PauseProgram();
    m_emul.runProgram();
}
