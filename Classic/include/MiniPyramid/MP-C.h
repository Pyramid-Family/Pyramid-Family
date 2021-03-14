#ifndef _MP_C_H
#define _MP_C_H

#include "../params.h"
#include "PyramidCounter.h"
#include <algorithm>
#include <string.h>


class MP_C
{
public:
	int w, d;
	int counter_num = 0;
	PyramidCounter** counters;
	BOBHash* hash[MAX_HASH_NUM * 2];
	bool* flag[MAX_HASH_NUM];

public:
	MP_C(int _w, int _d, int hash_seed = 1000)
	{
		d = _d, w = _w;

		counters = new PyramidCounter * [d];
		*counters = new PyramidCounter[w * d];
		counter_num = w * lo_cnt;     //counter_num low 

		for (int i = 1; i < d; ++i)
			counters[i] = *counters + w * i;

		for (int i = 0; i < d; i++)
		{
			flag[i] = new bool[counter_num];
			memset(flag[i], false, sizeof(bool) * (counter_num));
		}
		for (int i = 0; i < d * 2; i++)
		{
			hash[i] = new BOBHash(i + hash_seed);
		}
	}

	~MP_C()
	{
		delete[] * counters;
		delete[]counters;
		for (int i = 0; i < d; i++)
			delete flag[i];
		for (int i = 0; i < d * 2; i++)
			delete hash[i];
	}

	void Insert(const char* str)
	{
		int g;
		for (int i = 0; i < d; i++)
		{
			uint32_t idx = (hash[i]->run(str, KEY_LEN)) % (w * lo_cnt);
			int num = counters[i][idx / lo_cnt].Query(idx % lo_cnt);
			g = (hash[i + d]->run(str, KEY_LEN)) % 2;

			if (g == 0) //+1
			{
				if (flag[i][idx] == false) //+
				{
					counters[i][idx / lo_cnt].Insert(idx % lo_cnt);
				}
				else
				{
					if (num == 1)
					{
						counters[i][idx / lo_cnt].Delete(idx % lo_cnt);
						flag[i][idx] = false;
					}
					else
					{
						counters[i][idx / lo_cnt].Delete(idx % lo_cnt);
					}
				}
			}
			else //-1
			{
				if (flag[i][idx] == false)
				{
					if (num == 0)
					{
						counters[i][idx / lo_cnt].Insert(idx % lo_cnt);
						flag[i][idx] = true;
					}
					else
					{
						counters[i][idx / lo_cnt].Delete(idx % lo_cnt);
					}
				}
				else
				{
					counters[i][idx / lo_cnt].Insert(idx % lo_cnt);
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
			uint32_t idx = (hash[i]->run(str, KEY_LEN)) % (w * lo_cnt);
			temp = counters[i][idx / lo_cnt].Query(idx % lo_cnt);
			g = (hash[i + d]->run(str, KEY_LEN)) % 2;
			if (flag[i][idx] == true)
				temp = -temp;
			res[i] = (g == 0 ? temp : -temp);
		}

		sort(res, res + d);
		if (d % 2 == 0)
			return ((res[d / 2] + res[d / 2 - 1]) / 2);
		else
			return (res[d / 2]);
	}


	void Delete(const char* str)
	{
		int g;
		for (int i = 0; i < d; i++)
		{
			uint32_t idx = (hash[i]->run(str, KEY_LEN)) % (w * lo_cnt);
			int num = counters[i][idx / lo_cnt].Query(idx % lo_cnt);
			g = (hash[i + d]->run(str, KEY_LEN)) % 2;

			if (g == 1) //+1
			{
				if (flag[i][idx] == false)
				{
					counters[i][idx / lo_cnt].Insert(idx % lo_cnt);
				}
				else
				{
					if (num == 1)
					{
						counters[i][idx / lo_cnt].Delete(idx % lo_cnt);
						flag[i][idx] = false;
					}
					else
					{
						counters[i][idx / lo_cnt].Delete(idx % lo_cnt);
					}
				}
			}
			else //-1
			{
				if (flag[i][idx] == false)
				{
					if (num == 0)
					{
						counters[i][idx / lo_cnt].Insert(idx % lo_cnt);
						flag[i][idx] = true;
					}
					else
					{
						counters[i][idx / lo_cnt].Delete(idx % lo_cnt);
					}
				}
				else
				{
					counters[i][idx / lo_cnt].Insert(idx % lo_cnt);
				}
			}
		}
	}

};


#endif
