#ifndef HSTACKHPP
#define HSTACKHPP

template<typename SizeType, typename ValueType> requires requires(SizeType a, SizeType b) {
	{ a * b } -> std::convertible_to<SizeType>;
	{ a / b } -> std::convertible_to<SizeType>;
	{ a < b } -> std::convertible_to<bool>;
	{ a > b } -> std::convertible_to<bool>;
	{ a <= b } -> std::convertible_to<bool>;
	{ a >= b } -> std::convertible_to<bool>;
	{ a == b } -> std::convertible_to<bool>;
	{ a != b } -> std::convertible_to<bool>;
} class Hstack {
	SizeType   _pt;
	SizeType   _sz;
	ValueType* _dt;

	void extend_size();
	void reduce_size();

public:
    Hstack() : _pt(0),
               _sz(0),
               _dt(nullptr) {}
    Hstack(SizeType sz) : _pt(sz),
                          _sz(sz),
                          _dt(new ValueType[sz]) {
        for (SizeType i = 0; i < this->size(); i++) {
            _dt[i] = ValueType();
        }
    }
	Hstack(SizeType sz, ValueType* ptr_arr) : _pt(sz),
											  _sz(sz),
											  _dt(ptr_arr) {}
    template<typename... Args> requires (std::convertible_to<Args, ValueType> && ...)
	Hstack(Args&&... values) : _pt(sizeof...(values)), 
							   _sz(sizeof...(values)), 
							   _dt(new ValueType[sizeof...(values)]) {
		uint32_t index = 0;
		
		((this->_dt[index++] = ValueType(std::forward<ValueType>(values))), ...);
	}

	template<typename T> requires std::convertible_to<T, ValueType>
	void push(T&& value) {
        if (_pt == _sz) this->extend_size();
        
        _dt[_pt++] = std::forward<T>(value);	
	}
	ValueType& pop() {
		if (_pt == 0) throw std::out_of_range("Hstack::pop");
		
		while (_pt <= _sz / 4 && _sz > 1) this->reduce_size();

		_pt--;

		return _dt[_pt];
	}
	ValueType& last() {
		if (_pt <= 0) throw std::out_of_range("Hstack::last");
		
		return _dt[_pt - 1];
	}
	[[nodiscard]] constexpr SizeType size() const noexcept {
		return _pt;
	}
	[[nodiscard]] ValueType* data() const noexcept {
		return _dt;
	}
	void del() {
		delete[] _dt;

		_pt = 0;
		_sz = 0;
		_dt = nullptr;
	}
	constexpr void clear() noexcept {
		_pt = 0;
	}

	template<typename Func, typename... Args> requires requires(Func f, ValueType v, Args... args) {
    	{ f(v, args...) } -> std::convertible_to<void>;
	} void do_each(Func func, const Args&... args) const {
		for (SizeType i = 0; i < this->size(); i++) {
			func(_dt[i], args... );
		}
	}
	template<typename Func, typename... Args> requires requires(Func f, ValueType v, Args... args) {
    	{ f(v, args...) } -> std::convertible_to<bool>;
	} void do_each(Func func, ValueType** value, const Args&... args) const {
		for (SizeType i = 0; i < this->size(); i++) { 
			if (func(_dt[i], args... )) { 
				*value = &_dt[i];
			}
		}
	}
	template<typename Func, typename... Args> requires requires(Func f, ValueType v, Args... args) {
    	{ f(v, args...) } -> std::convertible_to<bool>;
	} ValueType* do_each(Func func, const Args&... args) const {
		for (SizeType i = 0; i < this->size(); i++) { 
			if (func(_dt[i], args... )) { 
				return &_dt[i];
			}
		}
		return nullptr;
	}

	ValueType& operator[](SizeType index) {
		if (index >= _pt) throw std::out_of_range("Hstack::operator[]");
		
		return _dt[index];
	}

	const ValueType& operator[](SizeType index) const {
		if (index >= _pt) throw std::out_of_range("Hstack::operator[]");

		return _dt[index];
	}

   ~Hstack() { 
		delete[] _dt; 
	}
    Hstack(const Hstack& other) : _pt(other._pt),
                                  _sz(other._sz),
                                  _dt(new ValueType[other._sz]) {
        for (SizeType i = 0; i < this->size(); i++) {
            _dt[i] = ValueType(std::forward<ValueType>(other._dt[i]));
        }
    }
    Hstack(Hstack&& other) noexcept : _pt(other._pt),
                                      _sz(other._sz),
                                      _dt(other._dt) {
        other._pt = 0;
		other._sz = 0;
		other._dt = nullptr;
    }
	Hstack& operator=(const Hstack& other) {
		if (this == &other) return *this;

		this->_pt = other._pt;
		this->_sz = other._sz;
		this->_dt = new ValueType[other._sz];

		for (SizeType i = 0; i < this->size(); i++) {
			_dt[i] = std::forward<ValueType>(other._dt[i]);
		}

		return *this;
	}
	Hstack& operator=(Hstack&& other) noexcept {
		if (this == &other) return *this;

		this->_pt = other._pt;
		this->_sz = other._sz;
		this->_dt = other._dt;

		other._pt = 0;
		other._sz = 0;
		other._dt = nullptr;

		return *this;
	}
};

template<typename SizeType, typename ValueType> requires requires(SizeType a, SizeType b) {
	{ a * b } -> std::convertible_to<SizeType>;
	{ a / b } -> std::convertible_to<SizeType>;
	{ a < b } -> std::convertible_to<bool>;
	{ a > b } -> std::convertible_to<bool>;
	{ a <= b } -> std::convertible_to<bool>;
	{ a >= b } -> std::convertible_to<bool>;
	{ a == b } -> std::convertible_to<bool>;
	{ a != b } -> std::convertible_to<bool>;
} inline void Hstack<SizeType, ValueType>::extend_size() {
    if (_dt == nullptr) {
		_pt = 0;
        _sz = 1;
        _dt = new ValueType[_sz];
        return;
    }

    SizeType new_sz = _sz * 2;
    ValueType* new_data = new ValueType[new_sz];

    for (SizeType i = 0; i < this->size(); i++) {
        new_data[i] = std::forward<ValueType>(_dt[i]);
    }

	delete[] _dt;

    _dt = new_data;
    _sz = new_sz;
}

template<typename SizeType, typename ValueType> requires requires(SizeType a, SizeType b) {
	{ a * b } -> std::convertible_to<SizeType>;
	{ a / b } -> std::convertible_to<SizeType>;
	{ a < b } -> std::convertible_to<bool>;
	{ a > b } -> std::convertible_to<bool>;
	{ a <= b } -> std::convertible_to<bool>;
	{ a >= b } -> std::convertible_to<bool>;
	{ a == b } -> std::convertible_to<bool>;
	{ a != b } -> std::convertible_to<bool>;
} inline void Hstack<SizeType, ValueType>::reduce_size() {
    if (_sz <= 1) return;

    SizeType new_sz = _sz / 2;
    ValueType* new_data = new ValueType[new_sz];

    for (SizeType i = 0; i < this->size(); i++) {
        new_data[i] = std::forward<ValueType>(_dt[i]);
    }

    delete[] _dt;

    _dt = new_data;
    _sz = new_sz;
}

#endif // HSTACKHPP