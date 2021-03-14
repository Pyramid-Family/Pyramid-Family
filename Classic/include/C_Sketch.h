#ifndef _CSKETCH_H
#define _CSKETCH_H

#include "BOBHash.h"
#include "params.h"
#include <string.h>
#include <algorithm>

class C_Sketch
{
public:
	int w, d;
	BOBHash* hash[MAX_HASH_NUM * 2];
	int index[MAX_HASH_NUM];
	int* counter[MAX_HASH_NUM];
	int MAX_CNT, MIN_CNT;

public:
	C_Sketch(int _w, int _d, int hash_seed = 1000)
	{
		d = _d, w = _w;

		for (int i = 0; i < d; i++)
		{
			counter[i] = new int[w];
			memset(counter[i], 0, sizeof(int) * w);
		}

		MAX_CNT = (1 << (COUNTER_SIZE - 1)) - 1;
		MIN_CNT = (-(1 << (COUNTER_SIZE - 1)));

		for (int i = 0; i < d * 2; i++)
		{
			hash[i] = new BOBHash(i + hash_seed);
		}
	}

	void Insert(const char* str)
	{
		int g = 0;
		for (int i = 0; i < d; i++)
		{
			index[i] = (hash[i]->run(str, KEY_LEN)) % w;
			g = (hash[i + d]->run(str, KEY_LEN)) % 2;

			if (g == 0)
			{
				if (counter[i][index[i]] != MAX_CNT)
				{
					counter[i][index[i]]++;
				}
			}
			else
			{
				if (counter[i][index[i]] != MIN_CNT)
				{
					counter[i][index[i]]--;
				}
			}
		}
	}

	void Delete(const char* str) {
		int g = 0;
		for (int i = 0; i < d; i++)
		{
			index[i] = (hash[i]->run(str, KEY_LEN)) % w;
			g = (hash[i + d]->run(str, KEY_LEN)) % 2;

			if (g == 1)
			{
				if (counter[i][index[i]] != MAX_CNT)
				{
					counter[i][index[i]]++;
				}
			}
			else
			{
				if (counter[i][index[i]] != MIN_CNT)
				{
					counter[i][index[i]]--;
				}
			}
		}
	}

	int Query(const char* str)
	{
		int temp;
		int res[MAX_HASH_NUM];
		int g;
		for (int i = 0; i < d; i++)
		{
			index[i] = (hash[i]->run(str, KEY_LEN)) % w;
			temp = counter[i][index[i]];
			g = (hash[i + d]->run(str, KEY_LEN)) % 2;

			res[i] = (g == 0 ? temp : -temp);
		}

		sort(res, res + d);
		if (d % 2 == 0)
			return ((res[d / 2] + res[d / 2 - 1]) / 2);
		else
			return (res[d / 2]);
	}

	~C_Sketch()
	{
		for (int i = 0; i < d; i++)
			delete[]counter[i];
		for (int i = 0; i < d * 2; i++)
			delete hash[i];
	}
};


#endif
