#pragma once

#include<string>
#include<vector>

namespace gen
{
	using namespace std;

	// Generate dataset of given size from ~N(mu, sigma^2)
	class NormDistGenerator
	{
	public:
		NormDistGenerator(int32_t mu = 10000, int32_t sigma = 100);

		// Generate dataset and returns vector of expected after sorting values
		vector<int32_t> Generate(size_t possible_bytes, const string & out_f_name);

	private:

		vector<int32_t> GenerateHelper(size_t possible_bytes, const string & out_f_name);

	private:
		int32_t mu_;
		int32_t sigma_;
	};
}
