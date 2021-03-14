#ifndef __PYRAMID_COUNTER_H__
#define __PYRAMID_COUNTER_H__
#include <stdint.h>

/*
* -------------------------------------------
*  |    |sig2|sig1|sig0|        lo2        |
*  |        lo1        |        lo0        |
*  |                  hi                   |
*  |                                       |
* -------------------------------------------
*/

const uint32_t lo_cnt = 3, lo_len = 4;
const uint32_t hi_len = 16;
const uint32_t hi_mask = (1 << hi_len) - 1;
const uint32_t lo_mask = (1 << lo_len) - 1;
const uint32_t hi_shift = lo_cnt * lo_len;
const uint32_t sig_shift = hi_len + lo_cnt * lo_len;
const uint32_t degen_sig = 0x80000000;      //1000 0000 0000 0000 0000000000000000
const uint32_t degen_mask = 0x7fffffff;     //0111 1111 1111 1111 1111111111111111
class PyramidCounter
{
private:
	uint32_t buf;

public:
	PyramidCounter() : buf(0)
	{
	}

	void Insert(int idx, int f = 1)
	{
		if (buf & degen_sig)							// if is normal counter, simply return
		{
			buf += f;
			return;
		}

		uint32_t lo = buf >> idx * lo_len & lo_mask;
		lo += f;
		if (lo > lo_mask)
		{
			uint32_t hi = buf >> hi_shift & hi_mask;
			hi += lo >> lo_len;
			lo &= lo_mask;
			if (hi > hi_mask)							// if hi overflow, degenerate to normal counter
			{
				buf = degen_sig | (hi << lo_len) | lo;
				return;
			}
			buf &= ~(lo_mask << idx * lo_len);			// clear lo
			buf |= lo << (idx * lo_len);				// set lo
			buf &= ~(hi_mask << hi_shift);				// clear hi
			buf |= hi << hi_shift;						// set hi
			buf |= 1 << (sig_shift + idx);				// set sig
		}
		else
		{
			buf += (f << (idx * lo_len));				// increase lo
		}
	}

	uint32_t Query(int idx)
	{
		if (buf & degen_sig)							// if is normal counter, simply return
			return buf & degen_mask;
		if (!(buf & 1 << (sig_shift + idx)))			// if not overflow, return lo
		{
			uint32_t lo = buf >> idx * lo_len & lo_mask;
			return lo;
		}
		uint32_t hi = buf >> hi_shift & hi_mask;
		uint32_t lo = buf >> idx * lo_len & lo_mask;
		return hi << lo_len | lo;						// else return hi and lo
	}

	void Delete(int idx)
	{
		if (buf & degen_sig)							// if is normal counter, simply return
		{
			--buf;
			return;
		}
		uint32_t lo = (buf >> (idx * lo_len)) & lo_mask;
		uint32_t hi = buf >> hi_shift & hi_mask;
		if (hi == 0)
		{
			if (lo == 0)
			{
				return;
			}
			else
			{
				buf -= (1 << (idx * lo_len));
				return;
			}
		}
		else
		{
			if (lo == 0)
			{
				if (hi == 1)
					buf &= ~(1 << (sig_shift + idx));
				buf -= 1 << hi_shift;
				buf |= lo_mask << idx * lo_len;
				return;
			}
			else
				buf -= (1 << (idx * lo_len));
		}
	}
};

#endif
