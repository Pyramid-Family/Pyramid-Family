#ifndef __MP_CU_H__
#define __MP_CU_H__
#include "../BOBHash.h"
#include "PyramidCounter.h"
#include "MP-CM.h"


class MP_CU : public MP_CM
{
private:
	uint32_t* idx;
	int* ret_val;

public:
	MP_CU() {}
	MP_CU(int _w, int _d, int hash_seed = 1000) { init(_w, _d, hash_seed); }
	~MP_CU() { clear(); }

	void init(int _w, int _d, int hash_seed = 1000)
	{
		MP_CM::init(_w, _d, hash_seed);
		idx = new uint32_t[_d];
		ret_val = new int[_d];
	}

	void clear()
	{
		delete[]idx;
		delete[]ret_val;
	}

	void insert(const char* str)
	{
		int min_val = INT32_MAX;
		for (int i = 0; i < d; ++i)
		{
			idx[i] = hash[i]->run(str, KEY_LEN) % (w * lo_cnt);
			ret_val[i] = counters[i][idx[i] / lo_cnt].Query(idx[i] % lo_cnt);
			min_val = min(min_val, ret_val[i]);
		}
		for (int i = 0; i < d; ++i)
			if (ret_val[i] == min_val)
				counters[i][idx[i] / lo_cnt].Insert(idx[i] % lo_cnt);
	}
};

#endif
