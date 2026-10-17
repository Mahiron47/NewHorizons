#ifndef IHVMODULEHPP
#define IHVMODULEHPP

class IHVmodule {
    bool val_accesability = false;

public:
    bool get_accesability() const {
        return val_accesability;
    }
    void set_accesability(bool val) {
        val_accesability = val;
    }
    
    virtual ~IHVmodule() {}
    
    virtual const char* get_module_name() const = 0;
    virtual IHVmodule* initialize() const = 0;

    bool operator==(const IHVmodule& other) const {
        return strcmp(this->get_module_name(), other.get_module_name()) == 0;
    }
};

#endif // IHVMODULEHPP