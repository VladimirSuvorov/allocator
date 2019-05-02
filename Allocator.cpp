
#include <iostream>
#include <map>

#include "NewDeleteCallCounter.hxx"

#include "SList.hpp"
using allocator_project::FixedStackMemoryBlock;


size_t factorial(size_t index)noexcept {
	return (index) ? (index * factorial(index - 1)) : 1;
}

template<
	typename Key,
	typename Value = Key,
	typename Allocator= allocator_project::allocator::FixedBlockAllocator<std::pair<const Key, Value>>>
	using less_ordered_map=
	std::map<
	Key,
	Value,
	std::less<Key>,
	Allocator>;


//utility function to run container<allocator> cases:
//ContainerType must be specified explicitly, the container to work with
//initial_message - will be printed at the begining of the case 
//output_format - will be printed before output, should be related to the OutputF implementation
//InitializationF - how to initialize the container, takes 1 param - the container, by reference
//OutputF - how to output the container's content, takes 1 param - the container, by reference
template<typename ContainerType, typename InitializationF, typename OutputF>
void run_case(std::string_view initial_message, std::string_view output_format, InitializationF init, OutputF out) {
	using namespace std;
	using allocator_project::utility::printInitialMessage;
	using allocator_project::utility::printFormatNotificationMessage;
	using allocator_project::utility::printMemoryAllocDeallocNotificationMessage;
	printInitialMessage(initial_message);
	global_new_delete_counters.reset_new_call_counter();
	global_new_delete_counters.reset_delete_call_counter();
	printMemoryAllocDeallocNotificationMessage("allocations");
	ContainerType container;
	init(container);
	printFormatNotificationMessage(output_format);
	out(container);
	cout << endl;
	printMemoryAllocDeallocNotificationMessage("deallocations");
}
#include "TestingCore.hxx"
int main(int argc, char* argv[]) {
	using namespace std;
	using allocator_project::allocator::FixedBlockAllocator;
	using allocator_project::stl_compatible_container::SList;

	
	run_case<map<size_t, size_t>>(
		"std::map with default allocator runs:", 
		"[index] = value;",
		[](map<size_t, size_t> & m) {
		for (size_t i(0); i < 10; ++i)
			m[i] = factorial(i + 1);
	},
		[](map<size_t, size_t>& m) {
		for (size_t i(0); i < 10; ++i)
			std::cout << '[' << i << "] = " << m[i] << ';' << endl;
	});
	cout << endl;

	run_case<less_ordered_map<size_t>>(
		"std::map with custom allocator runs:",
		"[index] = value;",
		[](less_ordered_map<size_t> & m) {
		for (size_t i(0); i < 10; ++i)
			m[i] = factorial(i + 1);
	},
		[](less_ordered_map<size_t> & m) {
		for (size_t i(0); i < 10; ++i)
			std::cout << '[' << i << "] = " << m[i] << ';' << endl;
	});
	cout << endl;

	run_case<SList<int, std::allocator<int>>>(
		"custom container - SList - runs with std::allocator:",
		"value;",
		[](SList<int, std::allocator<int>> & l) {
		for (size_t i(0); i < 10; ++i)
			l.emplace_back(i);
	},
		[](SList<int, std::allocator<int>> & l) {
		for (const auto& el : l)
			std::cout << el << ';' << endl;
	});
	cout << endl;

	run_case<SList<int>>(
		"custom container - SList - runs with std::allocator:",
		"value;",
		[](SList<int> & l) {
		for (size_t i(0); i < 10; ++i)
			l.emplace_back(i);
	},
		[](SList<int> & l) {
		for (const auto& el : l)
			std::cout << el << ';' << endl;
	});
	cout << endl;

	return 0;
}
