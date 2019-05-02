#ifndef ALLOCATOR_PROJECT_FIXED_BLOCK_ALLOCATOR_HPP
#include "FixedStackMemoryBlock.hpp"
#define ALLOCATOR_PROJECT_FIXED_BLOCK_ALLOCATOR_HPP
namespace allocator_project{
	namespace allocator {
		//The most primitive but respectively fast allocator, that doesn't support flat memory order.
		//In the best case (asked once for less or equals to N elements) doesn't allocate anything, 
		//cos uses internal fixed memory block for N elements.
		//In the worst case allocates batches (see FixedStackMemoryBlock) with N-size fixed capacity in addition to internal one.
		//Probably incompatible with std::vector when required size more than N,
		//otherwise looks useless for it (use std::array<T,N> instead).
		//Possible usage is in a linked data structures like lists and trees.
		//std::bad_alloc is guaranteed (146%) to be thrown, if you would request more than N elements at a given time,
		//like this:
		//.allocate(M);// where M>N throws std::bad_alloc in 10 of 10 cases.
		//
		//It is not guaranteed, that it would free space for each element you deallocate internally, 
		//but it is strictly guaranteed that any memory block of N size will be empty after N deallocation of its elements.
		//
		//The batch (==memory block) will bee set a free (deleted) if it is completely empty and the previous one have a free space.
		//
		//NOTE: there some uninvestigated issues with _Container_proxy on MSVC in Debug Mode, especially for x86.
		template<typename T, typename S = std::size_t, S N = 10>
		class FixedBlockAllocator {
		public:
			using value_type = T;
			using pointer = T *;
			using size_type = S;

			FixedBlockAllocator() = default;
			~FixedBlockAllocator() = default;

			template<typename U, typename Z = size_type, Z M = N>
			struct rebind {
				using other = FixedBlockAllocator<U, S, N>;
			};

			template<typename U, typename Z = size_type, Z M = N>
			FixedBlockAllocator(const FixedBlockAllocator<U, S, N>& oth) {}


			void deallocate(pointer p, size_type hint) noexcept {
				return built_in_memory_block_.deallocate(p, hint);
			}
			pointer allocate(size_type n, pointer hint = nullptr) {
				return built_in_memory_block_.allocate(n, hint);
			}


		private:
			//there is no allocation occur here at all, untill we need only N elements; 
			//otherwise we allocate exactly same N-sized blocks
			FixedStackMemoryBlock<value_type, size_type, N> built_in_memory_block_;
		};		

		template<typename Allocator, typename For>
		using allocator_for =  typename std::allocator_traits<Allocator>::template rebind_alloc<For>;
	}
}
#endif