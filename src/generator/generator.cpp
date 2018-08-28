#include"generator.h"

#include<iostream>
#include<fstream>
#include<ios>

#include<exception>
#include <random>

// REMOVe
#include<algorithm>
#include<iterator>

namespace gen
{
	vector<uint32_t> ValuesGenerator::Generate(size_t possible_bytes, const string & out_f_name, uint64_t & total_sum)
	{
		try { return GenerateHelper(possible_bytes, out_f_name, total_sum); }
		catch (const runtime_error & e)
		{
			cerr << "Runtime error: " << e.what() << endl;
		}
	}
	
	vector<uint32_t> ValuesGenerator::GenerateHelper(size_t possible_bytes, const string & out_f_name, uint64_t & total_sum)
	{
		random_device rand_dev;
		mt19937 generator(rand_dev());
		uniform_int_distribution<uint32_t> dist_unif;

		size_t el_numb = possible_bytes / sizeof(uint32_t);
		vector<uint32_t> result(el_numb, 0);

		ofstream ofs; ofs.open(out_f_name, ios::out | ios::binary);

		if (ofs.is_open())
		{
			for (int el_id = 0; el_id < el_numb; ++el_id)
			{
				uint32_t cur_val = dist_unif(generator);
				result[el_id] = cur_val;
				total_sum += static_cast<uint64_t>(cur_val);
				ofs.write((char*)&cur_val, sizeof(uint32_t));
			}

			sort(begin(result), end(result));
		}
		else
		{
			ofs.close();
			throw runtime_error("Could not open file " + out_f_name + " for values generation.");
		}
		ofs.close();

		return result;
	}
}