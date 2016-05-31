/*
 * c++ load.cc `xapian-config --libs --cxxflags` -o load
*/
#include <xapian.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/shm.h>
#include <sys/ipc.h>
#define XAPIAN_INDEX "/index"
/*----------------------------------------------------------------------------*/
using namespace std;
using namespace Xapian;
/*----------------------------------------------------------------------------*/
void Load2Memory(Database &db)
{
  try 
  {
	int TOTAL_DOCS = db.get_doccount();
	Document doc;
	cout << "Total Documents: " << TOTAL_DOCS  << endl;

	for(int i=1; i < 10; i++)
	{
     		doc = db.get_document(i);
	     	cout << doc.get_data() << "<BR>" << endl;
	}
  } 
  catch (const Xapian::Error & error) 
  {
  	cout << "Exception: " << error.get_msg() << endl;
  }
}
/*----------------------------------------------------------------------------*/
int main(void)
{
	Database db(XAPIAN_INDEX);

	Load2Memory(db); 

	return 0;
}
/*----------------------------------------------------------------------------*/


