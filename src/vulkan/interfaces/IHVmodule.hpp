#ifndef IHVMODULEHPP
#define IHVMODULEHPP

#include "Horizon.hpp"

interface IHVmodule {
    bool val_accesability = false;

public:
    bool get_accesability() const {
        return val_accesability;
    }
    void set_accesability(bool val) {
        val_accesability = val;
    }
    virtual const char* get_module_name() const = 0;
    virtual IHVmodule* initialize() const = 0;
};

#endif // IHVMODULEHPP