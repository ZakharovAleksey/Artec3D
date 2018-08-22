# External sorting

Implementation of external sorting algorithm.

## Algorithm description

Implementation of external sorting algorithm is based on the folowing steps:
1. Performing of subdivision of initial dataset on chunks which are fit to the availible memory.
2. Performing a sorting procedure for each of this chunks and store intermidiate sorted data in files.
3. Performing k-way merge algorithm (here k - is the number of intermidiate chunks, which is equal to number of created files) based on min-heap.

More detailed explanation of k-way merge algorithm:
* Create min-heap from first elements in intermidiate sorted files (first elements are minimal in each file, because they are sorted).
* Take top element in min-heap ( top element in min-heap is minimal element in heap ; cost: O(1) ) and write it down to the result output file.
* Now we need to replace just taken minimal element with next element from  THE SAME chunk, inser it to min-heap end and perform SiftUp ( cost: O(logN) for Binary Heap) procedure to restore min-heap properties.
* Repeat previous steps, untill the min-heap is empty.

P.S. 
1. In this algorithm parts (1 and 2) could be performed asynchronously, so we can use async to increase algorithm.
2. [ MAY BE ] In this algorihm we could increase k-way merge algorithm: reading from EXTERNAL memory takes about 10ms, but we could read not only one element - we could read a block of memory (In the same 10 ms.). So we need to make min-heap not from only first elements of chunks- we need more elements from each chunck (until the fit in  availible memory). This will reduce number of reading from EXTERNAL memory: we read from EXTRERNAL memory only when queue of elements ended for the current chunk, and instead of reading one elment we read block of elements. 

## How to run application

To run this application you need to (for me it works on Windows 10, Visual Studio 2015):
1. Build solution with CMake (Version is not less than 3.5).
2. Run application with Visual Studio 2015.

## Prerequisities
To deploy or run this solution you need:

1. [CMake](https://cmake.org/download/) - is cross-platform free and open-source software for managing the build process of software using a compiler-independent method.
2. [Visual Studio 2015](https://www.visualstudio.com/ru/downloads/) - is an integrated development environment (IDE) from Microsoft used in this pgoject.
