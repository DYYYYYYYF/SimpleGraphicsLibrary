#pragma once

#include <memory>
#include <type_traits>
#include <memory>
#include <type_traits>

// 检测 shared_ptr
template<typename T>
struct is_shared_ptr : std::false_type {};

template<typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

// 检测 unique_ptr
template<typename T>
struct is_unique_ptr : std::false_type {};

template<typename T, typename D>
struct is_unique_ptr<std::unique_ptr<T, D>> : std::true_type {};

template<typename T>
inline constexpr bool is_unique_ptr_v = is_unique_ptr<T>::value;

// DynamicCast 主函数
template<typename To, typename From>
auto DynamicCast(From&& from) {
	using FromType = std::decay_t<From>;

	if constexpr (is_shared_ptr_v<FromType>) {
		// shared_ptr
		return std::dynamic_pointer_cast<To>(std::forward<From>(from));
	}
	else if constexpr (is_unique_ptr_v<FromType>) {
		// unique_ptr
		auto* raw = from.get();
		To* casted = dynamic_cast<To*>(raw);
		if (casted) {
			from.release();
			return std::unique_ptr<To>(casted);
		}
		return std::unique_ptr<To>(nullptr);
	}
	else if constexpr (std::is_pointer_v<FromType>) {
		// 裸指针
		return dynamic_cast<To*>(from);
	}
	else {
		static_assert(std::is_pointer_v<FromType>,
			"DynamicCast requires shared_ptr, unique_ptr, or raw pointer");
	}
}
