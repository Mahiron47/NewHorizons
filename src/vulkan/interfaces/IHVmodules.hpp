#ifndef IHVMODULESHPP
#define IHVMODULESHPP

#include "vulkan/interfaces/IHVmodule.hpp"

class IHVmodules {
    const char** _arr;

    void init_arr() {
        for (uint32_t i = 0; i < _modules_list.size(); i++) {
            _arr[i] = _modules_list[i]->get_module_name();
    }   }
    
protected:
	Hstack<uint32_t, IHVmodule*> _modules_list;
    
    // default constructor
    template<class... Args> requires(std::derived_from<std::remove_reference_t<Args>, IHVmodule> && ...)
    IHVmodules(Args&&... args) : _modules_list(std::forward<Args>(args).initialize()...),
                                 _arr(new const char*[sizeof...(args)]) {}

    void initialize() {
        this->init_modules_list();
        this->init_arr();
    }

    virtual void init_modules_list() = 0;
    
public:    
// move constructor and assignment operator
    IHVmodules(IHVmodules&& other) noexcept : _modules_list(std::move(other._modules_list)), 
                                              _arr(other._arr) {
        other._modules_list = nullptr;
        other._arr = nullptr;
    }
    IHVmodules& operator=(IHVmodules&& other) noexcept {
        if (this == &other) return *this;

        this->_modules_list = std::move(other._modules_list);
        this->_arr = other._arr;

        other._modules_list = nullptr;
        other._arr = nullptr;
        
        return *this;
    }

// getters
    constexpr uint32_t get_size() const {
        return _modules_list.size();
    }
    virtual const char** get_data() const {
        return _arr;
    }
    constexpr const Hstack<uint32_t, IHVmodule*>& get_modules_list() const {
        return _modules_list;
    }

    template<class Args> requires std::derived_from<std::remove_reference_t<Args>, IHVmodule>
    bool check_module(Args&& module) const {
        return _modules_list.do_each([](IHVmodule*& val, const IHVmodule& module) -> bool {
            return *val == module;
        }, module) != nullptr;
    }
    template<class Args> requires std::derived_from<std::remove_reference_t<Args>, IHVmodule>
    IHVmodule* get_module(Args&& module) const {
        return _modules_list.do_each([](IHVmodule*& val, const IHVmodule& module) -> bool {
            return *val == module;
        }, module);
    } 
    
    virtual ~IHVmodules() {
        this->_modules_list.do_each([](IHVmodule*& val) -> void {
            delete val;
        });
        
        if (_arr != nullptr) delete[] _arr;
    }
};

#endif //IHVMODULESHPP