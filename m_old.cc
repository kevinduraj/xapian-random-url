/*--------------------------------------------------------------------
c++ main.cc -lfcgi -L/usr/local/lib -lxapian  \
            -I/usr/local/include -o randurl.fcgi;
--------------------------------------------------------------------*/
#define VER "1/23/2008 2:43:34 PM"
#define XAPIAN_INDEX "/data/tldindex"
#define DEBUG 0
#define MAX 50 
/*------------------------------------------------------------------*/
#include <xapian.h>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>
/*------------------------------------------------------------------*/
using namespace std;
using namespace Xapian;
/*-------------------------------------------------------------------*
 *                          Function Definition                      *
 *-------------------------------------------------------------------*/
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
  Database db(XAPIAN_INDEX);
  Document doc;

  //Xapian::Database db;
  //db.add_database(Xapian::Database("/index"));
  //db.add_database(Xapian::Database("/data/index"));
  
  int TOTAL_DOCS = db.get_doccount();
	
  cout << "Content-type: text/html\r\n\r\n";
  cout << "<html>\n";
  cout << "Total Documents: " <<  TOTAL_DOCS - MAX << "<BR>";
  
  try
  {
      srand((unsigned)time(0));
      for(int i=1; i < MAX+1; i++)
      {
         int random_integer; 
         random_integer = (rand() % TOTAL_DOCS - MAX) + 1;
         doc = db.get_document(random_integer);
         //string url = doc.get_value(1);

         string url = db.get_document(random_integer).get_data(); 
         url = Last_Item(url);
         // str_out =  "<a href=\"http://" + str.substr(loc+4) + "\">" + str.substr(loc+4) + "</a>"; 
       	 //cout << " " << i << " <a href=\"http://" << url <<  "\">" << url << "</a>\n";
       	 cout << " " << i << " " << url << "\n";
         if((i%4)==0) cout << "<br>\n";
         
      }
   }
   catch (const Xapian::Error & error)
   {
       cout << "Exception: " << error.get_msg() << endl;
   }

  cout << "</html>\n";
  return 0;
}
/*------------------------------------------------------------------*/


