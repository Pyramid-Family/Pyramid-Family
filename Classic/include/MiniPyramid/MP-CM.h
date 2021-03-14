#ifndef __MP_CM_H__
#define __MP_CM_H__
#include "../BOBHash.h"
#include "PyramidCounter.h"
#include "../params.h"


class MP_CM
{
public:
	int w, d;
	BOBHash* hash[MAX_HASH_NUM];
	PyramidCounter** counters;

public:
	MP_CM() {}
	MP_CM(int _w, int _d, int hash_seed = 1000) { init(_w, _d, hash_seed); }
	~MP_CM() { clear(); }


	void init(int _w, int _d, int hash_seed = 1000) {
		w = _w, d = _d;

		counters = new PyramidCounter * [d];
		*counters = new PyramidCounter[w * d];

		for (int i = 1; i < d; ++i)
			counters[i] = *counters + w * i;

		for (int i = 0; i < d; i++)    //init d hash functions
		{
			hash[i] = new BOBHash(i + hash_seed);
		}
	}

	void Insert(const char* str)
	{
		for (int i = 0; i < d; ++i)
		{
			uint32_t idx = hash[i]->run(str, KEY_LEN) % (w * lo_cnt);
			counters[i][idx / lo_cnt].Insert(idx % lo_cnt);
		}
	}

	void Delete(const char* str)
	{
		for (int i = 0; i < d; ++i)
		{
			uint32_t idx = hash[i]->run(str, KEY_LEN) % (w * lo_cnt);
			counters[i][idx / lo_cnt].Delete(idx % lo_cnt);
		}
	}

	int Query(const char* str)
	{
		int ret = INT32_MAX;
		for (int i = 0; i < d; ++i)
		{
			uint32_t idx = hash[i]->run(str, KEY_LEN) % (w * lo_cnt);
			int val = counters[i][idx / lo_cnt].Query(idx % lo_cnt);
			ret = min(ret, val);
		}
		return ret;
	}




	int* Ccounter_query(const char* str, int d)
	{
		static int value[4];
		for (int i = 0; i < d; ++i)
		{
			uint32_t idx = hash[i]->run(str, KEY_LEN) % (w * lo_cnt);
			int val = counters[i][idx / lo_cnt].Query(idx % lo_cnt);
			value[i] = val;
		}
		return value;
	}

	void Ccounter_Delete(const char* str)
	{
		for (int i = 0; i < d; ++i)
		{
			uint32_t idx = hash[i]->run(str, KEY_LEN) % (w * lo_cnt);
			counters[i][idx / lo_cnt].Delete(idx % lo_cnt);
		}
	}

	void clear()
	{
		delete[] * counters;
		delete[]counters;
		for (int i = 0; i < d; i++)
		{
			delete hash[i];
		}
	}
};




#endif
