#ifndef _ASKETCH_H
#define _ASKETCH_H

#include "params.h"
#include "BOBHash.h"
#include <string.h>
#include <algorithm>
using namespace std;

class A_Sketch
{
private:
	int w, d;
	int* new_count;
	int* old_count;
	char** items;
	int* counter[MAX_HASH_NUM];
	int MAX_CNT;
	int Myindex[MAX_HASH_NUM];
	BOBHash* bobhash[MAX_HASH_NUM];

	char empty_item[KEY_LEN] = { 0 };
	bool is_empty_item(const char* str)
	{
		return memcmp(str, empty_item, KEY_LEN) == 0;
	}

public:

	A_Sketch(int _w, int _d, int hash_seed = 1000)
	{

		w = _w;
		d = _d;
		for (int i = 0; i < d; i++)
		{
			counter[i] = new int[w];
			memset(counter[i], 0, sizeof(int) * w);
		}

		MAX_CNT = (1 << COUNTER_SIZE) - 1;

		for (int i = 0; i < d; i++)
		{
			bobhash[i] = new BOBHash(i + hash_seed);
		}

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
		int estimate_value, min_index, min_value, hash_value, temp;
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
			estimate_value = (1 << 30);
			for (int i = 0; i < d; i++)
			{
				hash_value = (bobhash[i]->run(str, KEY_LEN)) % w;
				if (counter[i][hash_value] != MAX_CNT)
				{
					counter[i][hash_value] ++;
					estimate_value = estimate_value < counter[i][hash_value] ? estimate_value : counter[i][hash_value];
				}
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
					for (int i = 0; i < d; i++)
					{
						hash_value = (bobhash[i]->run(items[min_index], KEY_LEN)) % w;
						if (counter[i][hash_value] != MAX_CNT)
						{
							counter[i][hash_value] += temp;
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

		int hash_value, temp;
		int estimate_value = (1 << 30);
		for (int i = 0; i < d; i++)
		{
			Myindex[i] = (bobhash[i]->run(str, KEY_LEN)) % w;
			temp = counter[i][Myindex[i]];
			estimate_value = (estimate_value < temp ? estimate_value : temp);
		}
		return estimate_value;
	}
	~A_Sketch()
	{
		for (int i = 0; i < d; i++)
		{
			delete[]counter[i];
		}


		for (int i = 0; i < d; i++)
		{
			delete bobhash[i];
		}
		for (int i = 0; i < FILTER_SIZE; i++)
		{
			delete[]items[i];
		}
		delete old_count;
		delete new_count;
	}
};

#endif//_ASKETCH_H
