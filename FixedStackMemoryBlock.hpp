#ifndef ALLOCATOR_PROJECT_FIXED_STACK_MEMORY_BLOCK_HPP
#include <exception>
#include <memory>
#include "utility.hpp"
#define ALLOCATOR_PROJECT_FIXED_STACK_MEMORY_BLOCK_HPP
namespace allocator_project {

	//NOTE: this is not an exact allocator, it just locks same, but it is not designed to be an allocator at all.
	//Represents a single memory block with a possibility to contain N elements of a given type.
	//
	//Internally, behaves similar to a stack data structure built on top of a fixed size array, but not exactly same.
	//Extensible via allocation of a next instance (like slist).
	//
	//NOTE: there some uninvestigated issues with _Container_proxy on MSVC in Debug Mode, especially for x86.
	template<typename T, typename S = std::size_t, S N = 10>
	struct FixedStackMemoryBlock {
		using value_type = T;
		using pointer = T *;
		using const_pointer = const T*;
		using size_type = S;
		using element_array_t = utility::element_array<value_type, N>;

		//checks if a pointer relates to a given memory range 
		bool owns(pointer ptr)const noexcept {
			return utility::owns(elements_, ptr);
		}

		//if has no elements inside
		bool empty()const noexcept {
			return (aquired_ <= cbegin());
		}

		//count of cells where an element could be placed
		size_type free_size()const noexcept {
			return (cend() - aquired_);
		}

		//free an element. If x elements allocated, it is guaranteed that after x calls of deallocate the MemoryBlock will be empty().
		void deallocate(pointer p, size_type hint=1)noexcept {
			if (p > aquired_)next_deallocate(p, hint);
			else if (!this_deallocate(p, hint))
				next_deallocate(p, hint);
		}

		//Allocates n elements, where n<=N.
		//if n>N std::bad_alloc will be thrown.
		//if free_size()<n, allocates next MemoryBlock and gives elements from there
		pointer allocate(size_type n, pointer hint=nullptr) {
			if (n > N)throw std::bad_alloc();//can not allocate more than N == block size :(
			if ((aquired_ + n) > cend())//delegating the allocation to the new one if can not alocate itself
				return  (next_) ? next_->allocate(n, hint) : allocate_next(n, hint);
			return ((aquired_ += n) - n);//just allocate if we can
		}

		//returns completely const intenal pointer to the next block if has it, otherwise nullptr
		//useful in cases you need simply check if block has next instance as well as for analyzing next instance.
		//usage:
		// if(auto next = mblock.hasNext())
		//		//analyse next
		// else //handle alternative scenario
		const FixedStackMemoryBlock * const  hasNext()const noexcept {
			return next_.get();
		}

		~FixedStackMemoryBlock() {
		}
	private:
		//Tries to dealocate a pointer from this block, if a pointer relates to it's memory space,
		//100% fails if the pointer isn't for this block memory space.
		//Returns the result of the try, on false will redirect the job to the next memory block.
		bool this_deallocate(pointer p, size_type hint) {
			bool shouldDeallocate = owns(p);
			if (shouldDeallocate) {
				unwind((p + hint) >= aquired_, p, hint);//fails if elements would be deallocated not in LIFO order,
				reset(((deallocated_+=hint) >= N));//provides a strict gurantee that all x ocupied elems. will be set a free after x calls of this method
				if(next_)
					if (free_size() && next_->empty())
						next_.reset();			
			}
			return !shouldDeallocate;
		}

		//unwind & reset
		//are for convinience only. First argument says if the operation should be done; 
		void unwind(bool shouldUnwind, pointer p, size_type hint) {
			if (shouldUnwind)aquired_ = p;
		}
		void reset(bool shouldReset) {
			if (shouldReset) {
				aquired_ = utility::get_pointer<value_type>(elements_);
				deallocated_ = 0;
			}
		}

		//delegates deallocation of a memory by the givent pointer to the next MemoryBlock if the block exists.
		//deallocates the block after the operation if the block is empty and previous one has a free space.
		void next_deallocate(pointer p, size_type hint) {
			if (next_) {
				next_->deallocate(p, hint);
				if (free_size() && next_->empty())
					next_.reset();
			}
		}

		//real memory allocation occurs only here
		//allocates next block, if the current one (and all the previous) can't satisfy the request for a memory 
		//and it is possible to satisfy such request (requested less or equal to N elements)
		pointer allocate_next(size_type n, pointer hint) {
			return (next_ = std::make_unique<FixedStackMemoryBlock>())->allocate(n, hint);
		}

		//cend & cbegin
		//are for convinience only. Not an iterators, just handy methods with similar semantics.
		constexpr const_pointer cend()const noexcept {
			return (cbegin() + N);
		}
		constexpr const_pointer cbegin()const noexcept {
			return utility::get_pointer<value_type>(elements_);
		}

		
		size_type deallocated_ = 0;//provides strict guarantee, that if deallocates N times, aquired_ will be set to initial state
		element_array_t elements_;
		pointer aquired_ = utility::get_pointer<value_type>(elements_);
		std::unique_ptr<FixedStackMemoryBlock> next_;
	};
}
#endif