#ifndef HV_STACK_HPP
#define HV_STACK_HPP

#include "Horizon.hpp"

#include <concepts>
#include <stdexcept>

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

template<ArithmeticComparable SizeType, typename ValueType> class Hstack {
	SizeType point;
	SizeType size;
	ValueType* data;

	void extend_size() {
		if (data == nullptr) {
			size = 1;
			data = new ValueType[size];
			return;
		}

		SizeType new_size = size * 2;
		ValueType* new_data = new ValueType[new_size];
		
		for (SizeType i = 0; i < size; i++) {
			new_data[i] = static_cast<ValueType&&>(data[i]);
		}
		delete[] data;
		data = new_data;
		size = new_size;
	}

	void reduce_size() {
		if (data == nullptr || size == 1) {
			return;
		}

		SizeType new_size = size / 2;
		ValueType* new_data = new ValueType[new_size];
		
		for (SizeType i = 0; i < new_size; i++) {
			new_data[i] = static_cast<ValueType&&>(data[i]);
		}
		delete[] data;
		data = new_data;
		size = new_size;
	}

public:
	Hstack() : point(0), size(0), data(nullptr) {}
	Hstack(SizeType size) : point(0), size(size), data(new ValueType[size]) {}
	~Hstack() { delete[] data; }

	void push(const ValueType& value) {
		if (point == size) {
			this->extend_size();
		}
		data[point++] = value;
	}

    void push(ValueType&& value) {
        if (point == size) {
            this->extend_size();
        }
        data[point++] = static_cast<ValueType&&>(value);
    }

	ValueType pop() {
		if (point == 0) {
			throw std::out_of_range("Hstack::pop");
		} if (point > 0 && point == size / 4) {
			this->reduce_size();
		}
		
		return static_cast<ValueType&&>(data[--point]);
	}

	ValueType& last() {
		if (point == 0) {
			throw std::out_of_range("Hstack::last");
		}
		return data[point - 1];
	}

	void del() {
		delete[] data;
		data = nullptr;
		size = 0;
		point = 0;
	}

	void clear() {
		point = 0;
	}

	template<typename Func> requires requires(Func f, ValueType& v) {
    	{ f(v) } -> std::convertible_to<void>;
	} void do_each(Func&& func) {
		for (SizeType i = 0; i < point; i++) {
			func(data[i]);
		}
	}

	template<typename Func> requires requires(Func f, ValueType& v) {
    	{ f(v) } -> std::convertible_to<bool>;
	} void do_each(Func&& func, ValueType*& value) {
		for (SizeType i = 0; i < point; i++) { 
			if (func(data[i])) { 
				value = &data[i];
			}
		}
	}

	ValueType& operator[](SizeType index) {
		if (index >= point) {
			throw std::out_of_range("Hstack::operator[]");
		}
		return data[index];
	}
};

#endif // HV_STACK_HPP