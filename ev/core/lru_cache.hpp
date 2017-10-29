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

    map_type m_data_map;
    mutable list_type m_data_list;
    std::size_t m_capacity;
};

template <typename Key, typename T>
inline lru_cache<Key, T>::lru_cache(std::size_t capacity)
    : m_capacity(std::max<size_t>(capacity, 1))
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
    typename map_type::iterator i = m_data_map.find(key);

    if (i == m_data_map.end()) {
        // insert the new item
        auto ret = m_data_map.insert({key, {dptr, m_data_list.end()}});
        if (!ret.second) return nullptr;
        m_data_list.push_front(key);
        m_data_map[key].pos = m_data_list.begin();
    }
    else {
        m_data_list.erase(i->second.pos);
        m_data_list.push_front(key);
        m_data_map[key] = {dptr, m_data_list.begin()};
    }
    evict_if_full();

    return dptr;
}

template <typename Key, typename T>
inline std::shared_ptr<T> lru_cache<Key, T>::get(const Key& key) const
{
    auto iter = m_data_map.find(key);
    if (iter != m_data_map.end()) {
        // make this value hot
        m_data_list.erase(iter->second.pos);
        m_data_list.push_front(key);
        iter->second.pos = m_data_list.begin();
        return iter->second.data;
    }
    return nullptr;
}

template <typename Key, typename T>
inline bool lru_cache<Key, T>::remove(const Key& key)
{
    auto pos = m_data_map.find(key);
    if (pos) {
        m_data_list.erase(pos->second);
        m_data_map.erase(key);
        return true;
    }
    return false;
}

template <typename Key, typename T>
inline size_t lru_cache<Key, T>::size() const
{
    return m_data_list.size();
}

template <typename Key, typename T>
inline size_t lru_cache<Key, T>::capacity() const
{
    return m_capacity;
}

template <typename Key, typename T>
inline bool lru_cache<Key, T>::empty() const
{
    return m_data_list.empty();
}

template <typename Key, typename T>
inline bool lru_cache<Key, T>::contains(const Key& key) const
{
    return m_data_map.find(key) != m_data_map.end();
}

template <typename Key, typename T>
inline void lru_cache<Key, T>::set_capacity(std::size_t capacity)
{
    m_capacity = capacity;
    while (size() > m_capacity) {
        evict_if_full();
    }
}

template <typename Key, typename T>
inline void lru_cache<Key, T>::evict_if_full()
{
    if (size() > m_capacity) {
        m_data_map.erase(m_data_list.back());
        m_data_list.pop_back();
    }
}
}
