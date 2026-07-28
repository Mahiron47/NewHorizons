#ifndef IHVINSTACEMODULEHPP
#define IHVINSTACEMODULEHPP

#include "vulkan/interfaces/IHVmodule.hpp"

interface IHVinstanceExtansions : public IHVmodule {
public:
    virtual const char* get_extension_name() const = 0;

};

interface IHVinstanceLayers : public IHVmodule {
public:
    virtual const char* get_layer_name() const = 0;
    
};

#endif //IHVINSTACEMODULEHPP