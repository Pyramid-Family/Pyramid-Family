#ifndef MP_OO_PE_H
#define MP_OO_PE_H

/*
 * On-Off sketch applying Mini-Pyramid on persistence estimation
 */

#include "bitset.h"
#include "Abstract.h"
#include "PyramidCounter.h"

template<typename DATA_TYPE, typename COUNT_TYPE>
class MP_OO_PE : public Abstract<DATA_TYPE, COUNT_TYPE> {
public:

	MP_OO_PE(uint32_t _hash_num, uint32_t _length) :
		hash_num(_hash_num), length(_length), hash_max(length * 3) {
		counters = new PyramidCounter * [hash_num];
		bitsets = new BitSet * [hash_num];
		for (uint32_t i = 0; i < hash_num; ++i) {
			counters[i] = new PyramidCounter[length];
			bitsets[i] = new BitSet(hash_max);
			memset(counters[i], 0, length * sizeof(PyramidCounter));
		}
	}

	~MP_OO_PE() {
		for (uint32_t i = 0; i < hash_num; ++i) {
			delete[] counters[i];
			delete bitsets[i];
		}
		delete[] counters;
		delete[] bitsets;
	}

	void Insert(const DATA_TYPE item, const COUNT_TYPE window) {
		for (uint32_t i = 0; i < hash_num; ++i) {
			uint32_t pos = this->hash(item, i) % hash_max;
			if (!bitsets[i]->SetNGet(pos))
				counters[i][pos / 3].Insert(pos % 3);
		}
	}

	COUNT_TYPE Query(const DATA_TYPE item) {
		COUNT_TYPE ret = INT_MAX;
		for (uint32_t i = 0; i < hash_num; ++i) {
			uint32_t pos = this->hash(item, i) % hash_max;
			ret = MIN(ret, counters[i][pos / 3].Query(pos % 3));
		}
		return ret;
	}

	void NewWindow(const COUNT_TYPE window) {
		for (uint32_t i = 0; i < hash_num; ++i) {
			bitsets[i]->Clear();
		}
	}

private:
	const uint32_t hash_num;
	const uint32_t length, hash_max;

	BitSet** bitsets;
	PyramidCounter** counters;
};

#endif //OO_PE_H
