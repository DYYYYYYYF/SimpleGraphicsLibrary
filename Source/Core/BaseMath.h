#pragma once

#include <Eigen/Dense>

using FVector2 = Eigen::Vector2f;
using FVector3 = Eigen::Vector3f;
using FVector4 = Eigen::Vector4f;
using FMatrix3 = Eigen::Matrix3f;
using FMatrix4 = Eigen::Matrix4f;

using FVector = Eigen::Vector3f;
using FMatrix = Eigen::Matrix4f;

namespace FMath {
	template<typename T>
	struct is_numeric{
		static constexpr bool value =
			(std::is_integral<T>::value &&
				!std::is_same<T, bool>::value &&
				!std::is_same<T, char>::value &&
				!std::is_same<T, signed char>::value &&
				!std::is_same<T, unsigned char>::value)
			||
			std::is_floating_point<T>::value;
	};

	template<typename T>
	T Clamp(T value, T minVal, T maxVal){
		static_assert(is_numeric<T>::value,
			"FMath::Clamp requires numeric types (int/float/double...)");

		if (value < minVal) return minVal;
		if (value > maxVal) return maxVal;
		return value;
	}
	
	// 单个标量的最小/最大值
	template<typename T>
	static T Min(const T& a, const T& b) {
		static_assert(is_numeric<T>::value,
			"FMath::Clamp requires numeric types (int/float/double...)");

		return std::min(a, b);
	}

	template<typename T>
	static T Max(const T& a, const T& b) {
		static_assert(is_numeric<T>::value,
			"FMath::Clamp requires numeric types (int/float/double...)");

		return std::max(a, b);
	}

}
