//
//		Implemtation of the Emulator class for the VC8000
//
#include "stdafx.h"
#include "Emulator.h"

/**/
/*
NAME

			bool Emulator::inssertMemory - Insert machine Language into memory

SYNOPSIS

		   bool Emulator::insertMemory( int a_location, long long a_contents )

				a_location	    -> an integer storing the value of the current location.
				a_contents   	-> a long long integer storing the machine language equivlent to the assembly program


DESCRIPTION

			If there is enough memory this function will insert a_contents(machine language)
			into a memory location that is specifed by a_location by maping it with m_memory


RETURNS

			returns true if insertion was sucessfull
			returns false if insertion was unsucessfull

*/
/**/

bool Emulator::insertMemory(int a_location, long long a_contents)
{
	if (a_location >= MEMSZ)
	{
		return false;
	}
	else 
	{
		m_memory[a_location] = a_contents;
		
		return true;
	}
}


/**/
/*
NAME

			bool Emulator::runProgram - Runs the program stored in memory

SYNOPSIS

		   bool Emulator::runProgram( )

DESCRIPTION

			This function will obtain the opcode and address of machine language by 
			using the respected division. Then will switch the case depnding on the opcode
			and store the correct information using the address recoreded in memory 
			to the register base on the opcode instruction


RETURNS

			returns true if Program runs successfully
			returns false if Program has errors

*/
/**/

// Runs the program recorded in memory.
bool Emulator::runProgram() 
{ 
	int address = 0;
	int input = 0;
	int opcode = 0;
	for (int loc = 100; loc < MEMSZ; loc++)
	{
		if (m_memory[loc] != 0)
		{
			address = m_memory[loc] % 1'000'000;
			opcode = m_memory[loc] / 1'000'0000;

			// Get number of OpCode.
			switch (opcode)
			{

			// Add.
			case 1:
				reg = reg + m_memory[address];
				break;

			// Subtract.
			case 2:
				reg = reg - m_memory[address];
				break;

			// Multiply.
			case 3:
				reg = reg * m_memory[address];
				break;

			// Divide.
			case 4:
				reg = reg / m_memory[address];
				break;

			// Load.
			case 5:
				reg = m_memory[address];
				break;

			// Store.
			case 6:
				m_memory[address] = reg;
				break;

			// Read.
			case 7:
				cout << endl << "? ";
				cin >> input;				
				m_memory[address] = input;
				break;

			// Write.
			case 8:
				cout << m_memory[address];
				break;

			// Branch.
			case 13:
				if(reg > 0 || reg < 0) // BRANCH dealts with non numbers ?
					loc = address - 1;
				break;

			// Branch Minus.
			case 14:
				if (reg < 0)
					loc = address - 1;
				break;

			// Branch Zero.
			case 15:
				if (reg == 0)
					loc = address - 1;
				break;

			// Branch Positive.
			case 16:
				if (reg > 0)
					loc = address - 1;
				break;

			// Halt.
			case 17:
				return true;

			// Error
			default:
				string error = "Clear the following errors to run Emulator ";
				cout << error << endl;
				return false;
			}
		}
	}
	return true;
}