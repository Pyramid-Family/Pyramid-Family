## How to run

Before running, modify the path in the main function (Line 591 in main.cpp, Line 331 in delete.cpp) to your dataset file.

After that, compile and run the program with the following commands.

```bash
$ cmake .
$ make
$ ./test
$ ./delete_test
```

If you want to use your own parameters, please modify the parameters defined in ```include/params.h``` and the main function.

We also provide code to test the effect of skewness on performance.
If you want to conduct experiments on synthetic datasets, please call function ```test_zipf_all``` in the main function.
