#include <iostream>
#include <fstream>
#include <unordered_map>
#include "sketches/SBF.h"
#include "sketches/MP-SBF.h"

vector<string> items;
unordered_map<string, int> freq;

void fileReader(const char* filename, int MAX_ITEM = INT32_MAX)
{
	ifstream inFile(filename, ios::binary);
	ios::sync_with_stdio(false);

	char key[13], timestamp[8];
	for (int i = 0; i < MAX_ITEM; ++i)
	{
		inFile.read(key, 13);
		if (inFile.gcount() < 13) break;
		//inFile.read(timestamp, 8);
		string str = string(key, 13);
		items.push_back(str);
		freq[str]++;
	}
	inFile.close();

	cout << "dataset name: " << filename << endl;
	cout << freq.size() << "flows, " << items.size() << " items read" << endl;
}

void test_SBF(int mem_in_byte)
{
	int d = 4;
	int w = mem_in_byte * 8 / 32;
	int w_p = w * 2 / 3, w_s = w / 3;
	SBF_RM sbf(w_p, w_s, d);

	for (auto key : items)
		sbf.insert(key);

	double ARE = 0, AAE = 0;
	for (auto pr : freq)
	{
		int est_val = sbf.query(pr.first);
		int real_val = pr.second;
		int dist = abs(est_val - real_val);
		ARE += (double)dist / real_val, AAE += dist;
	}
	ARE /= freq.size(), AAE /= freq.size();
	//cout << ARE << "\t" << AAE << "\t";
	cout << "SBF ARE = " << ARE << ", AAE = " << AAE << endl;
}

void test_MP_SBF(int mem_in_byte)
{
	int d = 4;
	int w = mem_in_byte * 8 / 32;
	int w_p = w * 2 / 3, w_s = w / 3;
	MP_SBF_RM sbf(w_p, w_s, d);

	for (auto key : items)
		sbf.insert(key);

	double ARE = 0, AAE = 0;
	for (auto pr : freq)
	{
		int est_val = sbf.query(pr.first);
		int real_val = pr.second;
		int dist = abs(est_val - real_val);
		ARE += (double)dist / real_val, AAE += dist;
	}
	ARE /= freq.size(), AAE /= freq.size();
	//cout << ARE << "\t" << AAE << endl;
	cout << "MP-SBF ARE = " << ARE << ", AAE = " << AAE << endl;
}

void test(int mem_in_byte)
{
	double mem_in_MB = (double)mem_in_byte / (1 << 20);
	cout << "Memory = " << mem_in_MB << "MB:" << endl;
	test_SBF(mem_in_byte);
	test_MP_SBF(mem_in_byte);
}

int main()
{
	fileReader("path to the dataset", 1000000);
	for (int mem_in_byte = 1 << 20; mem_in_byte <= 6 << 20; mem_in_byte += 1 << 20)
		test(mem_in_byte);
	return 0;
}
