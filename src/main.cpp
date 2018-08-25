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

// Simple tests for Single(Multi)Thread implementation
void TestSingleThread();
void TestMultiThread();

void Test()
{
	TestRunner tr;
	// Short test 5 MB file
	RUN_TEST(tr, TestSingleThread);
	// Long test 128 MB file
	//RUN_TEST(tr, TestMultiThread);
}

#endif 

using namespace std::chrono;

#define LOG_EXEC_TIME(solver, func){												\
	auto t_start = system_clock::now();												\
	solver.func();																	\
	auto t_end = system_clock::now();												\
	clog << #func << ":" << duration_cast<milliseconds>(t_end - t_start).count()	\
		 << "ms." << endl;															\
}

void TestArtec3D(const string & input_file_name, const uint64_t initial_byte_len, const uint64_t initial_total_sum)
{
	ifstream ifs; ifs.open(input_file_name, ios::out | ios::binary);
	CHECK_IF_OPEN(ifs, input_file_name);

	ifs.seekg(0, ios::end);
	int64_t file_byte_len = ifs.tellg();
	ifs.seekg(0, ios::beg);

	if (file_byte_len != initial_byte_len)
	{
		ifs.close();
		clog << "Exp len: " << initial_byte_len << " != " << "Real len: " << file_byte_len << endl;
		throw runtime_error("Byte length are not equal");
	}

	uint32_t cur, next;
	size_t unit_size = sizeof(uint32_t);

	ifs.read(reinterpret_cast<char*>(&cur), unit_size);
	uint64_t total_sum{ cur };
	while (ifs.tellg() < file_byte_len)
	{
		ifs.read(reinterpret_cast<char*>(&next), unit_size);
		total_sum += next;
		if (cur > next)
		{
			ifs.close();
			throw runtime_error("Not increasing order is detected");
		}
		cur = next;
	}

	bool total_sums_equal = (total_sum == initial_total_sum);
	
	if (!total_sums_equal)
	{
		ifs.close();
		clog << "Exp sum: " << initial_total_sum << " != " << "Real sum: " << total_sum << endl;
		throw runtime_error("Total sums are not equal");
	}

	ifs.close();
}



int main()
{	
#ifdef TEST

	Test();
	size_t file_mem = 1'535'766'444; //268'435'456; // 12'582'912; // -> 12 Mb (In bytes)

#endif
	
	size_t free_mem = 134'217'728; //33'554'432; // 524'288;//104'857'600; // [0, 128x1024x1024] ->1Mb (In bytes)

	std::cout << "Input amount of availible free memory in range [0, 134.217.728] bytes : ";
	while (std::cin >> free_mem && (free_mem < 0 || free_mem > 134'217'728))
		std::cout << "Wrong input. Try again: ";
	
#ifdef TEST

	gen::NormDistGenerator gen;
	uint64_t total_sum = 0;
	gen.Generate(file_mem, "input.bin", total_sum);

#endif

	try
	{
		auto t_start = std::chrono::system_clock::now();

		solver::Solver s(free_mem, "input.bin", "output.bin");
		s.Solve();

		auto t_end = std::chrono::system_clock::now();
		std::clog << "Time: " << std::chrono::duration_cast<std::chrono::seconds>(t_end - t_start).count() << "s." << std::endl;

		// Artec3D test
		try
		{
			auto t_start = std::chrono::system_clock::now();
			TestArtec3D("output.bin", file_mem, total_sum);
			auto t_end = std::chrono::system_clock::now();
			std::clog << "Test Artec3D time: " << std::chrono::duration_cast<std::chrono::seconds>(t_end - t_start).count() << "s." << std::endl;
		}
		catch (const runtime_error & e)
		{
			cerr << "Artec3D test error: " << e.what();
			exit(1);
		}
		catch (...)
		{
			cerr << "Unknown error in Artec3D test is detected" << endl;
			exit(1);
		}

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
	size_t input_file_bytes = 5'242'883;
	size_t availible_bytes = 1'048'576;

	gen::NormDistGenerator gen;
	uint64_t total_sum = 0;
	auto expected = gen.Generate(input_file_bytes, in_file, total_sum);

	solver::Solver s(availible_bytes, in_file, out_file);

	LOG_EXEC_TIME(s, GenerateChunksST);
	LOG_EXEC_TIME(s, MergeChunks);

	ifstream ifs; ifs.open(out_file, ios::in | ios::binary);

	ifs.seekg(0, ifs.end);
	size_t file_byte_len = ifs.tellg();
	ifs.seekg(0, ifs.beg);

	vector<uint32_t> result(file_byte_len / sizeof(uint32_t), 0);
	ifs.read(reinterpret_cast<char*>(&result[0]), file_byte_len);
	ifs.close();

	ASSERT_EQUAL(expected, result);

	try
	{
		TestArtec3D(out_file, input_file_bytes, total_sum);
	}
	catch (const runtime_error & e)
	{
		cerr << "Artec3D test error: " << e.what();
		exit(1);
	}
	catch (...)
	{
		cerr << "Unknown error in Artec3D test is detected" << endl;
		exit(1);
	}

	remove(in_file.c_str());
	remove(out_file.c_str());
}

void TestMultiThread()
{
	const string in_file("in_tmp_mt.bin"), out_file("out_tmp_mt.bin");
	size_t input_file_bytes = 134'217'728;
	size_t availible_bytes = 33'554'432;

	gen::NormDistGenerator gen;
	uint64_t total_sum = 0;
	auto expected = gen.Generate(input_file_bytes, in_file, total_sum);

	solver::Solver s(availible_bytes, in_file, out_file);

	LOG_EXEC_TIME(s, GenerateChunksMT);
	LOG_EXEC_TIME(s, MergeChunks);


	ifstream ifs; ifs.open(out_file, ios::in | ios::binary);

	ifs.seekg(0, ifs.end);
	size_t file_byte_len = ifs.tellg();
	ifs.seekg(0, ifs.beg);

	vector<uint32_t> result(file_byte_len / sizeof(uint32_t), 0);
	ifs.read(reinterpret_cast<char*>(&result[0]), file_byte_len);
	ifs.close();

	ASSERT_EQUAL(expected, result);

	remove(in_file.c_str());
	remove(out_file.c_str());
}

#endif