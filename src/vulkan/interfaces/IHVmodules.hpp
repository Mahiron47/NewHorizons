#ifndef IHVMODULESHPP
#define IHVMODULESHPP

#include "vulkan/interfaces/IHVmodule.hpp"
#include "memory/Hstack.hpp"

interface IHVmodules {
    const char** _arr;

    void init_arr() {
        for (uint32_t i = 0; i < _modules_list.size(); i++) {
            _arr[i] = _modules_list[i]->get_module_name();
        }
    }
    
protected:
	Hstack<uint32_t, IHVmodule*> _modules_list;
    
    // default constructor
    template<class... Args> requires(std::derived_from<std::remove_reference_t<Args>, IHVmodule> && ...)
    IHVmodules(Args&&... args) : _modules_list(args.initialize()...),
                                 _arr(new const char*[_modules_list.size()]) {}

    void initialize() {
        this->init_modules_list();
        this->init_arr();
    }

    virtual void init_modules_list() = 0;
    
public:    
    virtual ~IHVmodules() {
        this->_modules_list.do_each([](IHVmodule*& val) -> void {
            delete val;
        });
        
        if (_arr != nullptr) delete[] _arr;
    }
    
// move constructor and assignment operator
    IHVmodules(IHVmodules&& other) noexcept : _modules_list(static_cast<Hstack<uint32_t, IHVmodule*>&&>(other._modules_list)), 
                                              _arr(other._arr) {
        other._modules_list = nullptr;
        other._arr = nullptr;
    }
    IHVmodules& operator=(IHVmodules&& other) noexcept {
        if (this == &other) return *this;

        this->_modules_list = static_cast<Hstack<uint32_t, IHVmodule*>&&>(other._modules_list);
        this->_arr = other._arr;

        other._modules_list = nullptr;
        other._arr = nullptr;

        return *this;
    }

// getters
    uint32_t get_size() const {
        return _modules_list.size();
    }
    const char** get_data() const {
        return _arr;
    }
};

#endif //IHVMODULESHPP