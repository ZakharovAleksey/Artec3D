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
	NormDistGenerator::NormDistGenerator(int32_t mu, int32_t sigma) : mu_(mu), sigma_(sigma) {}

	vector<int32_t> NormDistGenerator::Generate(size_t possible_bytes, const string & out_f_name)
	{
		try { return GenerateHelper(possible_bytes, out_f_name); }
		catch (const runtime_error & e)
		{
			cerr << "Runtime error: " << e.what() << endl;
		}
	}
	
	vector<int32_t> NormDistGenerator::GenerateHelper(size_t possible_bytes, const string & out_f_name)
	{
		normal_distribution<double> distribution(mu_, sigma_);
		default_random_engine generator;

		size_t el_numb = possible_bytes / sizeof(int32_t);
		vector<int32_t> result(el_numb, 0);

		ofstream ofs; ofs.open(out_f_name, ios::out | ios::binary);

		if (ofs.is_open())
		{
			for (int el_id = 0; el_id < el_numb; ++el_id)
			{
				int32_t cur_val = static_cast<int32_t>(distribution(generator));
				result[el_id] = cur_val;
				ofs.write((char*)&cur_val, sizeof(int32_t));
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