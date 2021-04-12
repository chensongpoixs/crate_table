#include "file_table.h"




#include <map>
#include <cassert>


static bool filter_number(char * buf, uint32 &size)
{
	if (!buf)
	{
		return false;
	}
	int cur_index = 0;
	int pre_index = 0;
	while (buf[cur_index] != '\0')
	{
		if (buf[cur_index] == '.'|| (buf[cur_index] <= '9' && buf[cur_index] >= '0'))
		{
			buf[pre_index++] = buf[cur_index];
		}
		
		++cur_index;
	}
	buf[pre_index] = '\0';
	size = pre_index;
	return true;
}


file_table::file_table()
	:m_table_left_right_map()
	, m_table_level_rate_map()
	, m_table_rank_nums_map()
{

}


file_table::~file_table()
{
	m_table_left_right_map.clear();
	m_table_level_rate_map.clear();
	m_table_rank_nums_map.clear();
}


bool file_table::init(const char * file_name, bool rank_nums)
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
	if (pFile)
	{
		::fclose(pFile);
		pFile = NULL;
	}
	if (rank_nums)
	{
		bool ret = _read_rank_match(m_buf);
		if (m_buf)
		{
			delete[] m_buf;
			m_buf = nullptr;
		}
		return ret;
	}
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
	}
	uint32 row = 0; //行
	
	while (p[index] != '\0')
	{
		::memset(appfix, 0, 1024 * 1024 * 70);
		uint32 size = 0;
		uint32 column = 0;//列
		while (p[index] != CHAR_END)
		{
			
			if (p[index] != CHAR_NEXT)
			{
				appfix[size++] = p[index];
			}
			else //if (p[++index] == CHAR_NEXT)
			{
				{

					if (row != 0)
					{
						filter_number(&appfix[0], size);
						//没有奖励名次就过滤了  100.000过滤
						if (size > 0 && ((row == 1) || (row != 1 && std::atoi(appfix) != 100)))
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
						if (!filter_number(&first_left[0], first_left_index))
						{
							assert(-1);
						}
						if (!filter_number(&second_right[0], second_right_index))
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
						//m_fd.flush();
						//char *first_left = &appfix[0];
					}
					size = 0;
					::memset(appfix, 0, 1024 * 1024 * 70);

					++column;
				}
			}
			
			++index;
		}
		++row;
		m_fd << "[iFileLength =  "<< iFileLength <<"]end --->[ index =   "  << p[index] << "][appfix =  " << appfix <<" ]\n";
		printf("[iFileLength = %u]end ---> index = %d[%d][appfix = %s]\n", iFileLength,  index, p[index], appfix);
		++index;
		//m_fd.flush();
	}
	if (m_buf)
	{
		delete[] m_buf;
		m_buf = nullptr;
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
	uint32 rate_id = 2;
	for (uint32 i = m_table_left_right_map.size()-1; i > 0; --i)
	{
		MTABLE_DATA_LEVLE_MAP::iterator data_iter = m_table_left_right_map.find(i);
		if (data_iter == m_table_left_right_map.end())
		{
			break;
		}
		assert(data_iter->first == data_iter->second.index);
		m_fd << "{ \n \"id\":" << rate_id++ << ", \n";
		m_fd << "\"number_left\":" << data_iter->second.left << ", \n";
		m_fd << "\"number_right\":" << data_iter->second.right << ", \n";
		m_fd << "\"match_bonus_list\": [\n";
		for (uint32 j = 1 ; j  < m_table_level_rate_map.size() ; ++j)
		{
			
			MTABLE_LEVEL_RATE_MAP::iterator rate_iter = m_table_level_rate_map.find(j);
			if (rate_iter == m_table_level_rate_map.end())
			{
				break;
			}
			MTABLE_RANK_MAP::const_iterator rank_iter = m_table_rank_nums_map.find(j);
			if (rank_iter == m_table_rank_nums_map.end())
			{
				assert(-1);
				break;
			}
			/*MTABLE_RANK_MAP::const_iterator table_rank_iter = 	rank_iter->second.find(j);
			if (table_rank_iter == rank_iter->second.end())
			{
				break;
			}*/
			//assert(rank_iter->second.m_index == j);
			MNUMBER_RATE_MAP::const_iterator number_iter =  rate_iter->second.find(i);
			if (number_iter == rate_iter->second.end())
			{
				break;
			}
			if (j != 1)
			{
				m_fd << ",\n";
			}
			m_fd << "{\n \"ranking_left\":" << rank_iter->second.m_left << ",\n";
			m_fd << " \"ranking_right\":" << rank_iter->second.m_right << ", \n";
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
	m_table_rank_nums_map.clear();
}

bool  file_table::_read_rank_match(const char * buf)
{
	if (!buf)
	{
		return false;
	}

	

	std::ofstream m_fd;
	std::string log_name = "exex.log";
	m_fd.open(log_name, std::ios::out | std::ios::trunc);
	if (!m_fd.is_open())
	{
		printf("not open file %s failed !!!\n", log_name.c_str());
		return false;
	}
	const char * p = buf;
	int index = 0;
	char *appfix = new  char[1024 * 1024 * 70];
	char * first_left = new char[512];
	char * second_right = new char[512];
	uint32 first_left_index = 0;
	uint32 second_right_index = 0;
	uint32 row = 0; //行

	while (p[index] != '\0')
	{
		::memset(appfix, 0, 1024 * 1024 * 70);
		uint32 size = 0;
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
					if (column != 0)
					{
						filter_number(&appfix[0], size);
						//没有奖励名次就过滤了  100.000过滤
						if (size > 0 && ((row == 1) || (row != 1 && std::atoi(appfix) != 100)))
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

						

						::memset(first_left, 0, 512);
						::memset(second_right, 0, 512);
						bool two = false;
						first_left_index = 0;
						second_right_index = 0;

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
						if (!filter_number(&first_left[0], first_left_index))
						{
							assert(-1);
						}
						if (!filter_number(&second_right[0], second_right_index))
						{
							assert(-1);
						}
						ctable_rank_nums rank_nums;
						rank_nums.m_index = row;
						rank_nums.m_left = std::atoi(first_left);
						rank_nums.m_right = std::atoi(second_right);
						if (rank_nums.m_right == 0)
						{
							rank_nums.m_right = rank_nums.m_left;
						}

						if (!m_table_rank_nums_map.insert(std::make_pair(row, rank_nums)).second)
						{
							assert(-1);
						}
						if (!m_table_level_rate_map[row].insert(std::make_pair(column, std::to_string(column))).second)
						{
							assert(-1);
						}
						//printf("row = %u, left = %u, right = %u\n", row, table.left, table.right)
						/*ctable_data table;
						table.index = column;
						table.left = std::atoi(first_left);
						table.right = std::atoi(second_right);
						if (!m_table_left_right_map.insert(std::make_pair(column, table)).second)
						{
							assert(-1);
						}

						m_fd << "[level = " << table.index << "][left = " << table.left << "][ right = " << table.right << "]";
						printf("level = %u, left = %u, right = %u\n", table.index, table.left, table.right);*/
						//filter_number(&appfix[0], size);
						////没有奖励名次就过滤了  100.000过滤
						//if (size > 0 && ((row == 1) || (row != 1 && std::atoi(appfix) != 100)))
						//{
						//	if (!m_table_level_rate_map[row].insert(std::make_pair(column, appfix)).second)
						//	{
						//		assert(-1);
						//	}
						//	else
						//	{

						//	}
						//}
					}

				}
				else
				{
					
					::memset(first_left, 0, 512);
					::memset(second_right, 0, 512);
					bool two = false;
					first_left_index = 0;
					second_right_index = 0;

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
					if (!filter_number(&first_left[0], first_left_index))
					{
						assert(-1);
					}
					if (!filter_number(&second_right[0], second_right_index))
					{
						assert(-1);
					}
					ctable_data table;
					table.index = column;
					table.left = std::atoi(first_left);
					table.right = std::atoi(second_right);
					if (table.right == 0)
					{
						table.right = table.left;
					}
					if (!m_table_left_right_map.insert(std::make_pair(column, table)).second)
					{
						assert(-1);
					}
					if (table.left == 3)
					{
						printf("left 3- 10\n");
					}
					m_fd << "[level = " << table.index << "][left = " << table.left << "][ right = " << table.right << "]\n";
					printf("level = %u, left = %u, right = %u\n", table.index, table.left, table.right);
					//m_fd.flush();
					//char *first_left = &appfix[0];
				}
				size = 0;
				::memset(appfix, 0, 1024 * 1024 * 70);

				++column;
			}
			++index;
		}
		++row;
		m_fd << "end --->[ index =   " << p[index] << "][appfix =  " << appfix << " ]\n";
		printf("end ---> index = %d[%d][appfix = %s]\n", index, p[index], appfix);
		++index;
		//m_fd.flush();
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
}