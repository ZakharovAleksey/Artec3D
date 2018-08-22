#include "unit_test.h"

namespace unit_test
{
	TestRunner::TestRunner() : fail_count_(0) { }
	TestRunner::~TestRunner()
	{
		if (fail_count_ > 0)
		{
			cerr << "Failed Tests number = " << fail_count_ << ". Terminate." << endl;
			exit(1);
		}
		else
		{
			cerr << "All Tests has been passed succesfully." << endl;
		}
	}
}



