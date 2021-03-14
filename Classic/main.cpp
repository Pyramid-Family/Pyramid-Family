#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <numeric>

using namespace std;

#include "include/CM_Sketch.h"
#include "include/CU_Sketch.h"
#include "include/C_Sketch.h"
#include "include/A_Sketch.h"

#include "include/PCM_Sketch.h"
#include "include/PCU_Sketch.h"
#include "include/PC_Sketch.h"
#include "include/PA_Sketch.h"

#include "include/MiniPyramid/MP-CM.h"
#include "include/MiniPyramid/MP-CU.h"
#include "include/MiniPyramid/MP-C.h"
#include "include/MiniPyramid/MP-A.h"

vector<string> items;
unordered_map<string, int> freq;

double item_num = 0, flow_num = 0;
vector<double> experiment_ARE, experiment_AAE, experiment_through_insert, experiment_through_query;

void readFile(const char* filename, int MAX_ITEM = INT32_MAX)
{
	ifstream inFile(filename, ios::binary);
	ios::sync_with_stdio(false);

	int max_freq = 0;
	char key[KEY_LEN], timestamp[8];
	for (int i = 0; i < MAX_ITEM; ++i)
	{
		inFile.read(key, KEY_LEN);
		if (inFile.gcount() < KEY_LEN) break;
		//inFile.read(timestamp, 8);
		items.push_back(string(key, KEY_LEN));
		freq[string(key, KEY_LEN)]++;
	}
	inFile.close();

	item_num = items.size();
	flow_num = freq.size();
	for (auto pr : freq)
		max_freq = max(max_freq, pr.second);
	cout << freq.size() << "flows, " << items.size() << " items read" << endl;
	cout << "max freq = " << max_freq << endl;
}

void calcAcc(const vector<int>* ret_f)
{
	double _ARE = 0, _AAE = 0;

	for (int i = 0; i < testcycles; ++i)
	{
		int z = 0;
		for (auto pr : freq)
		{
			int est_val = ret_f[i][z++];
			int real_val = pr.second;
			int dist = abs(est_val - real_val);
			_ARE += (double)dist / real_val, _AAE += dist;
		}
	}
	_ARE /= freq.size(), _AAE /= freq.size();
	_ARE /= testcycles, _AAE /= testcycles;
	experiment_ARE.push_back(_ARE);
	experiment_AAE.push_back(_AAE);

	cout << "ARE = " << _ARE << ", AAE = " << _AAE << endl;/*<< ", insert = " << throughput_i << ", query = " << throughput_o << endl;*/
}

void test_CM(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;  //   bits/counter_size/hash_counts

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		CM_Sketch cm(w, d, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			cm.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += cm.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(cm.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}

void test_CU(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;  //   bits/counter_size/hash_counts

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		CU_Sketch cu(w, d, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			cu.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += cu.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(cu.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}

void test_C(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;  //   bits/counter_size/hash_counts

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		C_Sketch c(w, d, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			c.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += c.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(c.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}

void test_A(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;  //   bits/counter_size/hash_counts

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		A_Sketch a(w, d, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			a.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += a.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(a.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}

void test_PCM(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w_p = mem_in_byte * 8 / (WORD_SIZE * 2);

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		PCM_Sketch pcm(w_p, d, WORD_SIZE, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			pcm.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += pcm.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(pcm.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}

void test_PCU(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w_p = mem_in_byte * 8 / (WORD_SIZE * 2);

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;

	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		PCU_Sketch pcu(w_p, d, WORD_SIZE, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			pcu.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += pcu.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(pcu.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}

void test_PC(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w_p = mem_in_byte * 8 / (WORD_SIZE * 2);

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		PC_Sketch pc(w_p, d, WORD_SIZE, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			pc.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += pc.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(pc.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}

void test_PA(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w_p = mem_in_byte * 8 / (WORD_SIZE * 2);

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		PA_Sketch pa(w_p, d, WORD_SIZE, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			pa.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += pa.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(pa.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}

void test_MP_CM(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;  //   bits/counter_size/hash_counts

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		MP_CM mp_cm(w, d, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			mp_cm.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += mp_cm.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(mp_cm.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}

void test_MP_CU(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;  //   bits/counter_size/hash_counts

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		MP_CU mp_cu(w, d, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			mp_cu.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += mp_cu.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(mp_cu.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}


void test_MP_C(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;  //   bits/counter_size/hash_counts

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		MP_C mp_c(w, d, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			mp_c.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += mp_c.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(mp_c.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}

void test_MP_A(int mem_in_byte)
{
	int d = 4;  //counts of hash function
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;  //   bits/counter_size/hash_counts

	vector<int> ret_f[testcycles];
	double throughput_i = 0, throughput_o = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		MP_A mp_a(w, d, i * 100);

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items)
			mp_a.Insert(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);

		clock_gettime(CLOCK_MONOTONIC, &time3);
		for (auto pr : freq)
			estimate_sum += mp_a.Query(pr.first.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time4);

		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_i += (double)1000.0 * item_num / resns;
		resns = (long long)(time4.tv_sec - time3.tv_sec) * 1000000000LL + (time4.tv_nsec - time3.tv_nsec);
		throughput_o += (double)1000.0 * flow_num / resns;

		for (auto pr : freq)
			ret_f[i].push_back(mp_a.Query(pr.first.c_str()));
	}

	experiment_through_insert.push_back(throughput_i / testcycles);
	experiment_through_query.push_back(throughput_o / testcycles);
	calcAcc(ret_f);
}

void test_zipf_all(char* filename)
{
	int mem_in_byte = 5 << 20;
	cout << endl << "mem = " << (double)mem_in_byte / (1 << 20) << "MB : " << endl;  //show xxMB memory

	vector <string> name = { "name of 1st dataset", "name of 2nd dataset", "..." };
	for (int o = 0; o < name.size(); o++)
	{
		string name1 = filename + name.at(o);
		readFile(name1.c_str());

		cerr << "CM " << name.at(o) << endl;
		test_CM(mem_in_byte);
		test_PCM(mem_in_byte);
		test_MP_CM(mem_in_byte);

		cerr << "CU " << name.at(o) << endl;
		test_CU(mem_in_byte);
		test_PCU(mem_in_byte);
		test_MP_CU(mem_in_byte);

		cerr << "C " << name.at(o) << endl;
		test_C(mem_in_byte);
		test_PC(mem_in_byte);
		test_MP_C(mem_in_byte);

		cerr << "A " << name.at(o) << endl;
		test_A(mem_in_byte);
		test_PA(mem_in_byte);
		test_MP_A(mem_in_byte);

		items.clear();
		freq.clear();
	}

	exit(0);
}


int main()
{
	char* filename = "dataset name";
	//test_zipf_all("path to the synthetic datasets folder");
	readFile("path to the dataset", 1000000);

	int mem_in_byte_start = 1 << 20;
	int mem_in_byte_end = 6 << 20;
	int mem_step = 1 << 20;
	for (int mem_in_byte = mem_in_byte_start; mem_in_byte <= mem_in_byte_end; mem_in_byte += mem_step)
	{
		double mem_in_MB = (double)mem_in_byte / (1 << 20);
		cout << endl << "mem = " << mem_in_MB << "MB : " << endl;  //show xxMB memory

		cerr << "CM " << mem_in_MB << endl;
		test_CM(mem_in_byte);
		test_PCM(mem_in_byte);
		test_MP_CM(mem_in_byte);

		cerr << "CU " << mem_in_MB << endl;
		test_CU(mem_in_byte);
		test_PCU(mem_in_byte);
		test_MP_CU(mem_in_byte);

		cerr << "C " << mem_in_MB << endl;
		test_C(mem_in_byte);
		test_PC(mem_in_byte);
		test_MP_C(mem_in_byte);

		cerr << "A " << mem_in_MB << endl;
		test_A(mem_in_byte);
		test_PA(mem_in_byte);
		test_MP_A(mem_in_byte);
	}

	ofstream oFile;
	oFile.open("sheet.csv", ios::app);
	if (!oFile) return 0;
	oFile << filename << endl;
	oFile << "AAE," << "ARE," << "insert," << "query," << "sum," << "max" << endl;
	vector<string> name = { "CM", "CU", "C", "A" };
	for (int o = 0, j = 0, mem_in_byte = mem_in_byte_start; o < experiment_AAE.size(); o++)
	{
		if (o % 3 == 0)
		{
			cout << endl;
			cout << name.at(j % 4) << " ";
			if (o % 12 == 0)
			{
				cout << (double)mem_in_byte / (1 << 20) << "MB : " << endl;
				mem_in_byte += 1 << 20;
			}
			else
			{
				cout << endl;
			}

			j++;
		}
		cout << "AAE: " << experiment_AAE.at(o) << "\tARE: " << experiment_ARE.at(o) << "\tinsert: " << experiment_through_insert.at(o)
			<< "\tquery: " << experiment_through_query.at(o) << endl;

		oFile << experiment_AAE.at(o) << "," << experiment_ARE.at(o) << "," << experiment_through_insert.at(o) << ","
			<< experiment_through_query.at(o) << endl;
	}
	oFile.close();

	return 0;
}
