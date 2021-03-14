#ifndef _SBF_H
#define _SBF_H

#include <cstring>
#include "../common/BOBHash32.h"

class SBF
{
private:
	int w, d;
	uint32_t* counters;
	BOBHash32* hash;

public:
	SBF() {}
	SBF(int in_w, int in_d, int random_seed) { init(in_w, in_d, random_seed); }
	~SBF() { clear(); }

	void init(int in_w, int in_d, int random_seed)
	{
		w = in_w, d = in_d;
		counters = new uint32_t[w];
		memset(counters, 0, sizeof(uint32_t) * w);
		hash = new BOBHash32[d];
		for (int i = 0; i < d; ++i)
			hash[i].initialize(random_seed + i);
	}

	void clear()
	{
		delete[]counters;
		delete[]hash;
	}

	bool insert(const string& key, uint32_t f = 1)		// return whether is Recurring Minimum
	{
		uint32_t min_value = UINT32_MAX, min_count = 1;
		for (int i = 0; i < d; ++i)
		{
			uint32_t idx = hash[i].run(key.c_str(), key.size()) % w;
			counters[idx] += f;
			if (counters[idx] < min_value)
				min_value = counters[idx], min_count = 1;
			else if (counters[idx] == min_value)
				++min_count;
		}
		return min_count > 1;
	}

	bool query(const string& key, uint32_t& ret_value)	// return whether is Recurring Minimum
	{
		ret_value = UINT32_MAX;
		uint32_t min_count = 1;
		for (int i = 0; i < d; ++i)
		{
			uint32_t idx = hash[i].run(key.c_str(), key.size()) % w;
			if (counters[idx] < ret_value)
				ret_value = counters[idx], min_count = 1;
			else if (counters[idx] == ret_value)
				++min_count;
		}
		return min_count > 1;
	}
};

class SBF_RM
{
private:
	SBF primary, secondary;
	uint32_t value_p, value_s;

public:
	SBF_RM() {}
	SBF_RM(int w_p, int w_s, int d) { init(w_p, w_s, d); }

	void init(int w_p, int w_s, int d)
	{
		primary.init(w_p, d, 700);
		secondary.init(w_s, d, 800);
	}

	void insert(const string& key)
	{
		if (primary.insert(key)) return;
		secondary.query(key, value_s);
		if (value_s > 0)
			secondary.insert(key);
		else
		{
			primary.query(key, value_p);
			secondary.insert(key, value_p);
		}
	}

	uint32_t query(const string& key)
	{
		if (primary.query(key, value_p))
			return value_p;
		secondary.query(key, value_s);
		if (value_s > 0)
			return value_s;
		else
			return value_p;
	}
};

#endif
