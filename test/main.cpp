#include "test\header_rtdb.h"
using namespace test_db_1;
using namespace std;
int main(int argc, char *argv[])
{
	//header_tab_1[2]->index = 2;
	if (!rtdb_has_load())
	{
		printf("RTDB is not load!");
		exit(-1);
	}
	db_1_tbl1[1].ind = 13;
	test_db_1::TYPE_tbl1 tbl = db_1_tbl1;
	string ss = tbl[1].name;
	strcpy_s(tbl[1].name , "Ä£Äâ");
	return 0;
}