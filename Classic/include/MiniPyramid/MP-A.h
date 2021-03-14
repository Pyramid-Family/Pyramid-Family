#ifndef _MP_A_H
#define _MP_A_H

#include "../params.h"
#include "PyramidCounter.h"
#include "../BOBHash.h"
#include <string.h>

class MP_A
{
public:
	int w, d;
	int* new_count;
	int* old_count;
	char** items;
	PyramidCounter** counters;
	BOBHash* hash[MAX_HASH_NUM];

	char empty_item[KEY_LEN] = { 0 };
	bool is_empty_item(const char* str)
	{
		return memcmp(str, empty_item, KEY_LEN) == 0;
	}

public:
	MP_A(int _w, int _d, int hash_seed = 1000)
	{
		w = _w, d = _d;

		items = new char* [FILTER_SIZE];
		for (int i = 0; i < FILTER_SIZE; i++)
		{
			items[i] = new char[KEY_LEN];
			memset(items[i], 0, KEY_LEN);
		}

		new_count = new int[FILTER_SIZE];
		old_count = new int[FILTER_SIZE];
		memset(new_count, 0, sizeof(int) * FILTER_SIZE);
		memset(old_count, 0, sizeof(int) * FILTER_SIZE);

		counters = new PyramidCounter * [d];
		*counters = new PyramidCounter[w * d];

		for (int i = 1; i < d; ++i)
			counters[i] = *counters + w * i;

		for (int i = 0; i < d; i++)
		{
			hash[i] = new BOBHash(i + hash_seed);
		}
	}

	int find_element_in_filter(const char* str)
	{
		for (int i = 0; i < FILTER_SIZE; i++)
		{
			if (memcmp(str, items[i], KEY_LEN) == 0)
				return i;
		}
		return -1;
	}

	//can finish in finding element in filter
	int find_empty_in_filter()
	{
		for (int i = 0; i < FILTER_SIZE; i++)
		{
			if (is_empty_item(items[i]))
				return i;
		}
		return -1;
	}

	void Insert(const char* str)
	{
		int index = find_element_in_filter(str);
		int index_empty = find_empty_in_filter();
		int estimate_value = INT32_MAX;
		int min_index, min_value, hash_value, temp;
		if (index != -1)
		{
			new_count[index] += 1;
			return;
		}
		else if (index_empty != -1)
		{
			strncpy(items[index_empty], str, KEY_LEN);
			new_count[index_empty] = 1;
			old_count[index_empty] = 0;
		}
		else
		{
			for (int i = 0; i < d; i++)
			{
				uint32_t idx = hash[i]->run(str, KEY_LEN) % (w * lo_cnt);
				counters[i][idx / lo_cnt].Insert(idx % lo_cnt);
			}
			for (int i = 0; i < d; ++i)
			{
				uint32_t idx = hash[i]->run(str, KEY_LEN) % (w * lo_cnt);
				int val = counters[i][idx / lo_cnt].Query(idx % lo_cnt);
				estimate_value = min(estimate_value, val);
			}

			min_index = 0;
			min_value = (1 << 30);
			for (int i = 0; i < FILTER_SIZE; i++)
			{
				if (!is_empty_item(items[i]) && min_value > new_count[i])
				{
					min_value = new_count[i];
					min_index = i;
				}
			}
			if (estimate_value > min_value)
			{
				temp = new_count[min_index] - old_count[min_index];
				if (temp > 0)
				{
					for (int j = 0; j < temp; j++) // +temp
					{
						for (int i = 0; i < d; i++)
						{
							uint32_t idx = hash[i]->run(items[min_index], KEY_LEN) % (w * lo_cnt);
							counters[i][idx / lo_cnt].Insert(idx % lo_cnt);
						}
					}
				}
				strncpy(items[min_index], str, KEY_LEN);
				new_count[min_index] = estimate_value;
				old_count[min_index] = estimate_value;
			}
		}
	}


	int Query(const char* str)
	{
		int index = find_element_in_filter(str);
		if (index != -1)
		{
			return new_count[index];
		}

		int ret = INT32_MAX;
		for (int i = 0; i < d; ++i)
		{
			uint32_t idx = hash[i]->run(str, KEY_LEN) % (w * lo_cnt);
			int val = counters[i][idx / lo_cnt].Query(idx % lo_cnt);
			ret = min(ret, val);
		}

		return ret;
	}

	~MP_A()
	{
		for (int i = 0; i < FILTER_SIZE; i++)
		{
			delete[]items[i];
		}
		delete old_count;
		delete new_count;
		delete[] * counters;
		delete[]counters;
		for (int i = 0; i < d; i++)
		{
			delete hash[i];
		}
	}



};

#endif
