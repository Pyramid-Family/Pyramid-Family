#ifndef MP_OO_FPI_H
#define MP_OO_FPI_H

/*
 * On-Off sketch applying Mini-Pyramid on finding persistent items
 */

#include "bitset.h"
#include "Abstract.h"
#include "PyramidCounter.h"

template<typename DATA_TYPE, typename COUNT_TYPE, uint32_t SLOT_NUM>
class MP_OO_FPI : public Abstract<DATA_TYPE, COUNT_TYPE> {
public:

	struct Bucket {
		DATA_TYPE items[SLOT_NUM];
		COUNT_TYPE counters[SLOT_NUM];

		inline COUNT_TYPE Query(const DATA_TYPE item) {
			for (uint32_t i = 0; i < SLOT_NUM; ++i) {
				if (items[i] == item)
					return counters[i];
			}
			return 0;
		}

		inline uint32_t find_min_i()
		{
			COUNT_TYPE min_count = counters[0]; uint32_t min_i = 0;
			for (uint32_t i = 1; i < SLOT_NUM; ++i) {
				if (counters[i] < min_count)
					min_count = counters[i], min_i = i;
			}
			return min_i;
		}
	};

	MP_OO_FPI(uint64_t memory) :
		length((double)memory / (sizeof(Bucket) + sizeof(PyramidCounter) + (SLOT_NUM + 3) * BITSIZE)),
		hash_max(length * 3) {
		buckets = new Bucket[length];
		sketch = new PyramidCounter[length];

		memset(buckets, 0, length * sizeof(Bucket));
		memset(sketch, 0, length * sizeof(PyramidCounter));

		bucketBitsets = new BitSet(SLOT_NUM * length);
		sketchBitsets = new BitSet(hash_max);
	}

	~MP_OO_FPI() {
		delete[] buckets;
		delete[] sketch;
		delete bucketBitsets;
		delete sketchBitsets;
	}

	void Insert(const DATA_TYPE item, const COUNT_TYPE window) {
		uint32_t pos = this->hash(item) % hash_max;
		uint32_t bucketBitPos = pos / 3 * SLOT_NUM;

		for (uint32_t i = 0; i < SLOT_NUM; ++i) {
			if (buckets[pos / 3].items[i] == item) {
				buckets[pos / 3].counters[i] += (!bucketBitsets->SetNGet(bucketBitPos + i));
				return;
			}
		}

		if (!sketchBitsets->Get(pos)) {
			uint32_t i = buckets[pos / 3].find_min_i();
			if (buckets[pos / 3].counters[i] == sketch[pos / 3].Query(pos % 3)) {
				buckets[pos / 3].items[i] = item;
				buckets[pos / 3].counters[i] += 1;
				bucketBitsets->Set(bucketBitPos + i);
				return;
			}

			sketch[pos / 3].Insert(pos % 3);
			sketchBitsets->Set(pos);
		}
	}

	COUNT_TYPE Query(const DATA_TYPE item) {
		return buckets[this->hash(item) % hash_max / 3].Query(item);
	}

	void NewWindow(const COUNT_TYPE window) {
		bucketBitsets->Clear();
		sketchBitsets->Clear();
	}

private:
	const uint32_t length, hash_max;

	BitSet* bucketBitsets;
	Bucket* buckets;

	BitSet* sketchBitsets;
	PyramidCounter* sketch;
};

#endif //OO_FPI_H
