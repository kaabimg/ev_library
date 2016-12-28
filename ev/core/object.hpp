#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include "preprocessor.h"

namespace ev {

struct object_t {

    object_t(object_t* parent = nullptr);
    virtual ~object_t();

    void set_parent(object_t * parent);

private:
    void add_child(object_t*);
    void remove_child(object_t*);

private:

    object_t* m_parent = nullptr;
    std::vector<object_t*> m_children;
};

object_t::object_t(object_t *parent)
{
    set_parent(parent);
}

object_t::~object_t()
{
    if(m_parent) m_parent->remove_child(this);
    ev_delete_all(m_children);

}

void object_t::set_parent(object_t *parent)
{
    if(m_parent) m_parent->remove_child(this);
    m_parent = parent;
    if(m_parent) m_parent->add_child(this);
}

void object_t::add_child(object_t * c)
{
    m_children.push_back(c);
}

void object_t::remove_child(object_t * c)
{
    auto it = std::find(m_children.begin(), m_children.end(), c);
    if(it != m_children.end())m_children.erase(it);
}



template <typename T>
struct property_t {
    virtual T read(object_t* object)const=0;
    virtual void write(object_t*, const T & v)const=0;
};


template <typename T>
struct member_property_t : property_t<T> {


    using Reader = std::function<T (ev::object_t*)>;
    using Writer = std::function<void (ev::object_t*,const T &)>;

    const Reader reader;
    const Writer writer;


    member_property_t(Reader && r,Writer && w):
        reader(std::forward<Reader>(r)),
        writer(std::forward<Writer>(w)){}

    T read(object_t* object)const{return reader(object);}
    void write(object_t* object, const T & v)const{writer(object,v);}
};


template <typename T>
auto make_member_property(auto && reader,auto && writer) {
    return member_property_t<T>(std::move(reader),std::move(writer));
}



} // ev

