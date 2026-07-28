#ifndef ARITHMETICHPP
#define ARITHMETICHPP

#include <concepts>

template<typename T> concept ArithmeticComparable = requires(T a, T b) {
	//{ a + b } -> std::convertible_to<T>;
	//{ a - b } -> std::convertible_to<T>;
	{ a * b } -> std::convertible_to<T>;
	{ a / b } -> std::convertible_to<T>;
	{ a < b } -> std::convertible_to<bool>;
	{ a > b } -> std::convertible_to<bool>;
	{ a <= b } -> std::convertible_to<bool>;
	{ a >= b } -> std::convertible_to<bool>;
	{ a == b } -> std::convertible_to<bool>;
	{ a != b } -> std::convertible_to<bool>;
};

#endif //ARITHMETICHPP