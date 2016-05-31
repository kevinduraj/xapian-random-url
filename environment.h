#ifndef SEARCH_H_
#define SEARCH_H_
/*------------------------------------------------------------------*/
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>
/*------------------------------------------------------------------*/
#define MAX_PARAMS 25
/*------------------------------------------------------------------*/
using namespace std;
/*-------------------------------------------------------------------*
 *                 GLOBALS VARIABLES                                 *
 *-------------------------------------------------------------------*/
typedef struct {
    char *name;
    char *value;
} param;
param params[MAX_PARAMS];
/*------------------------------------------------------------------*/
int num_params = 0;
char client_q[256];
/*-------------------------------------------------------------------
  Convert 2 hexadecimal digits to an ASCII character, without checking. 
  -------------------------------------------------------------------*/
static char x2c(char *what) 
{
  register char digit;

  digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
  digit *= 16;
  digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
  return(digit);
}
/*-------------------------------------------------------------------
  Search for any '%' character and, interpreting the next two 
  characters as a hexadecimal character code, replace the three 
  characters with the encoded character. 
  -------------------------------------------------------------------*/
static void decode_hex( char *str ) 
{
    register int x, y;

    for ( x=0,y=0; str[y]; ++x,++y) 
    {
        if ((str[x] = str[y]) == '%') 
        {
            str[x] = x2c(&str[y+1]);
            y+=2;
        }
    }
    str[x] = '\0';
}
/*--------------------------------------------------------------------*
 * Split q parameter values into single values stored in vector 
 * -------------------------------------------------------------------*/
static void parse_multi_values(vector<string> &terms)
{
   #define DELIMITER " "
   char* token = strtok(params[num_params].value, DELIMITER);
   while(token != NULL)
   {
     terms.push_back(token);
     token = strtok(NULL, DELIMITER);
   }
}
 /*------------------------------------------------------------------
  Store the field in the global 'entries' table, 
  and dump the contents as HTML to output.
 -------------------------------------------------------------------*/
static void store_param(char *field,vector<string> &terms)
{
    int i=0;
   
    /*--- Store client query globaly ---*/
    if(!strncmp (field, "q=", 2)) { strncpy (client_q, field, 255); }
   
    /*--- Replace '+' with ' ' ---*/
    for( i=0; field[i]; ++i) if (field[i] == '+') field[i] = ' ';

    /*--- Find '=' and split there ---*/
    for ( i=0; field[i] != '\0' && field[i] != '='; ++i) ;
    if (field[i] == '\0')  // no '='
    {
       decode_hex( field );
    }
    else /*--- split at '=' ---*/
    {
       int eqpos = i;
       field[eqpos] = '\0';
       decode_hex( field );
       decode_hex( &field[eqpos+1] );
      
       params[num_params].name  = field;
       params[num_params].value = &field[eqpos+1];
       
       /* Process "q" search parameter */
        if (!strncmp (params[num_params].name, "q", 1)) 
            parse_multi_values(terms);

       ++num_params;       
    }
}
 /*--------------------------------------------------------------------
  Parse QUERY_STRING 
 ---------------------------------------------------------------------*/
static void parse_query_string(char *query, vector<string> &terms)
{
    int i;
    if (query == NULL) printf("No query information to decode.\n");
    else
    {
       char *ptr = query;
       while (*ptr != '\0')
       {
          char *field = ptr;
          for (i=ptr-query; query[i] != '\0'; ++i,++ptr) 
          {
             if (query[i] == '&')
             {
                query[i] = '\0';
                ptr = &query[i+1];
                break;
             }
          }
          store_param( field, terms );
        }
    }
}
/*------------------------------------------------------------------*/
#endif /*SEARCH_H_*/
