#pragma once

#include<string>
#include<vector>

namespace solver
{
	using namespace std;

	// Stores pair of value (int32_t value) and chunk_id of this value
	// Necessary in Merge Chunk procedure
	struct MinHeapKV
	{
		int32_t value;
		size_t chunk_id;

		MinHeapKV() : value(0), chunk_id(0) {}
		MinHeapKV(int32_t _value, size_t _chunck_id) : value(_value), chunk_id(_chunck_id) {}

		friend ostream & operator<<(ostream & os, const MinHeapKV & kv);
	};

	struct MinHeapComparator 
	{
		bool operator()(const MinHeapKV & left, const MinHeapKV & right) const 
		{
			return left.value > right.value;
		}
	};

	// Solver performs sorting procedure
	class Solver
	{
	public:
		Solver(size_t bytes_availible, const string & in_file_name, const string & out_file_name);

		// Performs Sorting
		void Solve(bool is_single_thread = false);

		// Performs SingleThread creation of intermediate files with sorted chunks of initial data file
		// !!! public for time measurement purposes only !!!
		void GenerateChunksST();
		// Performs ASYNC MultiThread creation of intermediate files with sorted chunks of initial data file
		// !!! public for time measurement purposes only !!!
		void GenerateChunksMT();
		// Performs merge procedure for previously created files with sorted data chunks
		// !!! public for time measurement purposes only !!!
		void MergeChunks();

	private:

		// Performs SingleThread creation of intermediate files with sorted chunks of initial data file
		void GenerateChunksSTHelper();

		// MultiThread Helpers implementation
		void GenerateChunksMTHelper();
		// Create single chunck file with 'cur_id' in ASYNC mode
		void GenerateSingleChunk(size_t chunck_id, const int64_t file_byte_len);

		// Write Helper for values writing in chunks
		void WriteHelper(ofstream & ofs, const string & out_file_name, const vector<int32_t> & values);

	private:
		// Free available memory size
		size_t bytes_availible_;
		// Number of chunk files with sorted data
		size_t chunks_numb_;
		// Size of uint32_t
		size_t unit_size_;

		string in_file_name_;
		string out_file_name_;
	};
}