#ifndef EXPRESSIONS_H_
#define EXPRESSIONS_H_
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
/*----------------------------------------------------------------------------
 *  Find and replace string 
 *----------------------------------------------------------------------------*/
static void find_replace(string & str,	string const & pattern,	string const & replace) 
{
	string::size_type start = str.find( pattern, 0 );
	while ( start != str.npos ) 
	{
		str.replace( start, pattern.size(), replace );
		start = str.find( pattern, start+replace.size() );
	}
}
/*----------------------------------------------------------------------------*/
static string tolower(string str) 
{
   for (unsigned i=0; i < strlen(str.c_str()); i++)
     if (str[i] >= 0x41 && str[i] <= 0x5A)
       str[i] = str[i] + 0x20;
   return str;
}
/*----------------------------------------------------------------------------*/
#endif /*EXPRESSIONS_H_*/
