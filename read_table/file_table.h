#pragma once
#define   _CRT_SECURE_NO_WARNINGS
#include <map>
#include <string>
#include <vector>
#include <string>
#include <map>
#include <vector>
#include <istream>
#include <ostream>
#include <iostream>
#include <fstream>
#include <iostream>
#include <fstream>
#include <unordered_map>
static const char CHAR_END = '\n';
static const char CHAR_NEXT = '\t';
static const char CHAR_SPACE = '-';
typedef unsigned int uint32;
struct ctable_data
{
	uint32 index;
	uint32 left;
	uint32 right;
	ctable_data()
		:index(0)
		, left(0)
		, right(0) {}
};
struct ctable_rank_nums
{
	uint32 m_index;
	uint32 m_left;
	uint32 m_right;
	ctable_rank_nums()
		: m_index(0)
		, m_left(0)
		, m_right(0) {}
};
class file_table
{
public:
	file_table();
	~file_table();
public:
	bool init(const char * file_name, bool rank_nums = false);
	bool to_file_json(const char * file_name);
	
	void destroy();
private:
	bool  _read_rank_match(const char * buf);
private:
	typedef		std::unordered_map<uint32, std::string>		MNUMBER_RATE_MAP;
	typedef		std::unordered_map<uint32, ctable_data>		MTABLE_DATA_LEVLE_MAP;
	typedef		std::unordered_map<uint32, ctable_rank_nums>  MTABLE_RANK_MAP;
	typedef		std::unordered_map<uint32, MNUMBER_RATE_MAP>	MTABLE_LEVEL_RATE_MAP;
	typedef		std::unordered_map<uint32, MTABLE_RANK_MAP>	MTABLE_RANK_NUMS_MAP;
private:
	MTABLE_DATA_LEVLE_MAP										m_table_left_right_map;  // level -> [left , right]
	MTABLE_LEVEL_RATE_MAP										m_table_level_rate_map;  // level -----> [Ãû´Î-> rate]
	MTABLE_RANK_MAP												m_table_rank_nums_map;

};

