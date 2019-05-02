#ifndef ALLOCATOR_PROJECT_TESTING_CORE_HXX
#include <map>
#include <gtest/gtest.h>
#include "FixedBlockAllocator.hpp"
#include "NewDeleteCallCounter.hxx"
#define ALLOCATOR_PROJECT_TESTING_CORE_HXX


template<typename T, typename S, S N, T value>
bool mustAllocateMemoryNotMoreThanOneTimePer_N_elements() {
	using namespace std;
	using allocator_project::allocator::FixedBlockAllocator;
	global_new_delete_counters.reset_new_call_counter();
	global_new_delete_counters.reset_delete_call_counter();

	map<S,
		T,
		less<S>,
		FixedBlockAllocator<pair<const S, T>, S, N>> m;
	for (S index(0); index < N; ++index)
		m[index] = value;
	return (global_new_delete_counters.new_calls <= 1);
}


template<typename T, typename S, S N, T value>
bool mustAllocateAndDeallocateMemorySameTimes() {
	using namespace std;
	using allocator_project::allocator::FixedBlockAllocator;
	global_new_delete_counters.reset_new_call_counter();
	global_new_delete_counters.reset_delete_call_counter();
	constexpr S some_multiplier_value = 9;//can be any, but you should to note, that it can slow down the test case if it would be too big

	[]() {
		map<S,
			T,
			less<S>,
			FixedBlockAllocator<pair<const S, T>, S, N>> m;
		for (S index(0); index < (N * some_multiplier_value); ++index)
			m[index] = value;
	}();

	return	(global_new_delete_counters.delete_calls == global_new_delete_counters.new_calls);
}



template<typename T, typename S, S N, T value, S lessThanN>
bool memoryBlockMustBeEmptyBeforeAllocationAndDeallocateCorrectly() {
	using namespace std;
	using allocator_project::FixedStackMemoryBlock;
	global_new_delete_counters.reset_new_call_counter();
	global_new_delete_counters.reset_delete_call_counter();
	//the core of an allocator, allocates memory for elements inside itself or a new instance of itself on the heap.
	FixedStackMemoryBlock<T, S, N> fsmb;

	//initially must be empty, with free space == N, has no next
	if ((!fsmb.empty()) || (fsmb.free_size() != N) || fsmb.hasNext())return false;
	
	auto allocLessThanN = fsmb.allocate(lessThanN);		//allocated == lessThanN

	//neither allocation nor deallocation must be performed, as well as no next block must appear
	if ((global_new_delete_counters.new_calls != 0) || (global_new_delete_counters.delete_calls != 0) || fsmb.hasNext())
		return false;
		
	auto allocUpToExactN = fsmb.allocate((N-lessThanN));//allocated == lessThanN + (N - lessThanN) == N

	//there still neither allocation nor deallocation must be performed , as well as no next block must appear
	if ((global_new_delete_counters.new_calls != 0) || (global_new_delete_counters.delete_calls != 0) || fsmb.hasNext())
		return false;
	
	auto allocUpToMoreThanN = fsmb.allocate(lessThanN);	//allocated == N + lessThanN // MORE THAN N
	   
	//OKAY: something could be allocated, but the must not be deallocated anything, the next block must appear
	if ((global_new_delete_counters.new_calls == 0)|| (global_new_delete_counters.delete_calls != 0) || (!fsmb.hasNext()))
		return false;

	//curent block has next one, but next one must not have another next one.
	//MUST BE: [current]->[next!=nullptr]->[another next==nullptr]
	if (fsmb.hasNext()->hasNext())return false;

	fsmb.deallocate(allocUpToExactN, (N - lessThanN));// start deallocate from the middle 

	//as far as we set a free (N - lessThanN) elements, there must be a free space, but next block must be still existing.
	//no deletion must occur here.
	if ((fsmb.free_size() != (N - lessThanN)) || (!fsmb.hasNext()) || (global_new_delete_counters.delete_calls != 0))return false;

	fsmb.deallocate(allocUpToMoreThanN, lessThanN);// deallocate last block
	
	//as far as all memory of the last block was deallocated, the last block must be deleted, 
	//whereas the current one must have a free space as it had before.
	if ((fsmb.free_size() != (N - lessThanN)) || (fsmb.hasNext()) || (global_new_delete_counters.delete_calls == 0))return false;

	global_new_delete_counters.reset_delete_call_counter();
	fsmb.deallocate(allocLessThanN, lessThanN);// deallocate all remaining memory, no actual deletion must be performed
	
	//after all, memory block must be empty and clear.
	//cos delete counter was reset to 0, before the command and command does not lead to another deletion,
	//delete counter must be = 0.
	if ((!fsmb.empty()) || (fsmb.free_size() != N) || (global_new_delete_counters.delete_calls != 0))return false;

	bool thrown = false;
	try {
		fsmb.allocate((N * N));//must throw bad_alloc, cos cant allocate more than N at a given time
	} catch (const std::bad_alloc& e) {
		thrown = true;
	}
	if (!thrown)return false;

	//it must not say, that it owns a pointer (e.g. nullptr), that does not belongs to it
	if(fsmb.owns(nullptr))return false;

	return true;
}

template<typename T, typename S, S N, T value>
bool customSListContainerTest(){
	using namespace std;
	using allocator_project::allocator::FixedBlockAllocator;
	using allocator_project::stl_compatible_container::SList;
	SList<T, FixedBlockAllocator<T, S, N>> slst;
	global_new_delete_counters.reset_delete_call_counter();
	global_new_delete_counters.reset_new_call_counter();

	//returned value and inserted in this case must be same
	if (slst.emplace_back(value) != value)return false;
	//after 1 insertion size==1 and no allocations
	if ((slst.size() != 1)||(global_new_delete_counters.new_calls!=0))return false;

	slst.clear();//must be empty after clearing
	if (!slst.empty())return false;

	return true;
}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#endif // !TESTING_CORE_HPP
