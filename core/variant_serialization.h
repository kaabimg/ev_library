#ifndef EV_VARIANT_SERIALIZATION_H
#define EV_VARIANT_SERIALIZATION_H

#include "variant.h"

namespace ev { namespace core {


struct variant_serializer_t {

    static std::string save(const variant_t & variant);
    static variant_t load(const std::string & data );

};

}

}

#endif // EV_VARIANT_SERIALIZATION_H
