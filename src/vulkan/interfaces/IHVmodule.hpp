#ifndef IHVMODULEHPP
#define IHVMODULEHPP

#include "Horizon.hpp"

struct IHVinstanceLayersPtr;

interface IHVmodule {
    bool val_accesability = false;
public:
    virtual bool get_accesability() const {
        return val_accesability;
    }
    virtual void set_accesability(bool val) {
        val_accesability = val;
    }
    virtual IHVinstanceLayersPtr&& initialize() const = 0;

};

#endif // IHVMODULEHPP