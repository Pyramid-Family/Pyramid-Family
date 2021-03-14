# include "BOBHash.h"
# include "params.h"
# include <iostream>
# include <string.h>

using namespace std;

class CU_Sketch
{
public:
	int w, d;
	int* counter[MAX_HASH_NUM];
	int COUNTER_SIZE_MAX_CNT = (1 << (COUNTER_SIZE - 1)) - 1;
	BOBHash* hash[MAX_HASH_NUM];
	int index[MAX_HASH_NUM];    //index of each d

public:
	CU_Sketch(int _w, int _d, int hash_seed = 1000)
	{
		w = _w, d = _d;

		for (int i = 0; i < d; i++)    //allc memory
		{
			counter[i] = new int[w];
			memset(counter[i], 0, sizeof(int) * w);
		}

		for (int i = 0; i < d; i++)    //init d hash functions
		{
			hash[i] = new BOBHash(i + hash_seed);
		}
	}

	void Insert(const char* str)
	{
		int temp = 0, min_value = COUNTER_SIZE_MAX_CNT;;
		for (int i = 0; i < d; i++)
		{
			index[i] = (hash[i]->run(str, KEY_LEN)) % w;
			temp = counter[i][index[i]];
			min_value = temp < min_value ? temp : min_value;
		}

		if (min_value == COUNTER_SIZE_MAX_CNT)
			return;

		for (int i = 0; i < d; i++)
		{
			if (counter[i][index[i]] == min_value)
				counter[i][index[i]] ++;
		}
	}

	int Query(const char* str)
	{
		int temp = 0, min_value = COUNTER_SIZE_MAX_CNT;;
		for (int i = 0; i < d; i++)
		{
			index[i] = (hash[i]->run(str, KEY_LEN)) % w;
			temp = counter[i][index[i]];
			min_value = temp < min_value ? temp : min_value;
		}
		return min_value;
	}

	void Delete(const char* str)
	{
		for (int i = 0; i < d; i++)
		{
			index[i] = (hash[i]->run(str, KEY_LEN)) % w;
			counter[i][index[i]] --;
		}
	}

	~CU_Sketch()
	{
		for (int i = 0; i < d; i++)
		{
			delete[]counter[i];
			delete hash[i];
		}
	}
};