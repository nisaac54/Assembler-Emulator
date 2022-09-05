//
//		Implementation of the symbol table class.  This is the format I want for commenting functions.
//
#include "stdafx.h"
#include "SymTab.h"
#include "Errors.h"

/**/
/*

NAME

        void SymbolTable::AddSymbol - Adds a symbol to the symbol tabel

SYNOPSIS

        void SymbolTable::AddSymbol( const string &a_symbol, int a_loc );

    	    a_symbol	-> The name of the symbol to be added to the symbol table.
    	    a_loc		-> the location to be associated with the symbol.

DESCRIPTION

        This function will place the symbol "a_symbol" and its location "a_loc"
        in the symbol table.

RETURN
        returns nothing

*/
/**/

 
void SymbolTable::AddSymbol( const string &a_symbol, int a_loc )
{
    // If the symbol is already in the symbol table, record it as multiply defined.
    map<string, int>::iterator st = m_symbolTable.find( a_symbol );
    if( st != m_symbolTable.end() ) {

        st->second = multiplyDefinedSymbol;
        return;
    }
    // Record the location in the symbol table.
    m_symbolTable[a_symbol] = a_loc;
}


void SymbolTable::DisplaySymbolTable()
{
        cout << "Symbol Table: " << endl;
        cout << "------------------------------------" << endl;
        cout << "\tSymbol # \tSymbol \tLocation\n";

        int label_loc = 0;
        for (m_st_itr = m_symbolTable.begin(); m_st_itr != m_symbolTable.end(); ++m_st_itr)
        {
            cout << "\t" << label_loc << "\t\t " << m_st_itr->first << "\t " << m_st_itr->second << "\n";
            label_loc++;
        }
}


// If the symbol is found in the symbol table return true else return false
bool SymbolTable::LookupSymbol(const string& a_symbol, const int& a_loc)
{
    for (m_st_itr = m_symbolTable.begin(); m_st_itr != m_symbolTable.end(); ++m_st_itr)
    {
        if (m_st_itr->first == a_symbol && m_st_itr->second == a_loc)
        {
            return true;
        }
    }
    Errors::RecordError(string("Symbol Not Found!"));
    return false;
}