#ifndef ABILITYHPP
#define AbILITYHPP

#include "concepts"

template<typename T> concept Moveable = requires(T type, T&& rvalue) {
    rvalue = type;
};

#endif //ABILITYHPP