#include "benchmark.h"
using namespace std;

int main(int argc, char* argv[]) {
	for (uint32_t i = 1; i < argc; ++i) {
		std::cout << argv[i] << std::endl;
		BenchMark<uint32_t, int32_t> dataset(argv[i], 1600);

		cout << "--------PE--------" << endl;
		for (uint32_t mem_in_byte = (1 << 20); mem_in_byte <= (6 << 20); mem_in_byte += (1 << 20))
		{
			double mem_in_MB = (double)mem_in_byte / (1 << 20);
			//cout << mem_in_MB << "\t";
			cout << "Memory = " << mem_in_MB << "MB" << endl;
			dataset.SketchError(10, mem_in_byte);
			cout << endl;
		}

		cout << "--------FPI--------" << endl;
		for (uint32_t mem_in_byte = (100 << 10); mem_in_byte <= (600 << 10); mem_in_byte += (100 << 10))
		{
			double mem_in_KB = (double)mem_in_byte / (1 << 10);
			//cout << mem_in_KB << "\t";
			cout << "Memory = " << mem_in_KB << "KB" << endl;
			dataset.TopKError(0.00005, mem_in_byte);
			cout << endl;
		}

		//dataset.Thp();
	}
	return 0;
}
