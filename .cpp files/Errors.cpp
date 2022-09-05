#include "stdafx.h"

#include "Errors.h"

vector<string> Errors::m_ErrorMsgs;

// Records an error message if present 
void Errors::RecordError(string a_emsg)
{
	m_ErrorMsgs.push_back(a_emsg);
}

// Displays all error messages
void Errors::DisplayErrors()
{
	for (int i = 0; i < m_ErrorMsgs.size(); i++)
	{
		cout << setw(20) << "Error Code: " << setw(30) << m_ErrorMsgs[i] << endl;
	}
}

// Clear out error message
void Errors::InitErrorReporting()
{
	m_ErrorMsgs.clear();
}

// Pause the calling program
void Errors::PauseProgram()
{
	cout << "------------------------------------" << endl;
	cout << "Tap anything to continue: " << endl << endl;
	system("pause>nul");
}