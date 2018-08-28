#pragma once

#include<string>
#include<vector>

namespace gen
{
	using namespace std;

	// Generate dataset of given size from ~N(mu, sigma^2)
	class ValuesGenerator
	{
	public:
		ValuesGenerator() = default;

		// Generate dataset and returns vector of expected after sorting values
		vector<uint32_t> Generate(size_t possible_bytes, const string & out_f_name, uint64_t & total_sum);

	private:
		vector<uint32_t> GenerateHelper(size_t possible_bytes, const string & out_f_name, uint64_t & total_sum);

	};
}
