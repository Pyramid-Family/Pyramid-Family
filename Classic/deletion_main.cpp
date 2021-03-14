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

vector<string> items, items_delete;
unordered_map<string, int> freq;

double item_num = 0, flow_num = 0;
vector<double> experiment_ARE, experiment_AAE, experiment_through_delete;

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

void get_delete_items(int delete_percent)
{
	for (int i = 0; i < items.size(); ++i)
		if (i % 100 < delete_percent)
		{
			items_delete.push_back(items[i]);
			freq[items[i]]--;
			if (freq[items[i]] == 0)
				freq.erase(items[i]);
		}
}

void reset_delete_items()
{
	for (auto item : items_delete)
		freq[item]++;
	items_delete.clear();
}

void calcAcc(const vector<int>* ret_f)
{
	double ARE = 0, AAE = 0;

	for (int i = 0; i < testcycles; ++i)
	{
		int z = 0;
		for (auto pr : freq)
		{
			int est_val = ret_f[i][z++];
			int real_val = pr.second;
			int dist = abs(est_val - real_val);
			ARE += (double)dist / real_val, AAE += dist;
		}
	}
	ARE /= freq.size(), AAE /= freq.size();
	ARE /= testcycles, AAE /= testcycles;
	experiment_ARE.push_back(ARE);
	experiment_AAE.push_back(AAE);

	cout << "ARE = " << ARE << ", AAE = " << AAE << endl;
}

void test_CM(int mem_in_byte)
{
	int d = 4;
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;

	vector<int> ret_f[testcycles];
	double throughput_d = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2;
		long long resns;

		CM_Sketch cm(w, d, i * 100);

		for (auto key : items)
			cm.Insert(key.c_str());

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items_delete)
			cm.Delete(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);
		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_d += (double)1000.0 * items_delete.size() / resns;

		for (auto pr : freq)
			ret_f[i].push_back(cm.Query(pr.first.c_str()));
	}

	experiment_through_delete.push_back(throughput_d / testcycles);
	calcAcc(ret_f);
}

void test_C(int mem_in_byte)
{
	int d = 4;
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;

	vector<int> ret_f[testcycles];
	double throughput_d = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2;
		long long resns;

		C_Sketch c(w, d, i * 100);

		for (auto key : items)
			c.Insert(key.c_str());

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items_delete)
			c.Delete(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);
		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_d += (double)1000.0 * items_delete.size() / resns;

		for (auto pr : freq)
			ret_f[i].push_back(c.Query(pr.first.c_str()));
	}

	experiment_through_delete.push_back(throughput_d / testcycles);
	calcAcc(ret_f);
}

void test_PCM(int mem_in_byte)
{
	int d = 4;
	int w_p = mem_in_byte * 8 / (WORD_SIZE * 2);

	vector<int> ret_f[testcycles];
	double throughput_d = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		PCM_Sketch pcm(w_p, d, WORD_SIZE, i * 100);

		for (auto key : items)
			pcm.Insert(key.c_str());

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items_delete)
			pcm.Delete(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);
		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_d += (double)1000.0 * items_delete.size() / resns;

		for (auto pr : freq)
			ret_f[i].push_back(pcm.Query(pr.first.c_str()));
	}

	experiment_through_delete.push_back(throughput_d / testcycles);
	calcAcc(ret_f);
}

void test_PC(int mem_in_byte)
{
	int d = 4;
	int w_p = mem_in_byte * 8 / (WORD_SIZE * 2);

	vector<int> ret_f[testcycles];
	double throughput_d = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2, time3, time4;
		long long resns;

		PC_Sketch pc(w_p, d, WORD_SIZE, i * 100);

		for (auto key : items)
			pc.Insert(key.c_str());

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items_delete)
			pc.Delete(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);
		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_d += (double)1000.0 * items_delete.size() / resns;

		for (auto pr : freq)
			ret_f[i].push_back(pc.Query(pr.first.c_str()));
	}

	experiment_through_delete.push_back(throughput_d / testcycles);
	calcAcc(ret_f);
}

void test_MP_CM(int mem_in_byte)
{
	int d = 4;
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;

	vector<int> ret_f[testcycles];
	double throughput_d = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2;
		long long resns;

		MP_CM mp_cm(w, d, i * 100);

		for (auto key : items)
			mp_cm.Insert(key.c_str());

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items_delete)
			mp_cm.Delete(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);
		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_d += (double)1000.0 * items_delete.size() / resns;

		for (auto pr : freq)
			ret_f[i].push_back(mp_cm.Query(pr.first.c_str()));
	}

	experiment_through_delete.push_back(throughput_d / testcycles);
	calcAcc(ret_f);
}


void test_MP_C(int mem_in_byte)
{
	int d = 4;
	int w = mem_in_byte * 8 / COUNTER_SIZE / d;

	vector<int> ret_f[testcycles];
	double throughput_d = 0;
	long long estimate_sum = 0;
	for (int i = 0; i < testcycles; ++i)
	{
		timespec time1, time2;
		long long resns;

		MP_C mp_c(w, d, i * 100);

		for (auto key : items)
			mp_c.Insert(key.c_str());

		clock_gettime(CLOCK_MONOTONIC, &time1);
		for (auto key : items_delete)
			mp_c.Delete(key.c_str());
		clock_gettime(CLOCK_MONOTONIC, &time2);
		resns = (long long)(time2.tv_sec - time1.tv_sec) * 1000000000LL + (time2.tv_nsec - time1.tv_nsec);
		throughput_d += (double)1000.0 * items_delete.size() / resns;

		for (auto pr : freq)
			ret_f[i].push_back(mp_c.Query(pr.first.c_str()));
	}

	experiment_through_delete.push_back(throughput_d / testcycles);
	calcAcc(ret_f);
}

void test_zipf_all(char* dirname)
{
	int mem_in_byte = 5 << 20;
	cout << endl << "mem = " << (double)mem_in_byte / (1 << 20) << "MB : " << endl;  //show xxMB memory

	vector <string> name = { "0.dat", "1.dat", "2.dat" };
	for (int o = 0; o < name.size(); o++)
	{
		string name1 = dirname + name.at(o);
		readFile(name1.c_str());
		get_delete_items(20);

		cerr << "CM " << name.at(o) << endl;
		test_CM(mem_in_byte);
		test_PCM(mem_in_byte);
		test_MP_CM(mem_in_byte);

		cerr << "C " << name.at(o) << endl;
		test_C(mem_in_byte);
		test_PC(mem_in_byte);
		test_MP_C(mem_in_byte);

		items.clear();
		freq.clear();
		items_delete.clear();
	}

	exit(0);
}


int main()
{
	char* filename = "dataset name";
	//test_zipf_all("path to the synthetic datasets folder");
	readFile("path to the dataset", 1000000);

	int mem_in_byte = 1 << 20;
	for (int delete_percent = 0; delete_percent < 30; delete_percent += 5)
	{
		cout << endl << "delete rate = " << delete_percent << "% : " << endl;

		get_delete_items(delete_percent);

		cerr << "CM " << delete_percent << "%" << endl;
		test_CM(mem_in_byte);
		test_PCM(mem_in_byte);
		test_MP_CM(mem_in_byte);

		cerr << "C " << delete_percent << "%" << endl;
		test_C(mem_in_byte);
		test_PC(mem_in_byte);
		test_MP_C(mem_in_byte);

		reset_delete_items();
	}

	ofstream oFile;
	oFile.open("sheet.csv", ios::app);
	if (!oFile) return 0;
	oFile << filename << endl;
	oFile << "AAE," << "ARE," << "throughput" << endl;
	vector<string> name = { "CM", "C" };
	for (int o = 0, j = 0, dp = 0; o < experiment_AAE.size(); o++)
	{
		if (o % 3 == 0)
		{
			cout << endl;
			cout << name.at(j % 2) << " ";
			if (o % 6 == 0)
			{
				cout << dp << "% : " << endl;
				dp += 5;
			}
			else
			{
				cout << endl;
			}

			j++;
		}
		cout << "AAE: " << experiment_AAE.at(o) << "\tARE: " << experiment_ARE.at(o) << "\tinsert: " << experiment_through_delete.at(o) << endl;

		oFile << experiment_AAE.at(o) << "," << experiment_ARE.at(o) << "," << experiment_through_delete.at(o) << endl;
	}
	oFile.close();

	return 0;
}
