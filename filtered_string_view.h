#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <algorithm>
#include <compare>
#include <cstring>
#include <functional>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

namespace fsv {
	using filter = std::function<bool(const char&)>;
	class filtered_string_view {
		class iter {
		 public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using reference = const char&;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			iter() noexcept;

			auto operator*() const -> reference;
			auto operator->() const -> pointer;

			auto operator++() -> iter&;
			auto operator++(int) -> iter;
			auto operator--() -> iter&;
			auto operator--(int) -> iter;

			friend auto operator==(const iter&, const iter&) noexcept -> bool;
			friend auto operator!=(const iter&, const iter&) noexcept -> bool;

		 private:
			using ptr = const char*;
			iter(ptr start, ptr end, ptr curr, filter predicate);

			auto increment_ptr(ptr&) -> void;
			auto decrement_ptr(ptr&) -> void;
			ptr start_;
			ptr current_;
			ptr end_;
			filter predicate_;

			friend class filtered_string_view;
		};

	 public:
	 	static filter default_predicate;
		// inline static filter default_predicate = [](const char&) { return true; };
		using iterator = iter;
		using const_iterator = iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		auto begin() -> iterator;
		auto begin() const -> const_iterator;
		auto cbegin() const -> const_iterator;

		auto end() -> iterator;
		auto end() const -> const_iterator;
		auto cend() const -> const_iterator;

		auto rbegin() -> reverse_iterator;
		auto rbegin() const -> const_reverse_iterator;
		auto crbegin() const -> const_reverse_iterator;

		auto rend() -> reverse_iterator;
		auto rend() const -> const_reverse_iterator;
		auto crend() const -> const_reverse_iterator;

		filtered_string_view() noexcept;

		filtered_string_view(const std::string& str) noexcept;

		filtered_string_view(const std::string& str, filter predicate) noexcept;

		filtered_string_view(const char* str);

		filtered_string_view(const char* str, filter predicate);

		// copy constructor
		filtered_string_view(const filtered_string_view& other) noexcept;

		// move constructor
		filtered_string_view(filtered_string_view&& other) noexcept;

		// destructor
		~filtered_string_view() noexcept = default;

		// copy assignment
		auto operator=(const filtered_string_view& other) noexcept -> filtered_string_view&;

		// move assignment
		auto operator=(filtered_string_view&& other) noexcept -> filtered_string_view&;

		// subscript
		auto operator[](int n) const -> const char&;

		explicit operator std::string() const;

		auto at(int index) const -> const char&;

		auto size() const -> size_t;

		auto empty() const -> bool;

		auto data() const noexcept -> const char*;

		auto predicate() const noexcept -> const filter&;

		friend auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering;

		friend auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;

		friend auto operator!=(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;

		friend auto operator<(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;

		friend auto operator>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;

		friend auto operator<=(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;

		friend auto operator>=(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;

		friend auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream&;

	 private:
		const char* strptr_;
		std::size_t length_;
		filter predicate_;

		auto find_filtered_str_length() const -> std::size_t;

		auto swap(filtered_string_view& other) noexcept -> void;

		auto filter_string() const -> std::string;
	};

	// non-member utility functions
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) noexcept -> filtered_string_view;

	auto split(const filtered_string_view& fsv, const filtered_string_view& tok) -> std::vector<filtered_string_view>;

	auto substr(const filtered_string_view& fsv, int pos = 0, int count = 0) -> filtered_string_view;

} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
