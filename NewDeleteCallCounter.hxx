#ifndef ALLOCATOR_PROJECT_NEW_DELETE_CALL_COUNTER_HXX
#define ALLOCATOR_PROJECT_NEW_DELETE_CALL_COUNTER_HXX

#ifndef ALLOCATOR_PROJECT_NEW_DELETE_CALL_COUNTER_USE_OUTPUT
#define ALLOCATOR_PROJECT_NEW_DELETE_CALL_COUNTER_DONT_USE_OUTPUT
#endif

struct {
	size_t new_calls = 0;
	size_t delete_calls = 0;
	void reset_new_call_counter(size_t to = 0) {
		new_calls = to;
	}
	void reset_delete_call_counter(size_t to = 0) {
		delete_calls = to;
	}
	void on_new_is_called() {
		++new_calls;
	}
	void on_delete_is_called() {
		++delete_calls;
	}
}global_new_delete_counters;



template<typename R, typename E>
constexpr R not_return_throw() {
	throw E{};
}

void* operator new(std::size_t sz) {
	using namespace std;
	global_new_delete_counters.on_new_is_called();
#ifndef ALLOCATOR_PROJECT_NEW_DELETE_CALL_COUNTER_DONT_USE_OUTPUT	
	printf("%zu) allocation performed, size = %zu\n", global_new_delete_counters.new_calls, sz);
#endif
	auto ptr = malloc(sz);
	return (ptr) ? ptr : not_return_throw<void*, std::bad_alloc>();
}
void operator delete(void* ptr) noexcept {
	using namespace std;
	global_new_delete_counters.on_delete_is_called();	
#ifndef ALLOCATOR_PROJECT_NEW_DELETE_CALL_COUNTER_DONT_USE_OUTPUT	
	printf("%zu) deallocation performed\n", global_new_delete_counters.delete_calls);
#endif
	free(ptr);
}


#endif // ALLOCATOR_PROJECT_NEW_DELETE_CALL_COUNTER

