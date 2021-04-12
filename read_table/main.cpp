
#include "file_table.h"
#include <cassert>
/*#include <string>
#include <map>
#include <vector>
#include <istream>
#include <ostream>


static const char CHAR_END = '\n';
static const char CHAR_NEXT = '\t';
static const char CHAR_SPACE = '-'*/;


struct ctable_json_name
{
	char * m_input_name;
	char * m_output_name;
	bool  m_rank;
};


struct ctable_json_name table_name_list[] = 
{
	/*
	{
		"Eight_Handed_Payouts.tab",
		"Eight_Handed_Payouts.json",
		false
	},
	{
		"Savage_Payouts1in8Final.tab",
		"Savage_Payouts1in8Final.json",
		false
	},
	{
		"Six_Handed_Payouts.tab",
		"Six_Handed_Payouts.json",
		false
	}
	,
	*/
	/*{
		"15.tab",
		"15.json",
		true
	}
	,*/{
		"20.tab",
		"20.json",
		true
	}
};


int main(int argc, char *argv[])
{
	
	for (uint32 i = 0; i < sizeof(table_name_list) / sizeof(struct ctable_json_name); ++i)
	{
		file_table table;
		if (!table.init(table_name_list[i].m_input_name, table_name_list[i].m_rank))
		{
			return -1;
		}

		if (!table.to_file_json(table_name_list[i].m_output_name))
		{
			assert(-1);
		}
	}
	

	/*std::string file_name = "1.tab";
	FILE * pFile = fopen(file_name.c_str(), "r");
	if (!pFile)
	{
		printf("[error] [not oenn file name = %s failed ]\n", file_name.c_str());
		return -1;
	}

	fseek(pFile, 0, SEEK_END);
	long iFileLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	if (iFileLength <= 0)
	{
		fclose(pFile);
		pFile = NULL;
		printf("read file %s failed", file_name.c_str());
		return false;
	}
	char * m_buf = new char[iFileLength + 1];
	if (!m_buf)
	{
		fclose(pFile);
		pFile = NULL;
		printf("alloc long = %u failed", iFileLength);
		return -1;
	}
	fread(m_buf, sizeof(char), iFileLength, pFile);
	char * p = m_buf;
	int index = 0;
	char appfix[100];
	memset(appfix, 0, 100);
	while (p[index] != CHAR_END)
	{
		if (p[index] == CHAR_NEXT)
		{
			printf("end ---> index = %d[%d][appfix = %s]\n", index, p[index], appfix);

			break;
		}
		else
		{
			appfix[index] = p[index];
		}
		printf("%d\n", p[index]);
		++index;

	}
	printf("end ---> index = %d[%d]\n", index, p[index]);
	if (pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}

*/



	return EXIT_SUCCESS;
}