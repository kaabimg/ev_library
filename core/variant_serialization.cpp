#include "variant_serialization.h"


#include <sstream>

using namespace ev::core;
#include "logging.h"


namespace ev { namespace core { namespace details {



template <typename T>
std::stringstream & operator << (std::stringstream &ss,const T & data){
    ss.write(reinterpret_cast<const char *>(&data),sizeof(T));
    return ss;
}

template <>
std::stringstream & operator << (std::stringstream &ss,const std::string & data){
    ss.write(data.c_str(),data.size());
    return ss;
}


//struct save_helper_t {

//    const variant_t & variant;

//    save_helper_t(const variant_t & var):variant(var){}


//    void serialize(std::stringstream & ar){
//        serialize(ar,variant);
//    }

//    void serialize(std::stringstream & ar,const variant_t & variant)
//    {
//        variant_type_e type = variant_type(variant);
//        ar << (uint8)type;

//        switch (type) {
//        case variant_type_e::boolean:
//            ar << variant_as<bool>(variant);
//            break;
//        case variant_type_e::integer:
//            ar << variant_as<int32>(variant);
//            break;
//        case variant_type_e::number:
//            ar << variant_as<double>(variant);
//            break;
//        case variant_type_e::string:
//            ar << variant_as<std::string>(variant);
//            break;
//        case variant_type_e::variant_array:
//            serialize(ar,variant_as<variant_array_t>(variant));
//            break;
//        case variant_type_e::variant_map:
//            serialize(ar,variant_as<variant_map_t>(variant));
//            break;
//        default:
//            break;
//        }
//    }


//    void serialize(std::stringstream & ar, const variant_array_t & array){
//        ar << array.size();

//        for (size_t i = 0; i < array.size(); ++i) {
//            serialize(ar, array[i]);
//        }
//    }

//    void serialize(std::stringstream & ar,const variant_map_t & map){
//        ar << map.size();
//        variant_map_t::const_iterator i = map.begin();

//        while(i != map.end()){
//            ar << i->first;
//            serialize(ar, i->second);
//            ++i;
//        }
//    }


//};


}}}



std::string variant_serializer_t::save(const ev::core::variant_t& /*variant*/)
{
    std::stringstream ss;
//    details::save_helper_t(variant).serialize(ss);
    return ss.str();
}

variant_t variant_serializer_t::load(const std::string& /*data*/)
{
    return  variant_t();
}
