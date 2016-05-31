/*
c++ fast.cc -lfcgi -L/usr/local/lib -lxapian -I/usr/local/include -o fast.fcgi; service httpd restart
*/
#define IMAGE "http://myhealthcare.com/MyHealthcare.gif"
#define VER "7/5/2007 12:03:28 PM"
#define HOST "http://myhealthcare.com"
#define APPLICATION "http://myhealthcare.com/cgi-bin/fast.fcgi"
#define ENGINE_NAME "MyHealthcare.com Search Engine"
#define XAPIAN_INDEX "/health"
#define BGCOLOR "gold"
#define FONTCOLOR "black"
/*---------------------------------------------------------------------*/
#include <xapian.h>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>
/*---------------------------------------------------------------------*/
#include "fcgi_config.h"
#include "fcgiapp.h"
/*---------------------------------------------------------------------*/
using namespace std;
using namespace Xapian;
/*---------------------------------------------------------------------*/
typedef struct {
    char *name;
    char *value;
} param;
/*---------------------------------------------------------------------*
 *                              GLOBAL                                 *
 *---------------------------------------------------------------------*/
#define MAX_PARAMS 25
#define MAX 4096 /* maximum string lenght */

int NEXT = 50;
param params[MAX_PARAMS];
int num_params = 0;
//char *getenv();
char client_q[256];
char client_p[256];
char temp[5000];
/*---------------------------------------------------------------------*
 *                          Function Definition                        *
 *---------------------------------------------------------------------*/
string tolower(string str);
void parse_multi_values();
void parse_query_string(char *query, vector<string> &terms);
string find_substring(const string &str, char *first, char *last);
void store_param(char *field,vector<string> &terms);
void decode_hex(char *str);
char x2c(char *what);

void highlight_text(char *str, vector<string> &highlight);
void parce_string_by_space(char *str, vector<string> &terms);
/*---------------------------------------------------------------------*/
extern char **environ;

FCGX_Stream *in, *out, *err;

FCGX_Stream & operator<<( FCGX_Stream &o, const char * s ) {
	FCGX_FPrintF ( out, s );
	return o;
}
FCGX_Stream & operator<<( FCGX_Stream &o, string s ) {
	FCGX_FPrintF ( out, s.c_str() );
	return o;
}
FCGX_Stream & operator<<( FCGX_Stream &o, int s ) {
	FCGX_FPrintF ( out, "%d", s );
	return o;
}
FCGX_Stream & operator<<( FCGX_Stream &o, double s ) {
	FCGX_FPrintF ( out, "%.3f", s );
	return o;
}
/*---------------------------------------------------------------------*
 *                          - = M A I N = -                            *
 *---------------------------------------------------------------------*/
int main(void)
{
  char *ptr;
  char *body2;
  char *i2;
  char *title2;
  char *url2;
  char *x2;
  
  FCGX_ParamArray envp;
  clock_t start_clock, end_clock;
  Database db(XAPIAN_INDEX);
  Enquire enquire(db);
  int count = 0;
  // char *body2, *i2, *title2, *url2, *x2;
	
  /********************** UNIQUE CLIENT REQUEST ************************/
  while ( FCGX_Accept ( &in, &out, &err, &envp ) >= 0 )
  {
    string body, title, desc, url;
    
    /* Initialize global variale to NULL */
    memset(client_q, '\0', sizeof(client_q));
    memset(client_p, '\0', sizeof(client_p));
      
      num_params=0;  
      vector<string> terms;
      vector<string>::iterator iter;
	    
      start_clock = clock();

      *out << "Content-type: text/html\r\n\r\n"  << "<html>\n"           
           << "<STYLE>\n"
           << " a:link    { color: black; text-decoration: none}\n"
           << " a:hover   { color: red;   text-decoration: underline}\n"
           << " a:visited { color: black; text-decoration: none}\n"
           << " a:active  { color: black; text-decoration: underline}\n"
           << "</STYLE>\n"
           << "<body onload=\"document.forms[0].q.focus();\">\n"
           << "<center><a href=\"" << HOST << "\"><img src=\"" << IMAGE << "\" border=0></a>\n";
           /*<< "<center><a href=\"" << HOST << "\"><h2>" << ENGINE_NAME << "</h2></a>\n";*/


    /*-- Display Environment Variables ---*/
    /* for(int i=0; i<27; i++)  *out << "<b>" << i << "</b> " << envp[i] << "<BR>"; */

    char* p_query = FCGX_GetParam( "QUERY_STRING", envp );
    parse_query_string(p_query, terms);
    
    *out  << "<table border=0 width=780>"
          << "<tr>"
          << "<td align=center valign=top><nobr>"
          << "  <FORM METHOD=\"GET\" ACTION=\"" << APPLICATION << "\">"
          << "	<INPUT type=text name=\"q\" SIZE=\"64\" MAXLENGTH=\"128\" value=\"";
   
   for (iter = terms.begin(); iter != terms.end(); ++iter ) *out << *iter << " ";
          
    *out  << "\">"
          << "  	<INPUT type=\"submit\" value=\" Search \">&nbsp;&nbsp;&nbsp;<br>"
          << "    </FORM>"
          << "    </nobr>"
          << "   </TD>"
          << "</tr>"
          << "</table>\n";
            
    /* Crop first character or QUERY_STRING name ex: "q=" or "p=" */
    if(client_q[0])  strcpy(client_q, &client_q[2]);    
    if(!client_p[0]) strcpy(client_p, "0");
    else  strcpy(client_p, &client_p[2]);

    *out << "<tt>Searching " << (int)db.get_doccount() << " web sites.<br>Term Frequency: ";
    for (iter = terms.begin(); iter != terms.end(); ++iter ) 
    { 
      *out << " " << *iter << ":" << (int)db.get_termfreq(*iter);
    }    
    *out << "</tt>\n";
    
    Query query(Query::OP_AND, terms.begin(), terms.end());
    *out << "<table border=1 width=900>\n";
    //*out << "<tr><td>" << query.get_description() << "</td></tr>\n";

    enquire.set_query(query);
    //enquire.set_cutoff(90,0);
    //enquire.set_sort_by_value_then_relevance(2,1);

    int current   = atoi(client_p); 
    //*out << "<h1>" << current << "</h2>";
       
    
    MSet matches  = enquire.get_mset(current, NEXT);
    int estimated = matches.get_matches_estimated();
    //*out << "&nbsp;Total:&nbsp;" << estimated << " results found.</td</tr>\n";

    int prev = current-NEXT; if(prev < 0) prev = 0;
    int next = current+NEXT;         
    
    /*----------------------------- Display top and buttom bar ---------------------------------*/    
    *out << "<TABLE cols=3 border=0 width=900>"
         << "<TR><TH width=\"200\" BGCOLOR=\"" << BGCOLOR << "\" align=left><a href=\"" << APPLICATION << "?q=";             
    for (iter = terms.begin(); iter != terms.end(); ++iter ) *out << *iter << "+";                  
    *out << "&p=" << prev << "\"><font color=" << FONTCOLOR << " face=Verdana size=2>&lt;&lt; Previous "
         << NEXT << "</font></a></TH>\n"
         << "<TH  BGCOLOR=\"" << BGCOLOR << "\" align=center><font color=" << FONTCOLOR << " face=Verdana size=2>Result: "
         << current << " - " << next << " of about " << estimated << " results found.</font></TH>\n"
         << "<TH width=\"200\" BGCOLOR=\"" << BGCOLOR << "\" align=right><a href=\"" << APPLICATION << "?q=";         
         for (iter = terms.begin(); iter != terms.end(); ++iter ) *out << *iter << "+";                      
    *out << "&p=" << next <<"\"><font color=" << FONTCOLOR << " face=Verdana size=2>Next  "
         << NEXT << "&gt;&gt;</font></a></TH>\n"
         << "</TR></TABLE>\n\n";
     /*-----------------------------------------------------------------------------------------*/

    for (MSetIterator i = matches.begin(); i != matches.end(); ++i)
    {
        *out << "<TABLE cols=3 border=0 width=900>"
             << "<tr><td><font face=Verdana size=2 color=black><b>" << (int)i.get_rank() + 1 << ".</b> ID:" << (int) *i
             << "</font>&nbsp;&nbsp;&nbsp;<font face=Verdana size=2 color=red>Relevance: " << (int)i.get_percent() << " " 
             << "</font>&nbsp;&nbsp;&nbsp;\n";

        Xapian::Document doc = i.get_document();
        *out << "<font face=Verdana size=2 color=green>Rank: " << doc.get_value(1) << "</font>&nbsp;&nbsp;&nbsp;\n"
             << "<font face=Verdana size=2 color=navy>Date: " << doc.get_value(2) << "</font></td></tr>\n";

        string str  = doc.get_data();
        
        char *per;
        char none[] = "          Not Found";
        memset(temp, '\0',     sizeof(temp));
        strcpy(temp, str.c_str());
        ptr = temp;
        
        if(ptr)
        {
          body2  = strstr(ptr,"body=");
          if(body2) /* --- security measure*/
          {
            ptr=body2;
            per = strstr(body2, "printf");
            if(per) *(per-1)= '\0';
          } 
          else { body2  = none; }
            
          i2     = strstr(ptr,"\ni=");     if(i2)    { *(i2-1)     = '\0'; ptr=i2;     } else { i2     = none; }
          title2 = strstr(ptr,"\ntitle="); if(title2){ *(title2-1) = '\0'; ptr=title2; } else { title2 = none; }
          url2   = strstr(ptr,"\nurl=");   if(url2)  { *(url2)     = '\0'; ptr=url2+1; } else { url2   = none; }
          x2     = strstr(ptr,"\nx=");     if(x2)    { *(x2)       = '\0'; ptr=x2+1;   } else { x2     = none; }
        }
                       
        //*out << "<tr><td><font face=Verdana size=3 color=black><b><a href=\"http://" << url2+5
        //     << "\" target=_blank>" << title2+7 << "</a></font></td></tr>\n"
        //     << "<tr><td><font face=Verdana size=2 color=black>" << body2+5 << "</font></td></tr>\n"
        //     << "<tr><td><a href=\"http://" << url2+5 << "\"><font face=Verdana size=2 color=green><b>"
        //     << url2+5 << "</b></font></a><br><br></td></tr>\n"
        //     << "</table>\n\n";        

        *out << "<tr><td><font face=Verdana size=3 color=black><b><a href=\"http://" << url2+5
             << "\" target=_blank>" << title2+7 << "</a></font></td></tr>\n"
             << "<tr><td><font face=Verdana size=2 color=black>"; 
             
        highlight_text(body2+5, terms);
           
             
        *out << "</font></td></tr>\n"
             << "<tr><td><a href=\"http://" << url2+5 << "\"><font face=Verdana size=2 color=green><b>"
             << url2+5 << "</b></font></a><br><br></td></tr>\n"
             << "</table>\n\n";          
        
    }

    /*----------------------------- Display top and buttom bar ---------------------------------*/    
    *out << "<TABLE cols=3 border=0 width=900>"
         << "<TR><TH width=\"200\" BGCOLOR=\"" << BGCOLOR << "\" align=left><a href=\"" << APPLICATION << "?q=";             
    for (iter = terms.begin(); iter != terms.end(); ++iter ) *out << *iter << "+";                  
    *out << "&p=" << prev << "\"><font color=" << FONTCOLOR << " face=Verdana size=2>&lt;&lt; Previous "
         << NEXT << "</font></a></TH>\n"
         << "<TH  BGCOLOR=\"" << BGCOLOR << "\" align=center><font color=" << FONTCOLOR << " face=Verdana size=2>Result: "
         << current << " - " << next << " of about " << estimated << " results found.</font></TH>\n"
         << "<TH width=\"200\" BGCOLOR=\"" << BGCOLOR << "\" align=right><a href=\"" << APPLICATION << "?q=";         
         for (iter = terms.begin(); iter != terms.end(); ++iter ) *out << *iter << "+";                      
    *out << "&p=" << next <<"\"><font color=" << FONTCOLOR << " face=Verdana size=2>Next  "
         << NEXT << "&gt;&gt;</font></a></TH>\n"
         << "</TR></TABLE>\n\n";
     /*-----------------------------------------------------------------------------------------*/
     

    end_clock = clock();
		     
    *out << "<tt>Execution time=(" << (double)(end_clock-start_clock) / (double)CLOCKS_PER_SEC << ")&nbsp;"
         << VER << "&nbsp;&nbsp;Request: " << ++count << "</tt>\n</center></body>\n</html>\n";
	}
	/************************* UNIQUE CLIENT REQUEST ENDS ******************************/

  FCGX_Finish( );

  return 0;
}
 /*---------------------------------------------------------------------
  Parse QUERY_STRING 
 ---------------------------------------------------------------------*/
void parse_query_string(char *query, vector<string> &terms)
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
/*-----------------------------------------------------*/
string find_substring(const string &str, char *first, char *last)
{
   string::size_type loc1 = str.find( first, 0 );
   string::size_type loc2;

   if( loc1 != string::npos )
   {
     string sub = str.substr(loc1);
     loc2 = sub.find( last, 0 );
     return str.substr(loc1, loc2);
   }
   else return "";
}
/*--------------------------------------------------------------------- *
 * Split q parameter values into single values stored in vector 
 * ---------------------------------------------------------------------*/
void parse_multi_values(vector<string> &terms)
{
   #define DELIMITER " "
   char* token = strtok(params[num_params].value, DELIMITER);
   while(token != NULL)
   {
     terms.push_back(tolower(token));
     token = strtok(NULL, DELIMITER);
   }
}
 /*---------------------------------------------------------------------
  Store the field in the global 'entries' table, 
  and dump the contents as HTML to output.
 ---------------------------------------------------------------------*/
void store_param(char *field,vector<string> &terms)
{
   register int i=0;
   
   /*--- store client query globaly ---*/
   if      (!strncmp (field, "q=", 2)) { strncpy (client_q, field, 255); }
   else if (!strncmp (field, "p=", 2)) { strncpy (client_p, field, 255); }
   
    //--- Replace '+' with ' '
    for( i=0; field[i]; ++i) if (field[i] == '+') field[i] = ' ';

    //--- Find '=' and split there
    for ( i=0; field[i] != '\0' && field[i] != '='; ++i) ;
    if (field[i] == '\0')  // no '='
    {
       decode_hex( field );
    }
    else //--- split at '='
    {
       int eqpos = i;
       field[eqpos] = '\0';
       decode_hex( field );
       decode_hex( &field[eqpos+1] );
      
       params[num_params].name = field;
       params[num_params].value = &field[eqpos+1];
       
       /* Process "q" search parameter */
       if (!strncmp (params[num_params].name, "q", 1))
          parse_multi_values(terms);

       ++num_params;       
    }
}

 /*---------------------------------------------------------------------
  Search for any '%' character and, interpreting the next two 
  characters as a hexadecimal character code, replace the three 
  characters with the encoded character. 
 ---------------------------------------------------------------------*/
void decode_hex( char *str ) 
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

 /*---------------------------------------------------------------------
  Convert 2 hexadecimal digits to an ASCII character, without checking. 
 ---------------------------------------------------------------------*/
char x2c(char *what) 
{
  register char digit;

  digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
  digit *= 16;
  digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
  return(digit);
}
/*----------------------------------------------------------------------------*/
void highlight_text(char *str, vector<string> &highlight)
{
    bool skip = false;    
    vector<string> words;
    vector<string>::iterator iter1;    
    vector<string>::iterator iter2;      
    
    parce_string_by_space(str, words);
    
    for (iter1 = words.begin(); iter1 != words.end(); ++iter1 )
    {   
       for (iter2 = highlight.begin(); iter2 != highlight.end(); ++iter2 )
       {
         if( strcmp((tolower((string)*iter1)).c_str(), (tolower((string)*iter2)).c_str() ) == 0)
         {
            skip=true;
            *out << "<font face=Verdana size=2 color=goldenrod><B>" << *iter1 << "</b></font>\n";              
         }
       }
       if(skip) skip=false;
       else *out << *iter1 << "\n";
    }
}
/*----------------------------------------------------------------------------*/
void parce_string_by_space(char *str, vector<string> &words)
{ 
   char c[MAX];
   strncpy(c, str, MAX);
   #define DELIMITER " " 
   char* token = strtok(c, DELIMITER);
   while(token != NULL)
   {
     words.push_back(token);
     token = strtok(NULL, DELIMITER);
   }
}
/*----------------------------------------------------------------------------*/
string tolower(string str) 
{
   for (unsigned i=0; i < strlen(str.c_str()); i++)
     if (str[i] >= 0x41 && str[i] <= 0x5A)
       str[i] = str[i] + 0x20;
   return str;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* SECURITY
body=this program tries to print hello world, but something not quite right comes out. #include <stdio.h> int main() { printf("%s %s ", "hello" "world!"); return (0); } source code hint 1: the first %s should print the first string to follow. the second %s should print the second following string. hint 2: the first %s does more than expected. hint 3: the second %s does the unexpected. hint 4: the format is not followed by two string parameters. answer: the problem is that there is no comma between "hello" and "world": printf("%s %s ", "hello" "world!"); this is the same as: printf("%s %s ", "hello" "world!"); or printf("%s %s ", "helloworld!"); the result is that the first %s prints helloworld! and the second prints garbage. main gallery
i=1
title=brief hello
url=www.nostarch.com/extras/hownotc/hello6_c_a.html
x=2007-03-29

-----------------------------------------------------------------------------*/
