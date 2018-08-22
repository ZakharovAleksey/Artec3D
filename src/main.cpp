#include"unit_test\unit_test.h"
#include"generator\generator.h"
#include"solver\solver.h"

#include<iostream>
#include<chrono>
#include<cstdio>

#include<fstream>

// !!! If you need to perform input file generation and launch test scenarios define
// otherwise - comment this row
#define TEST

#ifdef TEST

using namespace unit_test;
using namespace std::chrono;

// Simple tests for Single(Multi)Thread implementation
void TestSingleThread();
void TestMultiThread();

void Test()
{
	TestRunner tr;
	RUN_TEST(tr, TestSingleThread);
	RUN_TEST(tr, TestMultiThread);
}

#endif 


#define LOG_EXEC_TIME(solver, func){												\
	auto t_start = system_clock::now();												\
	solver.func();																	\
	auto t_end = system_clock::now();												\
	clog << #func << ":" << duration_cast<milliseconds>(t_end - t_start).count()	\
		 << "ms." << endl;															\
}


int main()
{	
#ifdef TEST

	Test();
	size_t file_mem = 12'582'912; // -> 12 Mb (In bytes)

#endif

	size_t free_mem = 1'048'576; // [0, 128x1024x1024] ->1Mb (In bytes)

	std::cout << "Input amount of availible free memory in range [0, 134.217.728] bytes : ";
	while (std::cin >> free_mem && (free_mem < 0 || free_mem > 134'217'728))
		std::cout << "Wrong input. Try again: ";
	
#ifdef TEST

	gen::NormDistGenerator gen;
	gen.Generate(file_mem, "input.bin");

#endif

	try
	{
		solver::Solver s(free_mem, "input.bin", "output.bin");
		s.Solve();
	}
	catch (const std::runtime_error & e)
	{
		std::cerr << "Runtime error: " << e.what() << std::endl;
	}
	catch (const std::logic_error & e)
	{
		std::cerr << "Logic error: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown error detected." << std::endl;
	}

	return 0;
}

#ifdef TEST

void TestSingleThread()
{
	const string in_file("in_tmp_st.bin"), out_file("out_tmp_st.bin");
	size_t input_file_bytes = 5'242'880;;
	size_t availible_bytes = 1'048'576;

	gen::NormDistGenerator gen;
	auto expected = gen.Generate(input_file_bytes, in_file);

	solver::Solver s(availible_bytes, in_file, out_file);

	LOG_EXEC_TIME(s, GenerateChunksST);
	LOG_EXEC_TIME(s, MergeChunks);

	ifstream ifs; ifs.open(out_file, ios::in | ios::binary);

	ifs.seekg(0, ifs.end);
	size_t file_byte_len = ifs.tellg();
	ifs.seekg(0, ifs.beg);

	vector<int32_t> result(file_byte_len / sizeof(int32_t), 0);
	ifs.read(reinterpret_cast<char*>(&result[0]), file_byte_len);
	ifs.close();

	ASSERT_EQUAL(expected, result);

	remove(in_file.c_str());
	remove(out_file.c_str());
}

void TestMultiThread()
{
	const string in_file("in_tmp_mt.bin"), out_file("out_tmp_mt.bin");
	size_t input_file_bytes = 5'242'880;
	size_t availible_bytes = 1'048'576;

	gen::NormDistGenerator gen;
	auto expected = gen.Generate(input_file_bytes, in_file);

	solver::Solver s(availible_bytes, in_file, out_file);

	LOG_EXEC_TIME(s, GenerateChunksMT);
	LOG_EXEC_TIME(s, MergeChunks);


	ifstream ifs; ifs.open(out_file, ios::in | ios::binary);

	ifs.seekg(0, ifs.end);
	size_t file_byte_len = ifs.tellg();
	ifs.seekg(0, ifs.beg);

	vector<int32_t> result(file_byte_len / sizeof(int32_t), 0);
	ifs.read(reinterpret_cast<char*>(&result[0]), file_byte_len);
	ifs.close();

	ASSERT_EQUAL(expected, result);

	remove(in_file.c_str());
	remove(out_file.c_str());
}

#endif