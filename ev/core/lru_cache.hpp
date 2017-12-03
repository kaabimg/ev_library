#pragma once

#include <unordered_map>
#include <list>
#include <memory>

#include <ev/core/logging.hpp>
namespace ev {

template <typename Key, typename T>
class lru_cache {
public:
    lru_cache(std::size_t capacity);

    std::shared_ptr<T> insert(const Key&, T*);
    std::shared_ptr<T> insert(const Key&, const T&);
    std::shared_ptr<T> insert(const Key&, T&&);
    std::shared_ptr<T> insert(const Key&, std::shared_ptr<T>);

    std::shared_ptr<T> get(const Key&) const;
    bool contains(const Key&) const;
    bool remove(const Key&);

    size_t size() const;
    size_t capacity() const;
    bool empty() const;

    void set_capacity(std::size_t capacity);

private:
    void evict_if_full();

private:
    using key_type = Key;

    using list_type = typename std::list<key_type>;
    using list_iterator = typename list_type::iterator;

    struct node_t {
        std::shared_ptr<T> data;
        mutable list_iterator pos;
    };

    using map_type = typename std::unordered_map<key_type, node_t>;

    map_type _data_map;
    mutable list_type _data_list;
    std::size_t _capacity;
};

template <typename Key, typename T>
inline lru_cache<Key, T>::lru_cache(std::size_t capacity)
    : _capacity(std::max<size_t>(capacity, 1))
{
}

template <typename Key, typename T>
inline std::shared_ptr<T> lru_cache<Key, T>::insert(const Key& key, T* d)
{
    return insert(key, std::shared_ptr<T>{d});
}

template <typename Key, typename T>
inline std::shared_ptr<T> lru_cache<Key, T>::insert(const Key& key, const T& d)
{
    return insert(key, std::make_shared<T>(d));
}

template <typename Key, typename T>
inline std::shared_ptr<T> lru_cache<Key, T>::insert(const Key& key, T&& d)
{
    return insert(key, std::make_shared<T>(std::move(d)));
}

template <typename Key, typename T>
inline std::shared_ptr<T> lru_cache<Key, T>::insert(const Key& key, std::shared_ptr<T> dptr)
{
    typename map_type::iterator i = _data_map.find(key);

    if (i == _data_map.end()) {
        // insert the new item
        auto ret = _data_map.insert({key, {dptr, _data_list.end()}});
        if (!ret.second) return nullptr;
        _data_list.push_front(key);
        _data_map[key].pos = _data_list.begin();
    }
    else {
        _data_list.erase(i->second.pos);
        _data_list.push_front(key);
        _data_map[key] = {dptr, _data_list.begin()};
    }
    evict_if_full();

    return dptr;
}

template <typename Key, typename T>
inline std::shared_ptr<T> lru_cache<Key, T>::get(const Key& key) const
{
    auto iter = _data_map.find(key);
    if (iter != _data_map.end()) {
        // make this value hot
        _data_list.erase(iter->second.pos);
        _data_list.push_front(key);
        iter->second.pos = _data_list.begin();
        return iter->second.data;
    }
    return nullptr;
}

template <typename Key, typename T>
inline bool lru_cache<Key, T>::remove(const Key& key)
{
    auto pos = _data_map.find(key);
    if (pos) {
        _data_list.erase(pos->second);
        _data_map.erase(key);
        return true;
    }
    return false;
}

template <typename Key, typename T>
inline size_t lru_cache<Key, T>::size() const
{
    return _data_list.size();
}

template <typename Key, typename T>
inline size_t lru_cache<Key, T>::capacity() const
{
    return _capacity;
}

template <typename Key, typename T>
inline bool lru_cache<Key, T>::empty() const
{
    return _data_list.empty();
}

template <typename Key, typename T>
inline bool lru_cache<Key, T>::contains(const Key& key) const
{
    return _data_map.find(key) != _data_map.end();
}

template <typename Key, typename T>
inline void lru_cache<Key, T>::set_capacity(std::size_t capacity)
{
    _capacity = capacity;
    while (size() > _capacity) {
        evict_if_full();
    }
}

template <typename Key, typename T>
inline void lru_cache<Key, T>::evict_if_full()
{
    if (size() > _capacity) {
        _data_map.erase(_data_list.back());
        _data_list.pop_back();
    }
}
}
