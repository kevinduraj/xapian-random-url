/*--------------------------------------------------------------------
   c++ main.cc -lfcgi -L/usr/local/lib -lxapian  \
               -I/usr/local/include -o randurl.fcgi;
--------------------------------------------------------------------*/
#define VER "1/23/2008 2:43:34 PM"
#define XAPIAN_INDEX "/data/links"
#define MAX 40 
/*------------------------------------------------------------------*/
#include <xapian.h>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>
#include <fstream>
#include "fcgiapp.h"
/*------------------------------------------------------------------*/
using namespace std;
using namespace Xapian;
/*------------------------------------------------------------------*/
/*unsigned get_random(Database &db, Document &doc)
{
   srand((unsigned)time(NULL)); 
   int total = db.get_doccount();
   unsigned i = rand() % total;
   return i;
}*/
/*-------------------------------------------------------------------*
 *                          Function Definition                      *
 *-------------------------------------------------------------------*/
FCGX_Stream *in, *out, *err;

FCGX_Stream & operator<<( FCGX_Stream &o, const char *s ) 
{
  FCGX_FPrintF ( out, s );
  return o;
}
FCGX_Stream & operator<<( FCGX_Stream &o, string s ) 
{
  FCGX_FPrintF ( out, s.c_str() );
  return o;
}
FCGX_Stream & operator<<( FCGX_Stream &o, int *s ) 
{
  FCGX_FPrintF ( out, "%d", s );
  return o;
}
FCGX_Stream & operator<<( FCGX_Stream &o, int s ) 
{
  FCGX_FPrintF ( out, "%d", s );
  return o;
}
/*-------------------------------------------------------------------*
 *                          Function Definition                      *
 *-------------------------------------------------------------------*/
string get_url(string str)
{    
  string::size_type loc = str.find( "url=", 0 );
  string str_out = "<HR>";
    
  if( loc != string::npos )
  {
    str_out =  str.substr(loc+4, str.length()-(loc+5)); 
  }
  return str_out;
}

string Last_Item(string str)
{    
    string::size_type loc = str.find( "url=", 0 );
    string str_out = "<HR>";
    
    if( loc != string::npos )
    {
      str_out =  "<a href=\"http://" 
                 + str.substr(loc+4, str.length()-loc-5) 
                 + "\">" + str.substr(loc+4, str.length()-loc-5) + "</a>"; 
    }
    return str_out;
}
/*-------------------------------------------------------------------*
 *                          - = M A I N = -                          *
 *-------------------------------------------------------------------*/
int main(void)
{
  //--- Global variables ---//
  Database db(XAPIAN_INDEX);
  Document doc;
  int TOTAL_DOCS = db.get_doccount();

  FCGX_ParamArray envp;
  unsigned executed=0;
  srand((unsigned)time(0));

  /************************** UNIQUE CLIENT REQUEST **************************/
  while ( FCGX_Accept ( &in, &out, &err, &envp ) >= 0 )
  {     
     executed++;
	   
     *out << "Content-type: text/html\r\n\r\n";
     *out << "<html>\n";
     *out << TOTAL_DOCS << "|" << executed << "<BR>";
     
     try
     {
        for(int i=1; i < MAX+1; i++)
        {
          try
          { 
            int random_integer; 
            random_integer = (rand() % TOTAL_DOCS - MAX) + 1;
            doc = db.get_document(random_integer);
            string body = db.get_document(random_integer).get_data();
       
            string url = get_url(body); 
            *out << " " << i << " " << random_integer << " <a href=\"" << url <<  "\">" << url << "</a>\n<BR>";
          }
          catch (const Xapian::Error & error)
          {
             *out << "Exception: " << error.get_msg() << "<BR>";
          }
        }
     }
     catch (const Xapian::Error & error)
     {
        *out << "Exception: " << error.get_msg() << "<BR>";
     }
     
     *out << "</html>\n";
  }
  /************************ UNIQUE CLIENT REQUEST ENDS ************************/

  FCGX_Finish( );
  return 0;
}
/*----------------------------------------------------------------------------*/


