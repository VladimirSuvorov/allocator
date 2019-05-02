#ifndef ALLOCATOR_PROJECT_SINGLE_LINKED_LIST_HPP
#include "FixedBlockAllocator.hpp"
#define ALLOCATOR_PROJECT_SINGLE_LINKED_LIST_HPP
namespace allocator_project{
	namespace stl_compatible_container {
		//The most primitive implementation of a single list I know
		//In this context stl-compatible means: having iterators.
		template<typename T, typename Allocator = allocator::FixedBlockAllocator<T >>
		class SList {
			struct Node;
		public:
			using value_type = T;
			using pointer = T *;
			using const_pointer = const T*;
			using reference = T &;
			using const_reference = const T &;

			//inner iterator class - the most primitive iterator I know
			class iterator {
				friend class SList;
				Node* n_ = nullptr;
				iterator(Node* n) :n_(n) {};
			public:
				iterator() = default;
				iterator(const iterator& src) :
					n_(src.n_) {}

				iterator operator++() {
					return iterator((n_) ? n_ = n_->next : nullptr);
				}
				iterator operator++(int) {
					iterator r(n_);
					n_ = n_->next;
					return r;
				}
				bool operator!=(const iterator & i)const noexcept {
					return (i.n_ != n_);
				}
				bool operator==(const iterator & i)const noexcept {
					return (i.n_ == n_);
				}
				pointer operator->() {
					return *n_;
				}
				const_pointer operator->()const noexcept {
					return *n_;
				}
				reference operator *() {
					return **n_;
				}
				const_reference operator *()const noexcept {
					return **n_;
				}

				pointer to_pointer()noexcept {
					return (n_) ? *n_ : nullptr;
				}
				const_pointer to_pointer()const noexcept {
					return (n_) ? *n_ : nullptr;
				}
			};

			//using iterator = Node *;
			using const_iterator = const iterator;

			iterator begin() {
				return { root_ };
			}
			iterator end() {
				return { (last_->next) };
			}
			const_iterator cbegin()const noexcept {
				return root_;
			}
			const_iterator cend()const noexcept {
				return { (last_->next) };
			}

			//the most primitive functional for something called as "STL-compatible":
			//emplace_back an element and clear - all that the container can do (except giving a forward iterators).

			template<typename...As>
			decltype(auto) emplace_back(As && ...as) {
				if (root_)
					return ((last_) ?
					(onAllocate(last_, std::forward<As>(as)...)) :
						(root_->next = (onAllocate(last_, std::forward<As>(as)...))))->element;
				else
					return onAllocate(root_, std::forward<As>(as)...)->element;
			}
			void clear() {
				using allocator_traits_t = std::allocator_traits<decltype(allocator_)>;
				if (root_) {
					for (auto toRelease(root_), next(root_->next); next != nullptr; toRelease = next, next = next->next) {
						allocator_traits_t::destroy(allocator_, toRelease);
						allocator_traits_t::deallocate(allocator_, toRelease, 1);
					}
				}
				if (last_) {
					allocator_traits_t::destroy(allocator_, last_);
					allocator_traits_t::deallocate(allocator_, last_, 1);
				}
				root_ = last_ = nullptr;
				size_ = 0;
			}
			constexpr size_t size()const noexcept {
				return size_;
			}
			constexpr bool empty()const noexcept {
				return (!size());
			}

			~SList() {
				clear();//clean up automatically if forgot to do it before
			}

		private:
			//allocating code, very trivial
			template<typename...As>
			Node* onAllocate(Node * &ptr, As && ...as) {
				using allocator_traits_t = std::allocator_traits<decltype(allocator_)>;
				auto p = allocator_traits_t::allocate(allocator_, 1);
				allocator_traits_t::construct(allocator_, p, std::forward<As>(as)...);
				if (ptr)ptr->next = p;
				++size_;
				return ptr = p;
			}

			//inner, internal usage only, SList node class, very trivial
			class Node {
				T element;

				using next_ptr_t = Node *;
				next_ptr_t next = nullptr;
				friend class SList;
			public:
				//constructs from anything if it is possible
				template<typename...As>
				Node(As&& ...as) :
					element(std::forward<As>(as)...) {}
				//operators to extract and/or operate an element.
				pointer operator->() {
					return &element;
				}
				const_pointer operator->()const noexcept {
					return &element;
				}
				reference operator *() {
					return element;
				}
				const_reference operator *()const noexcept {
					return element;
				}
				operator reference()const noexcept {
					return element;
				}
				operator const_reference()const noexcept {
					return element;
				}
			};
			using next_ptr_t = typename Node::next_ptr_t;
			next_ptr_t root_ = nullptr;
			next_ptr_t last_ = nullptr;

			using Allocator_t = allocator::allocator_for<Allocator, Node>;
		private:
			Allocator_t allocator_;
			size_t size_ = 0;
		};
	}
}
#endif // !
