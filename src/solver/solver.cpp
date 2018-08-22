#include"solver.h"

#include<vector>
#include<algorithm>
#include<iterator>

#include<iostream>
#include<fstream>
#include<sstream>
#include<ios>
#include <stdio.h>

#include<future>

#define CHECK_IF_OPEN(ifs, input_file_name){									\
	if (!ifs.is_open()){														\
		ifs.close();															\
		ostringstream os;														\
		os << "Could not open file " << input_file_name << " for Read (Write).";\
		throw runtime_error(os.str());											\
	}																			\
}

namespace solver
{
	Solver::Solver(size_t bytes_availible, const string & in_file_name, const string & out_file_name) : 
		bytes_availible_(bytes_availible), chunks_numb_(0), unit_size_(sizeof(int32_t)),
		in_file_name_(in_file_name), out_file_name_(out_file_name) 
	{
		if (bytes_availible_ % unit_size_ != 0)
			throw(logic_error("Input availible bytes must be divide on sizeof(int32_t)[4 bytes]!"));
	}

	void Solver::Solve(bool is_single_thread)
	{
		if (is_single_thread)
			GenerateChunksSTHelper();
		else
			GenerateChunksMTHelper();

		MergeChunks();
	}

	void Solver::GenerateChunksST()
	{
		try { GenerateChunksSTHelper(); }
		catch (const runtime_error & e)
		{
			cerr << "Runtime error: " << e.what() << endl;
		}
		catch (...)
		{
			cerr << "Unknown error detected" << endl;
		}
	}

	void Solver::GenerateChunksMT()
	{
		try { GenerateChunksMTHelper(); }
		catch (const runtime_error & e)
		{
			cerr << "Runtime error: " << e.what() << endl;
		}
		catch (...)
		{
			cerr << "Unknown error detected" << endl;
		}
	}

	void Solver::MergeChunks()
	{
		// Create MinHeap with <Key, Value> = <cur. min. value in file with 'chunk_id', chunk_id>
		vector<MinHeapKV> min_heap(chunks_numb_, MinHeapKV());

		vector<ifstream> ifs_sorted; ifs_sorted.resize(chunks_numb_);
		for (size_t id = 0; id < chunks_numb_; ++id)
		{
			ostringstream os; os << "sorted_chunk_" << id << ".bin";
			ifs_sorted[id].open(os.str(), ios::in | ios::binary);
			CHECK_IF_OPEN(ifs_sorted[id], os.str())
			
			// Read first element from file with 'id' (min. elem. in this file)
			ifs_sorted[id].read(reinterpret_cast<char*>(&min_heap[id].value), unit_size_);
			min_heap[id].chunk_id = id;
		}

		make_heap(begin(min_heap), end(min_heap), MinHeapComparator());


		ofstream ofs;
		ofs.open(out_file_name_ , ios::out | ios::binary);
		CHECK_IF_OPEN(ofs, out_file_name_)

		while (!min_heap.empty())
		{
			// Take min elem. from the heap and write it to the result output file
			auto top = *begin(min_heap);
			pop_heap(begin(min_heap), end(min_heap), MinHeapComparator()); 

			if (ifs_sorted[top.chunk_id])
			{
				ofs.write((char*)& top.value, unit_size_);
				
				// Insert next minimal value from file with appropriate 'chunk_id'
				// as a min_heap last element and make SiftUp() via STL push_heap()
				ifs_sorted[top.chunk_id].read(reinterpret_cast<char*>(&rbegin(min_heap)->value), unit_size_);
				push_heap(begin(min_heap), end(min_heap), MinHeapComparator());
			}
			else
			{
				// Here we read all data from file with appropriate 'chunk_id': so we
				// - Close this file
				// - Remove last elem. from min_heap (we have no more elem. with 'chunk_id')
				min_heap.pop_back();
				ifs_sorted[top.chunk_id].close();

				// !!! Not Necessary !!! removement of file with empty chunk
				ostringstream os; os << "sorted_chunk_" << top.chunk_id << ".bin";
				remove(os.str().c_str());
			}
		}
		ofs.close();
	}

	void Solver::WriteHelper(ofstream & ofs, const string & cur_out_file_name, const vector<int32_t>& values)
	{
		ofs.open(cur_out_file_name, ios::out | ios::binary);
		CHECK_IF_OPEN(ofs, cur_out_file_name)

		ofs.write((char*)& values[0], unit_size_ * values.size());
		ofs.close();
	}

	void Solver::GenerateChunksMTHelper()
	{
		ifstream ifs(in_file_name_, ios::in | ios::binary);
		CHECK_IF_OPEN(ifs, in_file_name_);

		// Obtain incoming file length in bytes and number of chunks which is equal
		// to file length divided on availible bytes and rounded upwards (if last 
		// sub-part of data is less then availible bytes)
		ifs.seekg(0, ifs.end);
		int64_t file_byte_len = ifs.tellg();
		chunks_numb_ = static_cast<size_t>(ceil((long double) file_byte_len / bytes_availible_));
		
		// Multithreading part : need no mutex beacuse threads has no intersection
		vector<future<void>> futures;

		for (size_t chunk_id = 0; chunk_id < chunks_numb_; ++chunk_id)
		{
			futures.push_back(async([&, chunk_id, file_byte_len]
			{
				GenerateSingleChunk(chunk_id, file_byte_len);
			}));
		}
	}

	void Solver::GenerateSingleChunk(size_t chunk_id, const int64_t file_byte_len)
	{
		ifstream ifs(in_file_name_, ios::in | ios::binary);
		ofstream ofs;

		// Find out the starting position for reading in incoming file which depends on
		// current 'chunk_id' value | And obtain size of current chunk (could be less
		// than availible bytes)
		ifs.seekg(chunk_id * bytes_availible_, ios::cur);
		size_t left_bytes = (size_t)(file_byte_len - (int64_t)ifs.tellg());
		size_t buffer_size = (left_bytes < bytes_availible_) ? left_bytes : bytes_availible_;

		vector<int32_t> values(buffer_size / unit_size_, 0);
		ifs.read(reinterpret_cast<char*>(&values[0]), buffer_size);

		sort(begin(values), end(values));

		// Write current chunk to file with appropriate 'chunk_id'
		ostringstream os; os << "sorted_chunk_" << chunk_id << ".bin";
		WriteHelper(ofs, os.str(), values);
	}

	
	void Solver::GenerateChunksSTHelper()
	{
		ifstream ifs(in_file_name_, ios::in | ios::binary);
		CHECK_IF_OPEN(ifs, in_file_name_);

		ofstream ofs;

		ifs.seekg(0, ifs.end);
		int64_t file_byte_len = ifs.tellg();
		ifs.seekg(0, ifs.beg);

		// Write data in chunks while their size os not less than availible bytes
		vector<int32_t> values; values.resize(size_t(bytes_availible_ / (int64_t)unit_size_));
		size_t cur_read_bytes = 0;

		while (ifs && cur_read_bytes + bytes_availible_ <= file_byte_len)
		{
			ifs.read(reinterpret_cast<char*>(&values[0]), bytes_availible_);
			cur_read_bytes += bytes_availible_;
			sort(begin(values), end(values));

			ostringstream os; os << "sorted_chunk_" << chunks_numb_++ << ".bin";
			WriteHelper(ofs, os.str(), values);
		}

		// Additional reading of memory block which size is less than available bytes
		if (cur_read_bytes < file_byte_len)
		{
			// Prepare vector of appropriate size (which is left)
			values.swap(vector<int32_t>());
			values.resize((file_byte_len - cur_read_bytes) / unit_size_);

			ifs.read(reinterpret_cast<char*>(&values[0]), file_byte_len - cur_read_bytes);
			sort(begin(values), end(values));
				
			ostringstream os; os << "sorted_chunk_" << chunks_numb_++ << ".bin";
			WriteHelper(ofs, os.str(), values);
		}
		ifs.close();
	}

	ostream & operator<<(ostream & os, const MinHeapKV & kv)
	{
		os << kv.value << " (" << kv.chunk_id << ") ";
		return os;
	}
}