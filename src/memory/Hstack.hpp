#ifndef HVSTACKHPP
#define HVSTACKHPP

#include "Horizon.hpp"
#include "concepts/Arithmetic.hpp"

template<ArithmeticComparable SizeType, typename ValueType> class Hstack {
	SizeType point;
	SizeType sz;
	ValueType* data;

	void extend_size() {
		if (this->data == nullptr) {
			this->sz = 1;
			this->data = new ValueType[this->sz];
			return;
		}

		SizeType new_size = this->sz * 2;
		ValueType* new_data = new ValueType[new_size];
		
		for (SizeType i = 0; i < this->sz; i++) {
			new_data[i] = static_cast<ValueType&&>(this->data[i]);
		}
		delete[] this->data;
		this->data = new_data;
		this->sz = new_size;
	}

	void reduce_size() {
		if (this->data == nullptr || this->sz == 1) {
			return;
		}

		SizeType new_size = this->sz / 2;
		ValueType* new_data = new ValueType[new_size];
		
		for (SizeType i = 0; i < new_size; i++) {
			new_data[i] = static_cast<ValueType&&>(this->data[i]);
		}
		delete[] this->data;
		this->data = new_data;
		this->sz = new_size;
	}

public:
	Hstack() : point(0), sz(0), data(nullptr) {}
	Hstack(const SizeType& sz) : point(0), sz(sz), data(new ValueType[sz]) {}
	Hstack(const Hstack& other) : point(other.point), sz(other.sz), data(new ValueType[other.sz]) {
		for (SizeType i = 0; i < other.point; i++) {
			data[i] = other.data[i];
		}
	}
	Hstack(Hstack&& other) noexcept : point(other.point), sz(other.sz), data(&other.data) {
		other.point = 0;
		other.sz = 0;
		other.data = nullptr;
	}
	template<typename... Args> requires (std::convertible_to<Args, ValueType> && ...)
	Hstack(Args&&... values) : point(sizeof...(values)), sz(sizeof...(values)), 
									   data(new ValueType[sizeof...(values)] {values...}) {}
	~Hstack() { delete[] data; }
	Hstack& operator=(const Hstack& other) {
		this->point = other.point;
		this->sz = other.sz;
		this->data = new ValueType[other.sz];
		for (SizeType i = 0; i < this->sz; i++) {
			this->data[i] = other.data[i];
		}

		return *this;
	}
	Hstack& operator=(Hstack&& other) {
		this->point = other.point;
		this->sz = other.sz;
		this->data = other.data;
		other.data = nullptr;

		return *this;
	}

	template<typename T> requires std::convertible_to<T, ValueType>
	void push(T&& value) {
        if (this->point == this->sz) {
            this->extend_size();
        }
        this->data[this->point++] = static_cast<ValueType&&>(value);
    }

	ValueType& pop() {
		if (this->point == 0) {
			throw std::out_of_range("Hstack::pop");
		} if (this->point > 0 && this->point == this->sz / 4) {
			this->reduce_size();
		}
		
		return this->data[--this->point];
	}

	ValueType& last() {
		if (this->point == 0) {
			throw std::out_of_range("Hstack::last");
		}
		return this->data[this->point - 1];
	}

	SizeType size() const {
		return this->sz;
	}

	void del() {
		delete[] this->data;
		this->data = nullptr;
		this->sz = 0;
		this->point = 0;
	}

	void clear() {
		this->point = 0;
	}

	template<typename Func, typename... Args> requires requires(Func f, ValueType v, Args... args) {
    	{ f(v, args...) } -> std::convertible_to<void>;
	} void do_each(Func&& func, Args&&... args) {
		for (SizeType i = 0; i < this->point; i++) {
			func(this->data[i], args...);
		}
	}

	template<typename Func, typename... Args> requires requires(Func f, ValueType v, Args... args) {
    	{ f(v, args...) } -> std::convertible_to<bool>;
	} void do_each(Func&& func, ValueType* value, Args&&... args) {
		for (SizeType i = 0; i < this->point; i++) { 
			if (func(this->data[i], args...)) { 
				value = &this->data[i];
			}
		}
	}

	template<typename Func, typename... Args> requires requires(Func f, ValueType v, Args... args) {
    	{ f(v, args...) } -> std::convertible_to<bool>;
	} ValueType* do_each(Func&& func, Args&&... args) {
		for (SizeType i = 0; i < this->point; i++) { 
			if (func(this->data[i], args...)) { 
				return &this->data[i];
			}
		}
		return nullptr;
	}

	ValueType& operator[](const SizeType& index) const {
		if (index >= this->point) {
			throw std::out_of_range("Hstack::operator[]");
		}
		return this->data[index];
	}
};

#endif // HVSTACKHPP