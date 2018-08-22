#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#pragma once

#include <iostream>

#include <stdexcept>
#include <sstream>
#include <exception>
#include <string>

#include<vector>
#include<algorithm>
#include<iterator>

namespace unit_test
{
	using namespace std;

	template<class U>
	ostream & operator<<(ostream & os, const vector<U> & v)
	{
		copy(begin(v), end(v), ostream_iterator<U>(os, " "));
		os << endl;
		return os;
	}

	template<class T, class U>
	void AssertEqual(const T & left, const U & right, const string hint)
	{
		if (right != left)
		{
			ostringstream os;
			os << left << " != " << right;
			if (!hint.empty())
				os << " Hint: " << hint;
			throw runtime_error(os.str());
		}
	}

	class TestRunner
	{
	public:

		TestRunner();
		~TestRunner();

		template<class TestFunc>
		void RunTest(TestFunc test, const string & test_name)
		{
			try
			{
				test();
				cerr << test_name << " OK" << endl;
			}
			catch (runtime_error & e)
			{
				++fail_count_;
				cerr << test_name << " failed. Error: " << e.what() << endl;
			}
			catch (...)
			{
				++fail_count_;
				cerr << test_name << " failed with Unknown error." << endl;
			}
		}

	private:
		size_t fail_count_;
	};
}


#define ASSERT_EQUAL(left, right) {					\
	ostringstream os;								\
	os << #left << " != " << #right << ", "			\
	<< __FILE__ << ": line " << __LINE__ << endl;	\
	unit_test::AssertEqual(left, right, os.str());	\
}

#define RUN_TEST(test_runner, test_func){			\
	test_runner.RunTest(test_func, #test_func);		\
}


#endif // !UNIT_TEST_H
