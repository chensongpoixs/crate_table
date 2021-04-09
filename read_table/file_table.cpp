#include "file_table.h"




#include <map>
#include <cassert>


static bool filter_number(char * buf)
{
	if (!buf)
	{
		return false;
	}
	int cur_index = 0;
	int pre_index = 0;
	while (buf[cur_index] != '\0')
	{
		if (buf[cur_index] > '9' || buf[cur_index] < '0' || buf[cur_index] == '.')
		{
			
		}
		else
		{
			buf[pre_index++] = buf[cur_index];
		}
		++cur_index;
	}
	buf[pre_index] = '\0';
	return true;
}


file_table::file_table()
	:m_table_left_right_map()
	, m_table_level_rate_map()
{

}


file_table::~file_table()
{
	m_table_left_right_map.clear();
	m_table_level_rate_map.clear();
}


bool file_table::init(const char * file_name)
{


	/*std::string file_name = "1.tab"*/;
	FILE * pFile = ::fopen(file_name, "r");
	if (!pFile)
	{
		printf("[error] [not oenn file name = %s failed ]\n", file_name);
		return false;
	}

	::fseek(pFile, 0, SEEK_END);
	long iFileLength = ::ftell(pFile);
	::fseek(pFile, 0, SEEK_SET);
	if (iFileLength <= 0)
	{
		::fclose(pFile);
		pFile = NULL;
		printf("read file %s failed", file_name);
		return false;
	}
	char * m_buf = new char[iFileLength + 1];
	if (!m_buf)
	{
		::fclose(pFile);
		pFile = NULL;
		printf("alloc long = %u failed", iFileLength);
		return false;
	}
	::fread(m_buf, sizeof(char), iFileLength, pFile);
	char * p = m_buf;
	int index = 0;
	char *appfix = new  char[1024*1024*70];
	
	
	std::ofstream m_fd;
	std::string log_name = "exex.log";
	m_fd.open(log_name, std::ios::out | std::ios::trunc);
	if (!m_fd.is_open())
	{
		::fclose(pFile);
		pFile = NULL;
		printf("alloc long = %u failed", iFileLength);
		return false;
		return false;
	}
	uint32 row = 0; //行
	
	while (p[index] != '\0')
	{
		::memset(appfix, 0, 1024 * 1024 * 70);
		int size = 0;
		uint32 column = 0;//列
		while (p[index] != CHAR_END)
		{
			
			if (p[index] != CHAR_NEXT)
			{
				appfix[size++] = p[index];
			}
			else
			{
				
				if (row != 0)
				{
					//没有奖励名次就过滤了  100.000过滤
					if (size > 0 && ((row  == 1)|| (row != 1 && std::atoi(appfix) != 100)))
					{
						if (!m_table_level_rate_map[row].insert(std::make_pair(column, appfix)).second)
						{
							assert(-1);
						}
						else
						{

						}
					}
					
				}
				else
				{
					char * first_left = new char[512];
					char * second_right = new char[512];
					::memset(first_left, 0, 512);
					::memset(second_right, 0, 512);
					bool two = false;
					uint32 first_left_index = 0;
					uint32 second_right_index = 0;
					int cur_index = 0;
					while (appfix[cur_index] != '\0')
					{
						if (appfix[cur_index] == CHAR_SPACE)
						{
							++cur_index;
							two = true;
							continue;
						}

						if (two)
						{
							second_right[second_right_index++] = appfix[cur_index];
						}
						else
						{
							first_left[first_left_index++] = appfix[cur_index];
						}
						++cur_index;
					}
					if (!filter_number(&first_left[0]))
					{
						assert(-1);
					}
					if (!filter_number(&second_right[0]))
					{
						assert(-1);
					}
					ctable_data table;
					table.index = column;
					table.left = std::atoi(first_left);
					table.right = std::atoi(second_right);
					if (!m_table_left_right_map.insert(std::make_pair(column, table)).second)
					{
						assert(-1);
					}
					if (first_left)
					{
						delete[] first_left;
						first_left = nullptr;
					}
					if (second_right)
					{
						delete[] second_right;
						second_right = nullptr;
					}
					m_fd << "[level = " << table.index << "][left = " << table.left << "][ right = " << table.right << "]";
					printf("level = %u, left = %u, right = %u\n", table.index, table.left, table.right);
					m_fd.flush();
					//char *first_left = &appfix[0];
				}
				size = 0;
				::memset(appfix, 0, 1024 * 1024 * 70);
			
				++column;
			}
			++index;
		}
		++row;
		m_fd << "[iFileLength =  "<< iFileLength <<"]end --->[ index =   "  << p[index] << "][appfix =  " << appfix <<" ]\n";
		printf("[iFileLength = %u]end ---> index = %d[%d][appfix = %s]\n", iFileLength,  index, p[index], appfix);
		++index;
		m_fd.flush();
	}
	if (appfix)
	{
		delete[] appfix;
		appfix = NULL;
	}
	m_fd.flush();
	if (m_fd.is_open())
	{
		m_fd.close();
	}
	if (pFile)
	{
		::fclose(pFile);
		pFile = NULL;
	}
	




	return true;
}


bool file_table::to_file_json(const char * file_name)
{
	m_table_left_right_map;
	m_table_level_rate_map;
	std::ofstream m_fd;
	std::string log_name(file_name);
	m_fd.open(log_name, std::ios::out | std::ios::trunc);
	if (!m_fd.is_open())
	{
		printf("not crate file %s failed \n", file_name);
		return false;
	}
	/*{
		"id":1,
			"number_left" : 1,
			"number_right" : 10,
			"match_bonus_list" : [
		{
			"ranking_left":1,
				"ranking_right" : 1,
				"bonus_rate" : 100
		}
			]
	},

	{
		"id":2,
		"number_left" : 11,
		"number_right" : 20,
		"match_bonus_list" : [
			{
				"ranking_left":1,
					"ranking_right" : 1,
					"bonus_rate" : 60
			},
			{
				"ranking_left":2,
				"ranking_right" : 2,
				"bonus_rate" : 40
			}
		]

	},*/
	for (uint32 i = 1; i < m_table_left_right_map.size() + 50; ++i)
	{
		MTABLE_DATA_LEVLE_MAP::iterator data_iter = m_table_left_right_map.find(i);
		if (data_iter == m_table_left_right_map.end())
		{
			break;
		}
		assert(data_iter->first == data_iter->second.index);
		m_fd << "{ \n \"id\":" << data_iter->first << ", \n";
		m_fd << "\"number_left\":" << data_iter->second.left << ", \n";
		m_fd << "\"number_right\":" << data_iter->second.right << ", \n";
		m_fd << "\"match_bonus_list\": [\n";
		for (uint32 j = 1; j < m_table_level_rate_map.size() + 50; ++j)
		{
			
			MTABLE_LEVEL_RATE_MAP::iterator rate_iter = m_table_level_rate_map.find(j);
			if (rate_iter == m_table_level_rate_map.end())
			{
				break;
			}
			
			MNUMBER_RATE_MAP::const_iterator number_iter =  rate_iter->second.find(i);
			if (number_iter == rate_iter->second.end())
			{
				break;
			}
			if (j != 1)
			{
				m_fd << ",\n";
			}
			m_fd << "{\n \"ranking_left\":" << j << ",\n";
			m_fd << " \"ranking_right\":" << j << ", \n";
			m_fd << "\"bonus_rate\":" << number_iter->second << " \n}\n";
			m_fd.flush();
		}
		m_fd << "]\n},\n";
		m_fd.flush();
	}


	m_fd.flush();
	if (m_fd.is_open())
	{
		m_fd.close();
	}
	return true;
}

void file_table::destroy()
{
	m_table_left_right_map.clear();
	m_table_level_rate_map.clear();
}