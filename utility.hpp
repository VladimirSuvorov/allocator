#ifndef ALLOCATOR_PROJECT_UTILITY_HPP
#include <array>
#include <cstddef>
#define ALLOCATOR_PROJECT_UTILITY_HPP
namespace allocator_project {
	namespace utility {
		namespace {
			using byte_t = uint8_t;	//std::byte
			template<typename B, size_t size>
			using byte_array = std::array<B, size>;

			template<typename T, size_t size>
			using array_type = T alignas(alignof(T[size]))[size];

			template<typename T, size_t size>
			constexpr size_t array_size = sizeof(array_type<T, size>);
		}
		template<typename T, size_t size, typename B = byte_t>
		using element_array = byte_array<B, array_size<T, size>>;


		//checks if a given pointer relates to a given memory space, representes via byte_array
		template<typename B, size_t size>
		constexpr bool owns(const byte_array<B, size>& byte_arr, void* ptr)noexcept {
			auto p = static_cast<B*>(ptr);
			return (p >= byte_arr.data()) && ((p - byte_arr.data()) <= byte_arr.size());
		}
		namespace {
			template<typename R>
			constexpr R* as(void* ptr) {
				return static_cast<R*>(ptr);
			}
			template<typename R>
			constexpr const R* as(const void* ptr) {
				return static_cast<const R*>(ptr);
			}
		}
		//returns pointer to an indexed element of type T that could be placed at a memory space of a given ElementArray 
		template<typename T, typename ElementArray>
		constexpr decltype(auto) get_pointer(ElementArray & data, size_t index = 0)noexcept {
			return as<T>(data.data() + (sizeof(T) * index));
		}



		void printMessage(
			std::string_view before_message,
			std::string_view note_message,
			std::string_view after_message,
			std::string_view very_long_delimiter_line) {
			using namespace std;
			cout
				<< very_long_delimiter_line << endl
				<< before_message << note_message << after_message << endl
				<< very_long_delimiter_line << '\n' << endl;
		}
		void printInitialMessage(std::string_view note_message) {
			constexpr auto very_long_line = "======================================================================";
			printMessage("", note_message, "", very_long_line);
		}
		void printFormatNotificationMessage(std::string_view note_message) {
			constexpr auto very_long_line = "----------------------------------------------------------------------";
			printMessage("NOTE: output format is: '", note_message, "' - per line", very_long_line);
		}
		void printMemoryAllocDeallocNotificationMessage(std::string_view note_message) {
			using namespace std;
			constexpr auto very_long_line = "/********************************************************************/";
			printMessage("You can see ", note_message, " performed here immediately below this notification message:", very_long_line);
		}
	}
}
#endif // !ALLOCATOR_PROJECT_UTILITY_HPP
